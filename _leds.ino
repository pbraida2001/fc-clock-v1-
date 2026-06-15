//******************************************************************************************************************
// DISPLAYS DE 7 SEGMENTOS COM FASTLED
//******************************************************************************************************************

// Variável global para armazenar a cor atual da fita
CRGB currentStripColor = CRGB::White;

//******************************************************************************************************************
// SEGUNDO CANAL FASTLED (IO 22, 36 LEDs)
//******************************************************************************************************************

#define STRIP2_PIN    22
#define STRIP2_LEDS   36

CRGB leds2[STRIP2_LEDS];

// Chame uma vez no setup(), após set_strip()
void initStrip2() {
  FastLED.addLeds<LED_TYPE, STRIP2_PIN, COLOR_ORDER>(leds2, STRIP2_LEDS).setCorrection(TypicalLEDStrip);
  fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
  FastLED.show();
}

//******************************************************************************************************************
static bool clock12hMode = false;
static bool dateCycleMode = false;        // 0 = desliga ciclo de data, 1 = liga
static bool dateCycleActive = false;      // estado atual do ciclo
static uint8_t dateCyclePhase = 0;        // 0=none, 1=dd/mm, 2=aaaa
static unsigned long dateCyclePhaseStartMs = 0;
static unsigned long dateCycleLastStartMs = 0; // referência para intervalo de 15s

// RTC
#include <ESP32Time.h>
// Para consultar status do Wi-Fi
#include <WiFi.h>
extern bool rtc_synced;
extern bool update_status;
extern int stripBrightness;

// Controle de brilho automático (dia/noite)
static bool autoBrightnessEnabled = false;
static unsigned long lastAutoBrightnessCheckMs = 0;

// Define brilho com base no horário local: dia=255, noite=16
static void updateAutoBrightness(bool forceUpdate) {
  if (!autoBrightnessEnabled) return;

  // Evita checar com muita frequência (1 min), a menos que force
  if (!forceUpdate && (millis() - lastAutoBrightnessCheckMs) < 60000UL) return;

  // Se RTC ainda não sincronizado, use brilho seguro (dia) e aguarde sync
  if (!rtc_synced) {
    if (stripBrightness != 255) {
      stripBrightness = 255;
      setStripBrightness(255);
    }
    lastAutoBrightnessCheckMs = millis();
    return;
  }

  ESP32Time rtc;
  struct tm t = rtc.getTimeStruct();
  int hour = t.tm_hour; // 0..23

  // Dia: 07:00 - 19:59 | Noite: 20:00 - 06:59
  bool isDay = (hour >= 7 && hour < 20);
  int target = isDay ? 255 : 16;

  if (stripBrightness != target) {
    stripBrightness = target;
    setStripBrightness(target);
  }

  lastAutoBrightnessCheckMs = millis();
}

// Habilita (1) ou desabilita (0) controle de brilho automático
void setAutoBrightness(uint8_t enable) {
  autoBrightnessEnabled = (enable == 1);
  // Aplica imediatamente ao habilitar
  updateAutoBrightness(true);
}

// Padrões dos dígitos 0-9 para display de 7 segmentos
// Ordem dos LEDs: LED0=Superior Direito, LED1=Topo, LED2=Superior Esquerdo, LED3=Meio, LED4=Inferior Direito, LED5=Base, LED6=Inferior Esquerdo

const uint8_t digitPatterns[10] = {

  0b01110111,  // 0: LED0,1,2,4,5,6 = 0b01110111 (64+32+16+4+2+1 = 119)
  0b00010001,  // 1: LED0,4 = 0b00010001 (16+1 = 17)
  0b01101011,  // 2: LED1,0,3,6,5 = 0b01101011 (64+32+8+2+1 = 107) 
  0b00111011,  // 3: LED0,1,3,4,5 = 0b00111011 (32+16+8+2+1 = 59)
  0b00011101,  // 4: LED0,2,3,4 = 0b00011101 (16+8+4+1 = 29)
  0b00111110,  // 5: LED1,2,3,4,5 = 0b01101110 (Topo, Superior Esquerdo, Meio, Inferior Direito, Base)
  0b01111110,  // 6: LED1,2,3,4,5,6 = 0b01111110 (Topo, Superior Esquerdo, Meio, Inferior Direito, Base, Inferior Esquerdo)
  0b00010011,  // 7: LED0,1,4 = 0b00010011 (16+2+1 = 19)
  0b01111111,  // 8: todos = 0b01111111 (127)
  0b00111111   // 9: LED0,1,2,3,4,5 = 0b00111111 (32+16+8+4+2+1 = 63)

};


//******************************************************************************************************************
// SET STRIP COLOR
//******************************************************************************************************************

void setStripColor(uint8_t r, uint8_t g, uint8_t b) {

  // Atualizar a cor global do strip
  currentStripColor = CRGB(r, g, b);

  for (int i = 0; i < stripPixels; i++) {

    leds[i] = CRGB(r, g, b);

  }
  
  FastLED.show();

}

//******************************************************************************************************************
// SET STRIP BRIGHTNESS
//******************************************************************************************************************

void setStripBrightness(uint8_t brightness) {

  FastLED.setBrightness(brightness);
  FastLED.show();

}

//******************************************************************************************************************
// SET STRIP PIXELS
//******************************************************************************************************************

void set_strip_pixels(long pixels) {

  if (leds) {
  
    delete[] leds;
      
  }

  stripPixels = pixels;
  leds = new CRGB[stripPixels];

       if(dataPin==1) FastLED.addLeds<LED_TYPE,19, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else if(dataPin==2) FastLED.addLeds<LED_TYPE,18, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else if(dataPin==3) FastLED.addLeds<LED_TYPE,22, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else if(dataPin==4) FastLED.addLeds<LED_TYPE,21, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else FastLED.addLeds<LED_TYPE,19,COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);

}

//******************************************************************************************************************
// SET STRIP
//******************************************************************************************************************

void set_strip(long pixels, int brightness, uint8_t r, uint8_t g, uint8_t b) {

  if (leds) {
  
    delete[] leds;
      
  }

  stripPixels = pixels;
  leds = new CRGB[stripPixels];
  
       if(dataPin==1) FastLED.addLeds<LED_TYPE,19, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else if(dataPin==2) FastLED.addLeds<LED_TYPE,18, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else if(dataPin==3) FastLED.addLeds<LED_TYPE,22, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else if(dataPin==4) FastLED.addLeds<LED_TYPE,21, COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);
  else FastLED.addLeds<LED_TYPE,19,COLOR_ORDER>(leds, stripPixels).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(brightness);
  setStripColor(0,0,0);
  FastLED.show();
  setStripColor(r,g,b);
  FastLED.show();

}

//******************************************************************************************************************
// Configurar um display de 7 segmentos
// displayIndex: índice do display (0-5)
// digit: dígito a ser exibido (0-9)
// startLed: LED inicial do display (cada display usa 7 LEDs consecutivos)
// Usa a cor atual definida por setStripColor
//******************************************************************************************************************

void setSevenSegmentDisplay(uint8_t displayIndex, uint8_t digit, uint16_t startLed) {
  if (displayIndex > 5 || digit > 9) return; // Máximo 6 displays, dígitos 0-9
  
  uint8_t pattern = digitPatterns[digit];
  
  // Mapear segmentos para LEDs na ordem correta:
  // LED0: Superior Direito, LED1: Topo, LED2: Superior Esquerdo, LED3: Meio, 
  // LED4: Inferior Direito, LED5: Base, LED6: Inferior Esquerdo
  
  for (uint8_t seg = 0; seg < 7; seg++) {
    uint16_t ledIndex = startLed + seg;
    if (ledIndex < stripPixels) {
      if (pattern & (1 << seg)) {
        leds[ledIndex] = currentStripColor;
      } else {
        leds[ledIndex] = CRGB::Black;
      }
    }
  }
}

//******************************************************************************************************************
// Exibir um número em múltiplos displays de 7 segmentos
// number: número a ser exibido
// numDisplays: quantidade de displays a usar (1-6)
// startLed: LED inicial do primeiro display
// Usa a cor atual definida por setStripColor
//******************************************************************************************************************

void showNumberOnDisplays(uint32_t number, uint8_t numDisplays, uint16_t startLed) {
  if (numDisplays > 6) numDisplays = 6;
  
  // Converter número em dígitos individuais
  uint8_t digits[6] = {0, 0, 0, 0, 0, 0};
  uint32_t temp = number;
  
  // Extrair dígitos (da direita para esquerda)
  for (int i = 0; i < numDisplays; i++) {
    digits[i] = temp % 10;
    temp /= 10;
  }
  
  // Exibir cada dígito em seu display correspondente
  for (uint8_t i = 0; i < numDisplays; i++) {
    uint16_t displayStartLed = startLed + (i * 7);
    setSevenSegmentDisplay(i, digits[numDisplays - 1 - i], displayStartLed);
  }
  
  FastLED.show();
}

//******************************************************************************************************************
// Efeito de contagem em displays de 7 segmentos
// startLed: LED inicial dos displays
// numDisplays: quantidade de displays (1-6)
// maxCount: valor máximo da contagem
// Usa a cor atual definida por setStripColor
//******************************************************************************************************************

void sevenSegmentCounter(uint16_t startLed, uint8_t numDisplays, uint32_t maxCount) {
  static uint32_t counter = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate > 500) { // Atualiza a cada 500ms
    showNumberOnDisplays(counter, numDisplays, startLed);
    
    counter++;
    if (counter > maxCount) {
      counter = 0;
    }
    
    lastUpdate = millis();
  }
}

//******************************************************************************************************************
// Exibir hora no formato HH:MM (usa 4 displays)
// hours: horas (0-23)
// minutes: minutos (0-59)
// startLed: LED inicial dos displays
// showColon: mostrar dois pontos entre horas e minutos
// Usa a cor atual definida por setStripColor
//******************************************************************************************************************

void showTimeOnDisplays(uint8_t hours, uint8_t minutes, uint16_t startLed, bool showColon) {
  // Display das horas (2 displays)
  setSevenSegmentDisplay(0, hours / 10, startLed);
  setSevenSegmentDisplay(1, hours % 10, startLed + 7);
  
  // Dois pontos (opcional - usando 2 LEDs extras)
  if (showColon && startLed + 14 + 1 < stripPixels) {
    leds[startLed + 14] = currentStripColor;
    leds[startLed + 15] = currentStripColor;
  }
  
  // Display dos minutos (2 displays)
  uint16_t minutesStart = startLed + (showColon ? 16 : 14);
  setSevenSegmentDisplay(2, minutes / 10, minutesStart);
  setSevenSegmentDisplay(3, minutes % 10, minutesStart + 7);
  
  FastLED.show();

}

//******************************************************************************************************************
// Contador de 0 a 9 infinito com intervalo de 1 segundo
// startLed: LED inicial do display
// Usa a cor atual definida por setStripColor
//******************************************************************************************************************

void sevenSegmentCounterSimple(uint16_t startLed) {

  static uint8_t counter = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= speed*100) { // Atualiza a cada 100ms*speed (100 ms)
    setSevenSegmentDisplay(0, counter, startLed);
    FastLED.show();
    
    counter++;
    if (counter > 9) {
      counter = 0; // Reinicia a contagem
    }
    
    lastUpdate = millis();
  }

}

//******************************************************************************************************************
// Teste simples para verificar um dígito específico - use para debug
// Exemplo: testSevenSegmentDigit(0, 6); // testa dígito 6
// Usa a cor atual definida por setStripColor
//******************************************************************************************************************

void testSevenSegmentDigit(uint16_t startLed, uint8_t digit) {

  if (digit > 9) return;
  
  // Limpa todos os LEDs do display primeiro
  for (uint8_t i = 0; i < 7; i++) {
    if (startLed + i < stripPixels) {
      leds[startLed + i] = CRGB::Black;
    }
  }
  
  uint8_t pattern = digitPatterns[digit];
  
  // Acende cada LED baseado no padrão
  for (uint8_t seg = 0; seg < 7; seg++) {

    uint16_t ledIndex = startLed + seg;
    
    if (ledIndex < stripPixels) {
    
        if (pattern & (1 << seg)) {
    
            leds[ledIndex] = currentStripColor;
    
        }
    
    }
  
}
  
  FastLED.show();

}

//******************************************************************************************************************
// Contador de 0 a 99 usando dois displays de 7 segmentos
// startLed: LED inicial do primeiro display (segundo display usa startLed + 7)
// Primeiro display: unidades, Segundo display: dezenas
// Usa a cor atual definida por setStripColor
// Atualiza baseado na variável speed (speed * 100ms de intervalo)
//******************************************************************************************************************

void twoDisplayCounter0to99(uint16_t startLed) {

  static uint8_t counter = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= speed * 100) { // Atualiza a cada 100ms * speed
    
    // Separar o número em dezenas e unidades
    uint8_t tens = counter / 10;    // Dezenas (segundo display)
    uint8_t units = counter % 10;   // Unidades (primeiro display)
    
    // Configurar primeiro display (unidades)
    setSevenSegmentDisplay(0, units, startLed);
    
    // Configurar segundo display (dezenas) - 7 LEDs após o primeiro
    setSevenSegmentDisplay(1, tens, startLed + 7);
    
    FastLED.show();
    
    counter++;
    if (counter > 99) {
      counter = 0; // Reinicia a contagem de 0 a 99
    }
    
    lastUpdate = millis();
  }

}

//******************************************************************************************************************
// Contador de 0 a 999 usando três displays de 7 segmentos
// startLed: LED inicial do primeiro display (segundo display usa startLed + 7, terceiro startLed + 14)
// Primeiro display: unidades, Segundo display: dezenas, Terceiro display: centenas
// Usa a cor atual definida por setStripColor
// Atualiza baseado na variável speed (speed * 100ms de intervalo)
//******************************************************************************************************************

void threeDisplayCounter0to999(uint16_t startLed) {

  static uint16_t counter = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= speed * 100) { // Atualiza a cada 100ms * speed
    
    // Separar o número em centenas, dezenas e unidades
    uint8_t hundreds = counter / 100;      // Centenas (terceiro display)
    uint8_t tens = (counter % 100) / 10;   // Dezenas (segundo display)
    uint8_t units = counter % 10;          // Unidades (primeiro display)
    
    // Configurar primeiro display (unidades)
    setSevenSegmentDisplay(0, units, startLed);
    
    // Configurar segundo display (dezenas) - 7 LEDs após o primeiro
    setSevenSegmentDisplay(1, tens, startLed + 7);
    
    // Configurar terceiro display (centenas) - 14 LEDs após o primeiro
    setSevenSegmentDisplay(2, hundreds, startLed + 14);
    
    FastLED.show();
    
    counter++;
    if (counter > 999) {
      counter = 0; // Reinicia a contagem de 0 a 999
    }
    
    lastUpdate = millis();
  }

}

//******************************************************************************************************************
// Contador de 0 a 9999 usando quatro displays de 7 segmentos
// startLed: LED inicial do primeiro display (segundo display usa startLed + 7, terceiro startLed + 14, quarto startLed + 21)
// Primeiro display: unidades, Segundo display: dezenas, Terceiro display: centenas, Quarto display: milhares
// Usa a cor atual definida por setStripColor
// Atualiza baseado na variável speed (speed * 100ms de intervalo)
//******************************************************************************************************************

void fourDisplayCounter0to9999(uint16_t startLed) {

  static uint16_t counter = 0;
  static unsigned long lastUpdate = 0;
  static bool stripChecked = false;

  // Garante tamanho mínimo de strip para 4 displays (4 x 7 = 28 LEDs)
  if (!stripChecked) {
    long required = (long)startLed + 28;
    if (stripPixels < required) {
      set_strip_pixels(required);
    }
    stripChecked = true;
  }
  
  if (millis() - lastUpdate >= speed * 10) { // Atualiza a cada 100ms * speed
    
    // Separar o número em milhares, centenas, dezenas e unidades
    uint8_t thousands = counter / 1000;        // Milhares (quarto display)
    uint8_t hundreds = (counter % 1000) / 100; // Centenas (terceiro display)
    uint8_t tens = (counter % 100) / 10;       // Dezenas (segundo display)
    uint8_t units = counter % 10;              // Unidades (primeiro display)
    
    // Configurar primeiro display (unidades)
    setSevenSegmentDisplay(0, units, startLed);
    
    // Configurar segundo display (dezenas) - 7 LEDs após o primeiro
    setSevenSegmentDisplay(1, tens, startLed + 7);
    
    // Configurar terceiro display (centenas) - 14 LEDs após o primeiro
    setSevenSegmentDisplay(2, hundreds, startLed + 14);
    
    // Configurar quarto display (milhares) - 21 LEDs após o primeiro
    setSevenSegmentDisplay(3, thousands, startLed + 21);
    
    FastLED.show();
    
    counter++;
    if (counter > 9999) {
      counter = 0; // Reinicia a contagem de 0 a 9999
    }
    
    lastUpdate = millis();
  }

}

//******************************************************************************************************************
// Define modo de exibição: 0 = 24h, 1 = 12h
//******************************************************************************************************************

void setClockDisplayMode(uint8_t mode) {

  clock12hMode = (mode == 1);

}

//******************************************************************************************************************
// Define ciclo de data: 0 = desliga, 1 = liga
// Ciclo a cada 60s: 2s dd/mm → 2s aaaa → 2s temperatura (XX.X C) → 2s humidade (99  H)
//******************************************************************************************************************

void setClockDateCycle(uint8_t mode) {

  dateCycleMode = (mode == 1);
  // reset estado do ciclo
  dateCycleActive = false;
  dateCyclePhase = 0;
  dateCyclePhaseStartMs = millis();
  dateCycleLastStartMs = millis();

}

//******************************************************************************************************************
// Relógio: mostra HH:MM usando RTC interno, com dois pontos piscando a cada 500ms
// Requer 30 LEDs a partir de startLed (4 dígitos = 28 + 2 dos dois pontos)
//******************************************************************************************************************

void displayTimeFromRTC(uint16_t startLed) {

  static ESP32Time rtc;                // RTC interno do ESP32
  static unsigned long lastToggle = 0; // controle de pisca
  static bool colonOn = false;
  static bool stripChecked = false;
  static unsigned long lastAnimUpdate = 0;
  static uint8_t animIdx = 0;          // índice na ordem de segmentos
  static const uint8_t animOrder[6] = {0, 1, 2, 6, 5, 4}; // ordem solicitada; pula o segmento do meio (3)

  // Garante tamanho mínimo de strip para HH:MM com dois pontos e LEDs extras (36 LEDs)
  // Layout: [D0:7][D1:7][extra][col1][col2][extra][D2:7][D3:7][4xreserv]
  if (!stripChecked) {
    long required = (long)startLed + 36;
    if (stripPixels < required) {
      set_strip_pixels(required);
    }
    stripChecked = true;
  }

  // Alterna os dois pontos a cada 500ms
  if (millis() - lastToggle >= 500) {
    colonOn = !colonOn;
    lastToggle = millis();
  }

  // Aplica brilho automático, se habilitado
  updateAutoBrightness(false);

  // Enquanto Wi-Fi/NTP não terminaram ou durante update, anima o primeiro dígito.
  // Durante update, não apaga centenas/milhares (letras estáticas são renderizadas em check_update).
  if (update_status) {
    // Em modo de update, não tocar nos LEDs aqui.
    // A função check_update() controla completamente a animação e letras.
    return;
  } else if (WiFi.status() != WL_CONNECTED || !rtc_synced) {
    // Durante conexão Wi-Fi/NTP: anima o primeiro dígito em vermelho,
    // mantém o dígito das dezenas de minutos apagado e exibe letras
    // C (milhares) e F (centenas) estáticas.
    for (uint8_t seg = 0; seg < 7; seg++) {
      leds[startLed + 7 + seg] = CRGB::Black;    // minutos dezenas (apagado)
      // Não apagar centenas (startLed+16) e milhares (startLed+23) para manter letras
    }
    // LEDs extras e dois pontos apagados
    leds[startLed + 14] = CRGB::Black;
    leds[startLed + 15] = CRGB::Black;
    leds[startLed + 16] = CRGB::Black;
    leds[startLed + 17] = CRGB::Black;
    leds[startLed + 32] = CRGB::Black;
    leds[startLed + 33] = CRGB::Black;
    leds[startLed + 34] = CRGB::Black;
    leds[startLed + 35] = CRGB::Black;

    // Renderiza letras estáticas: F no dígito das centenas, C no de milhares
    for (uint8_t s = 0; s < 7; s++) {
      // 'F' = Topo(1), SupEsq(2), Meio(3), InfEsq(6)
      bool onF = (s == 1) || (s == 2) || (s == 3) || (s == 6);
      leds[startLed + 18 + s] = onF ? CRGB::Red : CRGB::Black;

      // 'C' = Topo(1), SupEsq(2), Base(5), InfEsq(6)
      bool onC = (s == 1) || (s == 2) || (s == 5) || (s == 6);
      leds[startLed + 25 + s] = onC ? CRGB::Red : CRGB::Black;
    }

    // Atualiza animação do primeiro dígito
    if (millis() - lastAnimUpdate >= (unsigned long)(speed * 50)) {
      for (uint8_t s = 0; s < 7; s++) {
        leds[startLed + s] = CRGB::Black;
      }
      leds[startLed + animOrder[animIdx]] = CRGB::Red;
      animIdx = (animIdx + 1) % 6;
      lastAnimUpdate = millis();
      FastLED.show();
    }
    return;
  }

  // Lê hora/minuto do RTC
  struct tm t = rtc.getTimeStruct();
  uint8_t hours = (uint8_t)t.tm_hour;
  uint8_t minutes = (uint8_t)t.tm_min;
  bool isPM = hours >= 12;
  uint8_t dispHours = hours;
  if (clock12hMode) {
    dispHours = hours % 12;
    if (dispHours == 0) dispHours = 12;
  }

  // Ciclo de data (habilitado, sincronizado e sem update)
  if (dateCycleMode && rtc_synced && !update_status) {
    unsigned long now = millis();
    if (!dateCycleActive && (now - dateCycleLastStartMs) >= 60000UL) {
      dateCycleActive = true;
      dateCyclePhase = 1; // dd/mm
      dateCyclePhaseStartMs = now;
    }

    if (dateCycleActive) {
      // LEDs extras e dois pontos apagados durante o ciclo
      leds[startLed + 14] = CRGB::Black;
      leds[startLed + 15] = CRGB::Black;
      leds[startLed + 16] = CRGB::Black;
      leds[startLed + 17] = CRGB::Black;
      leds[startLed + 32] = CRGB::Black;
      leds[startLed + 33] = CRGB::Black;

      if (dateCyclePhase == 1) {
        // LED+34 (penúltimo) aceso indica exibição de data; LED+35 apagado
        leds[startLed + 34] = currentStripColor;
        leds[startLed + 35] = CRGB::Black;

        // Exibir dd/mm nos 4 dígitos
        // Dia (DD) nos displays de milhares e centenas (esquerda): D3 (startLed+23), D2 (startLed+16)
        // Mês (MM) nos displays de dezenas e unidades (direita): D1 (startLed+7), D0 (startLed)
        uint8_t day = (uint8_t)t.tm_mday;         // 1-31
        uint8_t month = (uint8_t)(t.tm_mon + 1);  // 1-12

        // Dia
        setSevenSegmentDisplay(3, day / 10, startLed + 25);
        setSevenSegmentDisplay(2, day % 10, startLed + 18);
        // Mês
        setSevenSegmentDisplay(1, month / 10, startLed + 7);
        setSevenSegmentDisplay(0, month % 10, startLed);
        FastLED.show();

        if ((now - dateCyclePhaseStartMs) >= 2000UL) {
          dateCyclePhase = 2; // aaaa
          dateCyclePhaseStartMs = now;
        }
        return; // não renderiza hora enquanto ciclo ativo
      }
      else if (dateCyclePhase == 2) {
        // LED+34 (penúltimo) aceso indica exibição do ano; LED+35 apagado
        leds[startLed + 34] = currentStripColor;
        leds[startLed + 35] = CRGB::Black;

        // Exibir ano aaaa nos 4 displays: D3 (milhar), D2 (centena), D1 (dezena), D0 (unidade)
        uint16_t year = (uint16_t)(t.tm_year + 1900);
        uint8_t thousands = year / 1000;
        uint8_t hundreds  = (year % 1000) / 100;
        uint8_t tens      = (year % 100) / 10;
        uint8_t ones      = year % 10;

        setSevenSegmentDisplay(3, thousands, startLed + 25);
        setSevenSegmentDisplay(2, hundreds,  startLed + 18);
        setSevenSegmentDisplay(1, tens,      startLed + 7);
        setSevenSegmentDisplay(0, ones,      startLed);
        FastLED.show();

        if ((now - dateCyclePhaseStartMs) >= 2000UL) {
          if (sensores.mode == 1 || sensores.mode == 3) {
            dateCyclePhase = 3; // temperatura
          } else if (sensores.mode == 2) {
            dateCyclePhase = 4; // humidade (pula temperatura)
          } else {
            // mode==0: não mostra nem temperatura nem humidade
            dateCycleActive = false;
            dateCyclePhase = 0;
            dateCycleLastStartMs = now;
          }
          dateCyclePhaseStartMs = now;
        }
        return; // não renderiza hora enquanto ciclo ativo
      }
      else if (dateCyclePhase == 3) {
        // Fase 3: Temperatura no formato XX.X C (Celsius) ou XX.X F (Fahrenheit)
        // +14: separador decimal (LED anterior aos dois pontos)
        // +33 (antipenúltimo): aceso durante exibição de temp/hum
        float tempVal = get_sensor_AHT10(0); // sempre retorna °C
        if (sensores.temp_unit == 1) {
          tempVal = tempVal * 9.0f / 5.0f + 32.0f; // converte para °F
        }
        if (tempVal < 0.0f)   tempVal = 0.0f;
        if (tempVal > 99.9f)  tempVal = 99.9f;
        uint8_t tempInt = (uint8_t)tempVal;
        uint8_t tempDec = (uint8_t)((tempVal - (float)tempInt) * 10.0f + 0.5f);
        if (tempDec > 9) tempDec = 9;

        leds[startLed + 14] = currentStripColor; // separador decimal
        leds[startLed + 15] = CRGB::Black;
        leds[startLed + 16] = CRGB::Black;
        leds[startLed + 17] = CRGB::Black;
        leds[startLed + 32] = CRGB::Black;
        leds[startLed + 33] = currentStripColor; // antipenúltimo: indicador temp/hum
        leds[startLed + 34] = CRGB::Black;
        leds[startLed + 35] = CRGB::Black;

        // D3 (startLed+25): dezenas da parte inteira
        setSevenSegmentDisplay(3, tempInt / 10, startLed + 25);
        // D2 (startLed+18): unidades da parte inteira
        setSevenSegmentDisplay(2, tempInt % 10, startLed + 18);
        // D1 (startLed+7): dígito decimal
        setSevenSegmentDisplay(1, tempDec, startLed + 7);
        // D0 (startLed): letra 'C' ou 'F' conforme sensores.temp_unit
        for (uint8_t s = 0; s < 7; s++) {
          bool onLetter;
          if (sensores.temp_unit == 1) {
            // 'F' = Topo(1), SupEsq(2), Meio(3), InfEsq(6)
            onLetter = (s == 1) || (s == 2) || (s == 3) || (s == 6);
          } else {
            // 'C' = Topo(1), SupEsq(2), Base(5), InfEsq(6)
            onLetter = (s == 1) || (s == 2) || (s == 5) || (s == 6);
          }
          leds[startLed + s] = onLetter ? currentStripColor : CRGB::Black;
        }
        FastLED.show();

        if ((now - dateCyclePhaseStartMs) >= 2000UL) {
          if (sensores.mode == 3) {
            dateCyclePhase = 4; // humidade
          } else {
            // mode==1: só temperatura — fim do ciclo
            dateCycleActive = false;
            dateCyclePhase = 0;
            dateCycleLastStartMs = now;
          }
          dateCyclePhaseStartMs = now;
        }
        return; // não renderiza hora enquanto ciclo ativo
      }
      else if (dateCyclePhase == 4) {
        // Fase 4: Humidade no formato 99  H
        // +33 (antipenúltimo): aceso durante exibição de temp/hum
        float humVal = get_sensor_AHT10(1);
        if (humVal < 0.0f)  humVal = 0.0f;
        if (humVal > 99.0f) humVal = 99.0f;
        uint8_t hum = (uint8_t)(humVal + 0.5f);
        if (hum > 99) hum = 99;

        leds[startLed + 14] = CRGB::Black;
        leds[startLed + 15] = CRGB::Black;
        leds[startLed + 16] = CRGB::Black;
        leds[startLed + 17] = CRGB::Black;
        leds[startLed + 32] = CRGB::Black;
        leds[startLed + 33] = currentStripColor; // antipenúltimo: indicador temp/hum
        leds[startLed + 34] = CRGB::Black;
        leds[startLed + 35] = CRGB::Black;

        // D3 (startLed+25): dezenas da humidade
        setSevenSegmentDisplay(3, hum / 10, startLed + 25);
        // D2 (startLed+18): unidades da humidade
        setSevenSegmentDisplay(2, hum % 10, startLed + 18);
        // D1 (startLed+7): apagado (pula um dígito)
        for (uint8_t s = 0; s < 7; s++) leds[startLed + 7 + s] = CRGB::Black;
        // D0 (startLed): letra 'H' — SupDir(0), SupEsq(2), Meio(3), InfDir(4), InfEsq(6)
        for (uint8_t s = 0; s < 7; s++) {
          bool onH = (s == 0) || (s == 2) || (s == 3) || (s == 4) || (s == 6);
          leds[startLed + s] = onH ? currentStripColor : CRGB::Black;
        }
        FastLED.show();

        if ((now - dateCyclePhaseStartMs) >= 2000UL) {
          // fim do ciclo, volta para hora
          dateCycleActive = false;
          dateCyclePhase = 0;
          dateCycleLastStartMs = now;
        }
        return; // não renderiza hora enquanto ciclo ativo
      }
    }
  }

  // Minutos
  setSevenSegmentDisplay(0, minutes % 10, startLed);       // unidades dos minutos
  setSevenSegmentDisplay(1, minutes / 10, startLed + 7);   // dezenas dos minutos

  // LEDs extras sempre desligados (+14, +17, +32..+33)
  leds[startLed + 14] = CRGB::Black;
  leds[startLed + 17] = CRGB::Black;
  leds[startLed + 32] = CRGB::Black;
  leds[startLed + 33] = CRGB::Black;
  // +34 (penúltimo): apagado durante exibição de hora
  leds[startLed + 34] = CRGB::Black;
  // +35 (último): aceso quando modo 12h E hora atual é PM
  leds[startLed + 35] = (clock12hMode && isPM) ? currentStripColor : CRGB::Black;

  // Dois pontos conforme modo (agora em +15 e +16)
  if (clock12hMode) {
    if (!isPM) {
      leds[startLed + 15] = currentStripColor;
      leds[startLed + 16] = colonOn ? currentStripColor : CRGB::Black;
    } else {
      leds[startLed + 15] = colonOn ? currentStripColor : CRGB::Black;
      leds[startLed + 16] = colonOn ? currentStripColor : CRGB::Black;
    }
  } else {
    leds[startLed + 15] = colonOn ? currentStripColor : CRGB::Black;
    leds[startLed + 16] = colonOn ? currentStripColor : CRGB::Black;
  }

  // Horas (após os quatro LEDs dos pontos + extras)
  setSevenSegmentDisplay(2, dispHours % 10, startLed + 18);    // unidades das horas
  setSevenSegmentDisplay(3, dispHours / 10, startLed + 25);    // dezenas das horas

  FastLED.show();

}

//******************************************************************************************************************
// Contador de 0 a 99999 usando cinco displays de 7 segmentos
// startLed: LED inicial do primeiro display (displays subsequentes em intervalos de 7 LEDs)
// Primeiro display: unidades, Segundo: dezenas, Terceiro: centenas, Quarto: milhares, Quinto: dezenas de milhares
// Usa a cor atual definida por setStripColor
// Atualiza baseado na variável speed (speed * 100ms de intervalo)
//******************************************************************************************************************

void fiveDisplayCounter0to99999(uint16_t startLed) {

  static uint32_t counter = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= speed * 100) { // Atualiza a cada 100ms * speed
    
    // Separar o número em dezenas de milhares, milhares, centenas, dezenas e unidades
    uint8_t tenThousands = counter / 10000;      // Dezenas de milhares (quinto display)
    uint8_t thousands = (counter % 10000) / 1000; // Milhares (quarto display)
    uint8_t hundreds = (counter % 1000) / 100;   // Centenas (terceiro display)
    uint8_t tens = (counter % 100) / 10;         // Dezenas (segundo display)
    uint8_t units = counter % 10;                // Unidades (primeiro display)
    
    // Configurar displays em ordem
    setSevenSegmentDisplay(0, units, startLed);           // Unidades
    setSevenSegmentDisplay(1, tens, startLed + 7);        // Dezenas
    setSevenSegmentDisplay(2, hundreds, startLed + 14);   // Centenas
    setSevenSegmentDisplay(3, thousands, startLed + 21);  // Milhares
    setSevenSegmentDisplay(4, tenThousands, startLed + 28); // Dezenas de milhares
    
    FastLED.show();
    
    counter++;
    if (counter > 99999) {
      counter = 0; // Reinicia a contagem de 0 a 99999
    }
    
    lastUpdate = millis();
  }

}

//******************************************************************************************************************
// Contador de 0 a 999999 usando seis displays de 7 segmentos
// startLed: LED inicial do primeiro display (displays subsequentes em intervalos de 7 LEDs)
// Primeiro display: unidades, Segundo: dezenas, DOIS LEDs SEMPRE LIGADOS, Terceiro: centenas, Quarto: milhares, DOIS LEDs SEMPRE LIGADOS, Quinto: dezenas de milhares, Sexto: centenas de milhares
// Usa a cor atual definida por setStripColor
// Atualiza baseado na variável speed (speed * 100ms de intervalo)
// Total de LEDs: 46 (6 displays × 7 LEDs + 4 LEDs sempre ligados)
//******************************************************************************************************************

void sixDisplayCounter0to999999(uint16_t startLed) {

  static uint32_t counter = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= speed * 1) { // Atualiza a cada 100ms * speed
    
    // Separar o número em centenas de milhares, dezenas de milhares, milhares, centenas, dezenas e unidades
    uint8_t hundredThousands = counter / 100000;     // Centenas de milhares (sexto display)
    uint8_t tenThousands = (counter % 100000) / 10000; // Dezenas de milhares (quinto display)
    uint8_t thousands = (counter % 10000) / 1000;    // Milhares (quarto display)
    uint8_t hundreds = (counter % 1000) / 100;       // Centenas (terceiro display)
    uint8_t tens = (counter % 100) / 10;             // Dezenas (segundo display)
    uint8_t units = counter % 10;                    // Unidades (primeiro display)
    
    // Configurar displays em ordem
    setSevenSegmentDisplay(0, units, startLed);               // Unidades
    setSevenSegmentDisplay(1, tens, startLed + 7);            // Dezenas
    
    // LEDs sempre ligados entre segundo e terceiro displays
    leds[startLed + 14] = currentStripColor;  // Primeiro LED sempre ligado
    leds[startLed + 15] = currentStripColor;  // Segundo LED sempre ligado
    
    setSevenSegmentDisplay(2, hundreds, startLed + 16);       // Centenas (após os 2 LEDs)
    setSevenSegmentDisplay(3, thousands, startLed + 23);      // Milhares
    
    // LEDs sempre ligados entre quarto e quinto displays
    leds[startLed + 30] = currentStripColor;  // Terceiro LED sempre ligado
    leds[startLed + 31] = currentStripColor;  // Quarto LED sempre ligado
    
    setSevenSegmentDisplay(4, tenThousands, startLed + 32);   // Dezenas de milhares (após os 2 LEDs)
    setSevenSegmentDisplay(5, hundredThousands, startLed + 39); // Centenas de milhares
    
    FastLED.show();
    
    counter++;
    if (counter > 999999) {
      counter = 0; // Reinicia a contagem de 0 a 999999
    }
    
    lastUpdate = millis();
  }

}

//******************************************************************************************************************
// Controle do efeito ativo no strip2
// 0 = desligado, 1 = rainbow, 2 = chase
//******************************************************************************************************************

static uint8_t strip2EffectActive = 1;

// Cor, brilho e velocidade do strip2 (referenciados como extern em _utils/_parse)
extern int strip2_color_r;
extern int strip2_color_g;
extern int strip2_color_b;
extern int strip2Brightness;
extern int strip2Speed;

// strip2Color  : cor "raw" (salva para recalcular strip2ColorDisplay quando brilho muda)
// strip2ColorDisplay : cor pré-multiplicada pelo brilho — usada diretamente pelos efeitos
// Efeitos baseados em paleta/CHSV aplicam nscale8 DENTRO do seu bloco millis(),
// evitando o problema de acumulação que ocorria ao chamar nscale8 a cada frame.
static CRGB strip2Color = CRGB::White;
static CRGB strip2ColorDisplay = CRGB::White;

void setStrip2Color(uint8_t r, uint8_t g, uint8_t b) {
  strip2Color = CRGB(r, g, b);
  strip2ColorDisplay = strip2Color;
  strip2ColorDisplay.nscale8((uint8_t)strip2Brightness);
}

void setStrip2Brightness(uint8_t brightness) {
  strip2Brightness = brightness;
  strip2ColorDisplay = strip2Color;
  strip2ColorDisplay.nscale8(brightness);
}

void setStrip2Speed(uint8_t spd) {
  strip2Speed = spd;
}

void setStrip2Effect(uint8_t effect) {
  strip2EffectActive = effect;
  if (effect == 0) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
  }
}

void clearStrip2() {
  fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
}

//******************************************************************************************************************
// Efeito perseguição: acende um LED por vez e avança para o próximo
//******************************************************************************************************************

void ledChase36() {
  static uint8_t currentLed = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    leds2[currentLed] = strip2ColorDisplay;
    currentLed++;
    if (currentLed >= STRIP2_LEDS) currentLed = 0;
    lastUpdate = millis();
  }
}

//******************************************************************************************************************
// Efeito arco-íris no strip2 (IO 22, 36 LEDs)
//******************************************************************************************************************

void rainbowStrip2() {
  static uint8_t hue = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 4)) {
    fill_rainbow(leds2, STRIP2_LEDS, hue, 256 / STRIP2_LEDS);
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    hue++;
    lastUpdate = millis();
  }
}

// 3: Cometa — pixel brilhante com rastro que vai apagando
void cometStrip2() {
  static int pos = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(180);
    leds2[pos] = strip2ColorDisplay;
    pos = (pos + 1) % STRIP2_LEDS;
    lastUpdate = millis();
  }
}

// 4: Respirar — fade in/out suave (range: preto → strip2ColorDisplay)
void breatheStrip2() {
  static unsigned long lastUpdate = 0;
  static uint8_t breatheVal = 0;
  static bool breatheUp = true;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 4)) {
    if (breatheUp) { breatheVal = qadd8(breatheVal, 5); if (breatheVal >= 250) breatheUp = false; }
    else           { breatheVal = qsub8(breatheVal, 5); if (breatheVal <=   5) breatheUp = true;  }
    CRGB c = strip2ColorDisplay;
    c.nscale8(breatheVal);
    fill_solid(leds2, STRIP2_LEDS, c);
    lastUpdate = millis();
  }
}

// 5: Estrobo — flash rápido ligado/desligado
void strobeStrip2() {
  static unsigned long lastUpdate = 0;
  static bool on = false;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    fill_solid(leds2, STRIP2_LEDS, on ? strip2ColorDisplay : CRGB::Black);
    on = !on;
    lastUpdate = millis();
  }
}

// 6: Color Wipe — preenche LED a LED depois apaga LED a LED
void colorWipeStrip2() {
  static int pos = 0;
  static bool filling = true;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    if (filling) {
      leds2[pos] = strip2ColorDisplay;
      pos++;
      if (pos >= STRIP2_LEDS) { pos = STRIP2_LEDS - 1; filling = false; }
    } else {
      leds2[pos] = CRGB::Black;
      pos--;
      if (pos < 0) { pos = 0; filling = true; }
    }
    lastUpdate = millis();
  }
}

// 7: Twinkle — pixels aleatórios acendem e apagam gradualmente
void twinkleStrip2() {
  static unsigned long lastFade = 0;
  static unsigned long lastSparkle = 0;
  if (millis() - lastFade >= 30) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(210);
    lastFade = millis();
  }
  if (millis() - lastSparkle >= (unsigned long)(strip2Speed * 20)) {
    leds2[random8(STRIP2_LEDS)] = strip2ColorDisplay;
    lastSparkle = millis();
  }
}

// 8: Fogo — simulação de chamas (usa paleta HeatColor)
void fireStrip2() {
  static uint8_t heat[STRIP2_LEDS];
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) heat[i] = qsub8(heat[i], random8(0, 30));
    for (int i = STRIP2_LEDS - 1; i >= 2; i--) heat[i] = ((uint16_t)heat[i-1] + heat[i-2] + heat[i-2]) / 3;
    if (random8() < 120) {
      uint8_t spark = random8(4);
      heat[spark] = qadd8(heat[spark], random8(160, 255));
    }
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i] = HeatColor(heat[i]);
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    lastUpdate = millis();
  }
}

// 9: Scanner — varredura estilo Larson (vai e volta)
void scannerStrip2() {
  static int pos = 0;
  static int dir = 1;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    leds2[pos] = strip2ColorDisplay;
    if (pos > 0)             { leds2[pos-1] = strip2ColorDisplay; leds2[pos-1].nscale8(80); }
    if (pos < STRIP2_LEDS-1) { leds2[pos+1] = strip2ColorDisplay; leds2[pos+1].nscale8(80); }
    pos += dir;
    if (pos >= STRIP2_LEDS - 1) { pos = STRIP2_LEDS - 1; dir = -1; }
    if (pos <= 0)                { pos = 0;                dir =  1; }
    lastUpdate = millis();
  }
}

// 10: Fill Fade — preenche tudo e depois apaga gradualmente
void fillFadeStrip2() {
  static unsigned long lastUpdate = 0;
  static uint8_t phase = 0;
  static int pos = 0;
  static uint8_t fadeVal = 255;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    if (phase == 0) {
      leds2[pos] = strip2ColorDisplay;
      pos++;
      if (pos >= STRIP2_LEDS) { phase = 1; fadeVal = 255; }
    } else {
      fadeVal = qsub8(fadeVal, 8);
      for (int i = 0; i < STRIP2_LEDS; i++) { leds2[i] = strip2ColorDisplay; leds2[i].nscale8(fadeVal); }
      if (fadeVal == 0) { phase = 0; pos = 0; fill_solid(leds2, STRIP2_LEDS, CRGB::Black); }
    }
    lastUpdate = millis();
  }
}

// 11: Alternado — pixels pares e ímpares piscam em alternância
void alternatingStrip2() {
  static unsigned long lastUpdate = 0;
  static bool toggle = false;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 50)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      leds2[i] = ((i % 2) == (toggle ? 1 : 0)) ? strip2ColorDisplay : CRGB::Black;
    }
    toggle = !toggle;
    lastUpdate = millis();
  }
}

// 12: Cor sólida — preenche todo o strip2
void solidColorStrip2() {
  fill_solid(leds2, STRIP2_LEDS, strip2ColorDisplay);
}

// 13: Rainbow Chase — pixel com cor do arco-íris avança em loop com rastro
void rainbowChaseStrip2() {
  static int pos = 0;
  static uint8_t hue = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(160);
    leds2[pos] = CHSV(hue, 255, 255);
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    pos = (pos + 1) % STRIP2_LEDS;
    hue += 7;
    lastUpdate = millis();
  }
}

// 14: Bounce — pixel vai e volta rebatendo nas pontas
void bounceStrip2() {
  static int pos = 0;
  static int dir = 1;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    leds2[pos] = strip2ColorDisplay;
    pos += dir;
    if (pos >= STRIP2_LEDS) { pos = STRIP2_LEDS - 2; dir = -1; }
    if (pos < 0)             { pos = 1;               dir =  1; }
    lastUpdate = millis();
  }
}

// 15: Meteor — cometa com rastro que desvanece aleatoriamente
void meteorStrip2() {
  static int pos = 0;
  static unsigned long lastUpdate = 0;
  const uint8_t meteorSize = 4;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      if (random8(10) > 5) leds2[i].nscale8(192);
    }
    for (int i = 0; i < meteorSize; i++) {
      int p = pos - i;
      if (p >= 0 && p < STRIP2_LEDS) {
        uint8_t fade = 255 - (i * 60);
        leds2[p] = strip2ColorDisplay;
        leds2[p].nscale8(fade);
      }
    }
    pos++;
    if (pos >= STRIP2_LEDS + meteorSize) pos = 0;
    lastUpdate = millis();
  }
}

// 16: Sparkle — pixels brancos piscam sobre fundo strip2Color
void sparkleStrip2() {
  static unsigned long lastFill = 0;
  static unsigned long lastSpark = 0;
  if (millis() - lastFill >= (unsigned long)(strip2Speed * 80)) {
    fill_solid(leds2, STRIP2_LEDS, strip2ColorDisplay);
    lastFill = millis();
  }
  if (millis() - lastSpark >= (unsigned long)(strip2Speed * 5)) {
    leds2[random8(STRIP2_LEDS)] = CRGB::White;
    lastSpark = millis();
  }
}

// 17: Running Lights — onda senoidal percorre o strip
void runningLightsStrip2() {
  static uint8_t phase = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t bright = sin8((i * 256 / STRIP2_LEDS) + phase);
      leds2[i] = strip2ColorDisplay;
      leds2[i].nscale8(bright);
    }
    phase += 8;
    lastUpdate = millis();
  }
}

// 18: Color Cycle — todo o strip muda de cor continuamente (HSV)
void colorCycleStrip2() {
  static uint8_t hue = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    fill_solid(leds2, STRIP2_LEDS, CHSV(hue, 255, 255));
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    hue++;
    lastUpdate = millis();
  }
}

// 19: Pacifica — ondas de oceano (OceanColors_p)
void pacificaStrip2() {
  static uint16_t sCIStart1 = 0, sCIStart2 = 0, sCIStart3 = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    sCIStart1 += beatsin16(3, 179, 269);
    sCIStart2 -= beatsin16(4, 109, 149);
    sCIStart3 -= beatsin16(5,  79, 109);
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t index = (uint8_t)(((uint16_t)i * 256 / STRIP2_LEDS + sCIStart1) >> 8);
      leds2[i] = ColorFromPalette(OceanColors_p, index, 200, LINEARBLEND);
    }
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    lastUpdate = millis();
  }
}

// 20: Dual Scanner — dois scanners opostos que se cruzam no centro
void dualScannerStrip2() {
  static int pos = 0;
  static int dir = 1;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    int pos2 = STRIP2_LEDS - 1 - pos;
    leds2[pos]  = strip2ColorDisplay;
    leds2[pos2] = strip2ColorDisplay;
    if (pos > 0)             { leds2[pos-1]  = strip2ColorDisplay; leds2[pos-1].nscale8(60); }
    if (pos < STRIP2_LEDS-1) { leds2[pos+1]  = strip2ColorDisplay; leds2[pos+1].nscale8(60); }
    if (pos2 > 0)            { leds2[pos2-1] = strip2ColorDisplay; leds2[pos2-1].nscale8(60); }
    if (pos2 < STRIP2_LEDS-1){ leds2[pos2+1] = strip2ColorDisplay; leds2[pos2+1].nscale8(60); }
    pos += dir;
    if (pos >= STRIP2_LEDS / 2) { pos = STRIP2_LEDS / 2 - 1; dir = -1; }
    if (pos < 0)                 { pos = 0;                    dir =  1; }
    lastUpdate = millis();
  }
}

// 21: Lava — paleta lava com movimento lento
void lavaStrip2() {
  static uint16_t index = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      leds2[i] = ColorFromPalette(LavaColors_p, (uint8_t)((index + i * 8) & 0xFF), 255, LINEARBLEND);
    }
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    index += 2;
    lastUpdate = millis();
  }
}

// 22: Ice — paleta fria azul/branco
void iceStrip2() {
  static const CRGBPalette16 icePalette = CRGBPalette16(
    CRGB(0,   0,  20), CRGB(0,   0,  60), CRGB(0,  20, 100), CRGB(0,  60, 140),
    CRGB(0, 100, 180), CRGB(20, 140, 200), CRGB(60, 180, 220), CRGB(120, 210, 240),
    CRGB(180, 230, 255), CRGB(220, 245, 255), CRGB(255, 255, 255), CRGB(200, 240, 255),
    CRGB(140, 210, 240), CRGB(60, 160, 210), CRGB(10, 100, 170), CRGB(0, 40, 100)
  );
  static uint16_t index = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      leds2[i] = ColorFromPalette(icePalette, (uint8_t)((index + i * 8) & 0xFF), 255, LINEARBLEND);
    }
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    index += 2;
    lastUpdate = millis();
  }
}

// 23: Wipe-In-Out — preenche do centro para as pontas e vice-versa
void wipeInOutStrip2() {
  static int step = 0;
  static bool expanding = true;
  static unsigned long lastUpdate = 0;
  int half = STRIP2_LEDS / 2;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    if (expanding) {
      int left  = half - step - 1;
      int right = half + step;
      if (left  >= 0)           leds2[left]  = strip2ColorDisplay;
      if (right < STRIP2_LEDS)  leds2[right] = strip2ColorDisplay;
      step++;
      if (step >= half) { expanding = false; }
    } else {
      int left  = half - step - 1;
      int right = half + step;
      if (left  >= 0)           leds2[left]  = CRGB::Black;
      if (right < STRIP2_LEDS)  leds2[right] = CRGB::Black;
      step--;
      if (step < 0) { step = 0; expanding = true; }
    }
    lastUpdate = millis();
  }
}

// 24: Fade-In-Out — todo strip pulsa ciclicamente
void fadeInOutStrip2() {
  static uint8_t val = 0;
  static int8_t dir = 5;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 8)) {
    val += dir;
    if (val >= 250) dir = -5;
    if (val <=   5) dir =  5;
    CRGB c = strip2ColorDisplay;
    c.nscale8(val);
    fill_solid(leds2, STRIP2_LEDS, c);
    lastUpdate = millis();
  }
}

// 25: Theater Chase — blocos de 3 acesos / 3 apagados deslocam-se
void theaterChaseStrip2() {
  static uint8_t offset = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 40)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      leds2[i] = ((i + offset) % 3 == 0) ? strip2ColorDisplay : CRGB::Black;
    }
    offset = (offset + 1) % 3;
    lastUpdate = millis();
  }
}

// 26: Lightning — relâmpagos aleatórios sobre fundo escuro
void lightningStrip2() {
  static unsigned long lastFlash = 0;
  static unsigned long flashEnd  = 0;
  static bool flashing = false;
  if (!flashing && millis() - lastFlash >= (unsigned long)(strip2Speed * 200)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    int start = random8(STRIP2_LEDS - 6);
    int len   = random8(3, 7);
    for (int i = start; i < start + len && i < STRIP2_LEDS; i++) leds2[i] = CRGB::White;
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    flashing  = true;
    flashEnd  = millis() + random8(20, 80);
    lastFlash = millis();
  }
  if (flashing && millis() >= flashEnd) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    flashing = false;
  }
}

// 27: Confetti — pixels com matizes aleatórias acendem e apagam
void confettiStrip2() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(220);
    int pos = random8(STRIP2_LEDS);
    leds2[pos] = CHSV(random8(), 200, 255);
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    lastUpdate = millis();
  }
}

// 28: Gradient Shift — gradiente deslizante entre strip2Color e sua rotação RGB
void gradientShiftStrip2() {
  static uint8_t shift = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t blend_val = sin8((i * 255 / STRIP2_LEDS) + shift);
      leds2[i] = blend(strip2ColorDisplay, CRGB(strip2ColorDisplay.b, strip2ColorDisplay.r, strip2ColorDisplay.g), blend_val);
    }
    shift += 3;
    lastUpdate = millis();
  }
}

// 29: Plasma — ondas sobrepostas criam padrão de plasma colorido
void plasmaStrip2() {
  static uint8_t phase1 = 0, phase2 = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t val = sin8(i * 7 + phase1) / 2 + sin8(i * 13 + phase2) / 2;
      leds2[i] = CHSV(val, 255, 255);
    }
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    phase1 += 3;
    phase2 += 5;
    lastUpdate = millis();
  }
}

// 30: Ripple — ondas concêntricas do centro para as pontas
void rippleStrip2() {
  static uint8_t phase = 0;
  static unsigned long lastUpdate = 0;
  int center = STRIP2_LEDS / 2;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t dist = (uint8_t)abs(i - center);
      uint8_t val = sin8(dist * 20 - phase);
      leds2[i] = strip2ColorDisplay;
      leds2[i].nscale8(val);
    }
    phase += 8;
    lastUpdate = millis();
  }
}

// 31: Cylon — scanner largo estilo Battlestar Galactica
void cylonStrip2() {
  static int pos = 0;
  static int dir = 1;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    leds2[pos] = strip2ColorDisplay;
    if (pos > 0)             { leds2[pos-1] = strip2ColorDisplay; leds2[pos-1].nscale8(100); }
    if (pos > 1)             { leds2[pos-2] = strip2ColorDisplay; leds2[pos-2].nscale8(30);  }
    if (pos < STRIP2_LEDS-1) { leds2[pos+1] = strip2ColorDisplay; leds2[pos+1].nscale8(100); }
    if (pos < STRIP2_LEDS-2) { leds2[pos+2] = strip2ColorDisplay; leds2[pos+2].nscale8(30);  }
    pos += dir;
    if (pos >= STRIP2_LEDS - 1) { pos = STRIP2_LEDS - 1; dir = -1; }
    if (pos <= 0)                { pos = 0;                dir =  1; }
    lastUpdate = millis();
  }
}

// 32: Sinelon — posição determinada por beatsin8, rastro que apaga
void sinelonStrip2() {
  static uint8_t beat = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(200);
    uint8_t pos = beatsin8(13, 0, STRIP2_LEDS - 1, 0, beat);
    leds2[pos] = strip2ColorDisplay;
    beat++;
    lastUpdate = millis();
  }
}

// 33: Pixel Stack — pixels acumulam da ponta esquerda para a direita
void pixelStackStrip2() {
  static int stackPos = STRIP2_LEDS - 1;
  static int cur = 0;
  static bool resetting = false;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    if (resetting) {
      fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
      stackPos = STRIP2_LEDS - 1;
      cur = 0;
      resetting = false;
    } else {
      fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
      // Acende pixels já empilhados
      for (int i = stackPos + 1; i < STRIP2_LEDS; i++) leds2[i] = strip2ColorDisplay;
      // Acende pixel em movimento
      if (cur <= stackPos) leds2[cur] = strip2ColorDisplay;
      cur++;
      if (cur > stackPos) { stackPos--; cur = 0; if (stackPos < 0) resetting = true; }
    }
    lastUpdate = millis();
  }
}

// 34: Pixel Rain — pixels caem de posições aleatórias no topo
void pixelRainStrip2() {
  static unsigned long lastFade = 0;
  static unsigned long lastDrop = 0;
  if (millis() - lastFade >= 40) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(200);
    lastFade = millis();
  }
  if (millis() - lastDrop >= (unsigned long)(strip2Speed * 25)) {
    int pos = random8(STRIP2_LEDS);
    leds2[pos] = strip2ColorDisplay;
    lastDrop = millis();
  }
}

// 35: Knight Rider — bloco de 5 pixels com gradiente vai e volta
void knightRiderStrip2() {
  static int pos = 0;
  static int dir = 1;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 18)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    for (int i = -2; i <= 2; i++) {
      int p = pos + i;
      if (p >= 0 && p < STRIP2_LEDS) {
        uint8_t sc = 255 - (uint8_t)(abs(i) * 50);
        leds2[p] = strip2ColorDisplay;
        leds2[p].nscale8(sc);
      }
    }
    pos += dir;
    if (pos >= STRIP2_LEDS - 1) { pos = STRIP2_LEDS - 1; dir = -1; }
    if (pos <= 0)                { pos = 0;               dir =  1; }
    lastUpdate = millis();
  }
}

// 36: Color Segments — divide o strip em 4 segmentos com hues diferentes
void colorSegmentsStrip2() {
  static uint8_t hue = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 30)) {
    int seg = STRIP2_LEDS / 4;
    for (int i = 0; i < 4; i++) {
      CRGB c = CHSV(hue + i * 64, 255, 255);
      c.nscale8((uint8_t)strip2Brightness);
      for (int j = i * seg; j < (i + 1) * seg && j < STRIP2_LEDS; j++) leds2[j] = c;
    }
    hue += 4;
    lastUpdate = millis();
  }
}

// 37: Shooting Star — estrela cadente com rastro longo que apaga
void shootingStarStrip2() {
  static int pos = -5;
  static unsigned long lastUpdate = 0;
  const int tailLen = 8;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 12)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    for (int i = 0; i < tailLen; i++) {
      int p = pos - i;
      if (p >= 0 && p < STRIP2_LEDS) {
        uint8_t sc = 255 - (uint8_t)(i * (255 / tailLen));
        leds2[p] = strip2ColorDisplay;
        leds2[p].nscale8(sc);
      }
    }
    pos++;
    if (pos >= STRIP2_LEDS + tailLen) pos = -tailLen;
    lastUpdate = millis();
  }
}

// 38: Heartbeat — dois pulsos rápidos seguidos de pausa (BPM 60)
void heartbeatStrip2() {
  static unsigned long lastUpdate = 0;
  static uint8_t step = 0;
  // steps: 0=on1,1=off1,2=on2,3=off2,4-8=pause
  const uint16_t timing[] = {80, 80, 80, 150, 300, 300, 300, 300, 300};
  if (millis() - lastUpdate >= timing[step]) {
    bool on = (step == 0 || step == 2);
    CRGB c = on ? strip2ColorDisplay : CRGB::Black;
    fill_solid(leds2, STRIP2_LEDS, c);
    step++;
    if (step >= 9) step = 0;
    lastUpdate = millis();
  }
}

// 39: Marquee — texto de barra deslizando: blocos de 2 on + 2 off
void marqueeStrip2() {
  static uint8_t offset = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 30)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      leds2[i] = ((i + offset) % 4 < 2) ? strip2ColorDisplay : CRGB::Black;
    }
    offset = (offset + 1) % 4;
    lastUpdate = millis();
  }
}

// 40: Cascade — onda de acender e apagar em cascata bidirecional
void cascadeStrip2() {
  static int pos = 0;
  static int dir = 1;
  static uint8_t phase = 0; // 0=acende, 1=apaga
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 12)) {
    leds2[pos] = (phase == 0) ? strip2ColorDisplay : CRGB::Black;
    pos += dir;
    if (pos >= STRIP2_LEDS) { pos = STRIP2_LEDS - 1; dir = -1; phase ^= 1; }
    if (pos < 0)             { pos = 0;               dir =  1; phase ^= 1; }
    lastUpdate = millis();
  }
}

// 41: Sparkle Burst — explosão de cor a partir do centro para as pontas
void sparkleBurstStrip2() {
  static unsigned long lastBurst = 0;
  static bool active = false;
  static int radius = 0;
  int center = STRIP2_LEDS / 2;
  if (!active && millis() - lastBurst >= (unsigned long)(strip2Speed * 200)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    active = true;
    radius = 0;
    lastBurst = millis();
  }
  if (active && millis() - lastBurst >= (unsigned long)(strip2Speed * 15)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    int left  = center - radius;
    int right = center + radius;
    if (left  >= 0)           leds2[left]  = strip2ColorDisplay;
    if (right < STRIP2_LEDS)  leds2[right] = strip2ColorDisplay;
    radius++;
    if (radius > center) { active = false; lastBurst = millis(); }
    lastBurst = millis();
  }
}

// 42: Dual Color Wipe — dois frontais opostos preenchem ao mesmo tempo
void dualColorWipeStrip2() {
  static int posL = 0;
  static int posR = STRIP2_LEDS - 1;
  static bool filling = true;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    if (filling) {
      if (posL <= posR) {
        leds2[posL] = strip2ColorDisplay;
        leds2[posR] = strip2ColorDisplay;
        posL++; posR--;
      } else { filling = false; }
    } else {
      posL = max(0, posL - 1);
      posR = min(STRIP2_LEDS - 1, posR + 1);
      leds2[posL] = CRGB::Black;
      leds2[posR] = CRGB::Black;
      if (posL == 0 && posR == STRIP2_LEDS - 1) {
        fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
        posL = 0; posR = STRIP2_LEDS - 1; filling = true;
      }
    }
    lastUpdate = millis();
  }
}

// 43: Northern Lights — paleta ForestColors flutuando lentamente
void northernLightsStrip2() {
  static uint16_t index = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 20)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t idx = (uint8_t)((index + i * 7 + sin8(i * 13 + (index >> 1))) & 0xFF);
      leds2[i] = ColorFromPalette(ForestColors_p, idx, 255, LINEARBLEND);
    }
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    index += 1;
    lastUpdate = millis();
  }
}

// 44: Rainbow Segments — arco-íris dividido em segmentos fixos que rotacionam
void rainbowSegmentsStrip2() {
  static uint8_t hue = 0;
  static unsigned long lastUpdate = 0;
  const int segSize = 6;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 25)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      int seg = (i + hue / 4) / segSize;
      leds2[i] = CHSV(seg * 32 + hue, 255, 255);
    }
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    hue++;
    lastUpdate = millis();
  }
}

// 45: Pulse Wave — pulso de brilho se propaga como onda do LED 0 ao 35
void pulseWaveStrip2() {
  static uint8_t phase = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 8)) {
    for (int i = 0; i < STRIP2_LEDS; i++) {
      uint8_t val = sin8(phase - (uint8_t)(i * 255 / STRIP2_LEDS));
      leds2[i] = strip2ColorDisplay;
      leds2[i].nscale8(val);
    }
    phase += 6;
    lastUpdate = millis();
  }
}

// 46: Orbit — dois pixels opostos giram em torno do strip
void orbitStrip2() {
  static uint8_t pos = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 15)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    int p1 = pos % STRIP2_LEDS;
    int p2 = (pos + STRIP2_LEDS / 2) % STRIP2_LEDS;
    leds2[p1] = strip2ColorDisplay;
    leds2[p2] = CRGB(strip2ColorDisplay.b, strip2ColorDisplay.r, strip2ColorDisplay.g); // complementar rotacionado
    pos++;
    lastUpdate = millis();
  }
}

// 47: Color Drip — pixels caem e acumulam no fundo com cor do arco-íris
void colorDripStrip2() {
  static uint8_t hue = 0;
  static unsigned long lastFade = 0;
  static unsigned long lastDrop = 0;
  if (millis() - lastFade >= 50) {
    for (int i = STRIP2_LEDS - 1; i > 0; i--) leds2[i] = leds2[i - 1]; // desloca para baixo
    leds2[0] = CRGB::Black;
    lastFade = millis();
  }
  if (millis() - lastDrop >= (unsigned long)(strip2Speed * 60)) {
    CRGB c = CHSV(hue, 255, 255);
    c.nscale8((uint8_t)strip2Brightness);
    leds2[0] = c;
    hue += 11;
    lastDrop = millis();
  }
}

// 48: Firefly — 4 pontos independentes com velocidades e fases diferentes
void fireflyStrip2() {
  static unsigned long lastUpdate = 0;
  static uint8_t pos[4]  = {0, 9, 18, 27};
  static int8_t  spd2[4] = {1, -1, 1, -1};
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 25)) {
    fill_solid(leds2, STRIP2_LEDS, CRGB::Black);
    for (int f = 0; f < 4; f++) {
      CRGB c = CHSV(f * 64, 200, 255);
      c.nscale8((uint8_t)strip2Brightness);
      leds2[pos[f]] = c;
      pos[f] = (uint8_t)((pos[f] + spd2[f] + STRIP2_LEDS) % STRIP2_LEDS);
    }
    lastUpdate = millis();
  }
}

// 49: Glitter Rainbow — arco-íris estático com flashes brancos aleatórios
void glitterRainbowStrip2() {
  static uint8_t hue = 0;
  static unsigned long lastRainbow = 0;
  static unsigned long lastGlitter = 0;
  if (millis() - lastRainbow >= (unsigned long)(strip2Speed * 20)) {
    fill_rainbow(leds2, STRIP2_LEDS, hue, 256 / STRIP2_LEDS);
    nscale8(leds2, STRIP2_LEDS, (uint8_t)strip2Brightness);
    hue++;
    lastRainbow = millis();
  }
  if (millis() - lastGlitter >= (unsigned long)(strip2Speed * 8)) {
    leds2[random8(STRIP2_LEDS)] = CRGB::White;
    lastGlitter = millis();
  }
}

// 50: Popcorn — pixels acendem aleatoriamente e desaparecem rápido
void popcornStrip2() {
  static unsigned long lastFade = 0;
  static unsigned long lastPop = 0;
  if (millis() - lastFade >= 20) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(230);
    lastFade = millis();
  }
  if (millis() - lastPop >= (unsigned long)(strip2Speed * 10)) {
    for (int i = 0; i < 3; i++) leds2[random8(STRIP2_LEDS)] = strip2ColorDisplay;
    lastPop = millis();
  }
}

// 51: Sunrise — fade de preto -> laranja -> amarelo -> branco (loop)
void sunriseStrip2() {
  static uint8_t val = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= (unsigned long)(strip2Speed * 12)) {
    CRGB c;
    if      (val < 85)  c = CRGB(val * 3, 0, 0);                           // preto -> vermelho
    else if (val < 170) c = CRGB(255, (val - 85) * 3, 0);                  // vermelho -> laranja/amarelo
    else                c = CRGB(255, 255, (val - 170) * 3);               // amarelo -> branco
    c.nscale8((uint8_t)strip2Brightness);
    fill_solid(leds2, STRIP2_LEDS, c);
    val++; // wrap automático via uint8_t overflow
    lastUpdate = millis();
  }
}

// 52: Matrix — colunas de pixels verdes caindo em velocidades diferentes (ignora strip2Color)
void matrixStrip2() {
  static unsigned long lastFade = 0;
  static unsigned long lastDrop = 0;
  if (millis() - lastFade >= 40) {
    for (int i = 0; i < STRIP2_LEDS; i++) leds2[i].nscale8(180);
    lastFade = millis();
  }
  if (millis() - lastDrop >= (unsigned long)(strip2Speed * 12)) {
    int pos = random8(STRIP2_LEDS);
    CRGB c = CRGB(0, 255, random8(60, 150));
    c.nscale8((uint8_t)strip2Brightness);
    leds2[pos] = c;
    lastDrop = millis();
  }
}

void runStrip2Effect() {
  switch (strip2EffectActive) {
    case 1:  rainbowStrip2();      break;
    case 2:  ledChase36();         break;
    case 3:  cometStrip2();        break;
    case 4:  breatheStrip2();      break;
    case 5:  strobeStrip2();       break;
    case 6:  colorWipeStrip2();    break;
    case 7:  twinkleStrip2();      break;
    case 8:  fireStrip2();         break;
    case 9:  scannerStrip2();      break;
    case 10: fillFadeStrip2();     break;
    case 11: alternatingStrip2();  break;
    case 12: solidColorStrip2();   break;
    case 13: rainbowChaseStrip2(); break;
    case 14: bounceStrip2();       break;
    case 15: meteorStrip2();       break;
    case 16: sparkleStrip2();      break;
    case 17: runningLightsStrip2();break;
    case 18: colorCycleStrip2();   break;
    case 19: pacificaStrip2();     break;
    case 20: dualScannerStrip2();  break;
    case 21: lavaStrip2();         break;
    case 22: iceStrip2();          break;
    case 23: wipeInOutStrip2();    break;
    case 24: fadeInOutStrip2();    break;
    case 25: theaterChaseStrip2(); break;
    case 26: lightningStrip2();    break;
    case 27: confettiStrip2();     break;
    case 28: gradientShiftStrip2();break;
    case 29: plasmaStrip2();       break;
    case 30: rippleStrip2();       break;
    case 31: cylonStrip2();        break;
    case 32: sinelonStrip2();      break;
    case 33: pixelStackStrip2();   break;
    case 34: pixelRainStrip2();    break;
    case 35: knightRiderStrip2();  break;
    case 36: colorSegmentsStrip2();break;
    case 37: shootingStarStrip2(); break;
    case 38: heartbeatStrip2();    break;
    case 39: marqueeStrip2();      break;
    case 40: cascadeStrip2();      break;
    case 41: sparkleBurstStrip2(); break;
    case 42: dualColorWipeStrip2();break;
    case 43: northernLightsStrip2();break;
    case 44: rainbowSegmentsStrip2();break;
    case 45: pulseWaveStrip2();    break;
    case 46: orbitStrip2();        break;
    case 47: colorDripStrip2();    break;
    case 48: fireflyStrip2();      break;
    case 49: glitterRainbowStrip2();break;
    case 50: popcornStrip2();      break;
    case 51: sunriseStrip2();      break;
    case 52: matrixStrip2();       break;
    default: break; // 0 = desligado
  }
  // Brilho aplicado DENTRO de cada efeito — não aqui (evita acumulação frame a frame)
}

//******************************************************************************************************************