//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// NTP: sincroniza RTC interno do ESP32 (UTC-3)
//******************************************************************************************************************

#include <FastLED.h>
extern CRGB* leds;
extern long stripPixels;
extern int speed;
void set_strip_pixels(long pixels);

void syncRTCWithNTP() {
  // Offset de fuso horário: -3 horas (Brasil), sem DST
  const long gmtOffset_sec = -3 * 3600;
  const int daylightOffset_sec = 0;

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  bool ok = false;

  // Tenta obter hora local até 10 vezes
  for (int i = 0; i < 10; i++) {

    if (getLocalTime(&timeinfo, 500)) { // aguarda até 500ms por tentativa
      
      ok = true;
      break;
    
    }
    
    vTaskDelay(50);
  
  }

  if (ok) {

    ESP32Time rtc;
    rtc.setTimeStruct(timeinfo);
    rtc_synced = true;
    last_rtc_sync_ms = millis();
    Serial.println("RTC synchronized via NTP (UTC-3).");
  
  } else {
  
    Serial.println("RTC sync failed: NTP not reachable.");
  
  }

}

//******************************************************************************************************************
// CONFIGURACAO DE REDE.HTML PAGE
//******************************************************************************************************************

void check_efeito(void) {
      
  if(digitalRead(BTN1)==LOW) { 

    delay(100);

    if(digitalRead(BTN1)==LOW) {  

      efeito++;

      if(efeito>TOTAL_EFEITOS) efeito=1;
      
    }

  }
    
}
//******************************************************************************************************************
// CHECK UPDATE
//******************************************************************************************************************

void check_update(void) {
  
  static uint8_t animIdx = 0;                         // índice na ordem de segmentos
  static const uint8_t animOrder[6] = {0,1,2,6,5,4};  // ordem solicitada, sem o meio (3)
  static unsigned long lastAnimUpdate = 0;
  static bool stripChecked = false;

  if (update_status) {
    // Garante strip suficiente para todos os dígitos, extras e indicadores (36 LEDs)
    if (!stripChecked) {

      long required = 36;
      
      if (stripPixels < required) {
      
        set_strip_pixels(required);
      
      }
      
      stripChecked = true;
    
    }

    // Mantém D1 (dezenas de minutos) apagado durante update
    for (uint8_t seg = 0; seg < 7; seg++) {
      if (stripPixels > (7 + seg)) leds[7 + seg] = CRGB::Black;
    }

    // LEDs extras (+14, +17) e dois pontos (+15, +16) apagados
    if (stripPixels > 14) leds[14] = CRGB::Black;
    if (stripPixels > 15) leds[15] = CRGB::Black;
    if (stripPixels > 16) leds[16] = CRGB::Black;
    if (stripPixels > 17) leds[17] = CRGB::Black;

    // LEDs finais (+32..+35) apagados
    if (stripPixels > 32) leds[32] = CRGB::Black;
    if (stripPixels > 33) leds[33] = CRGB::Black;
    if (stripPixels > 34) leds[34] = CRGB::Black;
    if (stripPixels > 35) leds[35] = CRGB::Black;

    // Letras durante update:
    // D3 (startLed+25) = 'F' | D2 (startLed+18) = 'U'
    // Mapeamento de segmentos: 0=SupDir,1=Topo,2=SupEsq,3=Meio,4=InfDir,5=Base,6=InfEsq
    const uint16_t startThousands = 25;  // D3
    const uint16_t startHundreds  = 18;  // D2
    for (uint8_t s = 0; s < 7; s++) {
      if (stripPixels > (startThousands + s)) {
        bool onF = (s == 1) || (s == 2) || (s == 3) || (s == 6);
        leds[startThousands + s] = onF ? CRGB::Red : CRGB::Black;
      }
      if (stripPixels > (startHundreds + s)) {
        bool onU = (s == 0) || (s == 2) || (s == 6) || (s == 5) || (s == 4);
        leds[startHundreds + s] = onU ? CRGB::Red : CRGB::Black;
      }
    }

    // Animação: acende um segmento por vez em vermelho no primeiro dígito
    if (millis() - lastAnimUpdate >= (unsigned long)(speed * 100)) { // ~1000ms com speed=10
      // Apaga todos os segmentos do primeiro dígito (apenas dígito 0)
      for (uint8_t s = 0; s < 7; s++) {
        leds[0 + s] = CRGB::Black;
      }
      // Acende segmento atual em vermelho
      leds[0 + animOrder[animIdx]] = CRGB::Red;

      animIdx = (animIdx + 1) % 6;
      lastAnimUpdate = millis();
      // Garante strip2 apagado durante o update
      fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
      FastLED.show();
    }

    // Mantém LED1 como indicador (opcional): comentar se não desejar
    if((millis()-update_interval_ms)>=100) {
      if(digitalRead(LED1)==LOW) digitalWrite(LED1,HIGH);
      else digitalWrite(LED1,LOW);
      update_interval_ms=millis();
    }
  }

}

//******************************************************************************************************************
// ENVIA VERSAO DO FIRMWARE
//******************************************************************************************************************

void envia_firmware(void) {
 
  String aux_aux;

  aux_aux=FIRMWARE_VERSION; 
  String aux_firmware;
  
  aux_firmware="{\"command\":\"get_unique_id\",\"build\":\""+aux_aux+"\",\"type\":1}";

  if(pkgbuffer[read_pkg].type=="UDP") { 

    send_message_udp(aux_firmware,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

  } else {

    String rc(aux_firmware);
    char aux_aux[300];

    rc.toCharArray(aux_aux,rc.length()+1);  

    pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
    pkgbuffer[read_pkg].client->send();

  }    
  
}

//******************************************************************************************************************
// RESETA GTW
//******************************************************************************************************************

void rip(void) {

  String aux_rip;

  Serial2.print("@{\"command\":\"resetRadio\",\"type\":0}#");  

  aux_rip="{\"command\":\"RIP\",\"type\":1}";

  if(pkgbuffer[read_pkg].type=="UDP") { 
  
    send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

  } else {

    String rc(aux_rip);
    char aux_aux[300];

    rc.toCharArray(aux_aux,rc.length()+1);  

    pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
    pkgbuffer[read_pkg].client->send();

  }

  delay(1000);
  
  ESP.restart();
  
}

//******************************************************************************************************************
