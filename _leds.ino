//******************************************************************************************************************
// DISPLAYS DE 7 SEGMENTOS COM FASTLED
//******************************************************************************************************************

// Variável global para armazenar a cor atual da fita
CRGB currentStripColor = CRGB::White;
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
// Mostra a cada 15s: 3s dd/mm, depois 3s aaaa, com dois pontos apagados
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

  // Garante tamanho mínimo de strip para HH:MM com dois pontos (28 + 2 = 30 LEDs)
  if (!stripChecked) {
    long required = (long)startLed + 30;
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
    // Dois pontos apagados
    leds[startLed + 14] = CRGB::Black;
    leds[startLed + 15] = CRGB::Black;

    // Renderiza letras estáticas: F no dígito das centenas, C no de milhares
    for (uint8_t s = 0; s < 7; s++) {
      // 'F' = Topo(1), SupEsq(2), Meio(3), InfEsq(6)
      bool onF = (s == 1) || (s == 2) || (s == 3) || (s == 6);
      leds[startLed + 16 + s] = onF ? CRGB::Red : CRGB::Black;

      // 'C' = Topo(1), SupEsq(2), Base(5), InfEsq(6)
      bool onC = (s == 1) || (s == 2) || (s == 5) || (s == 6);
      leds[startLed + 23 + s] = onC ? CRGB::Red : CRGB::Black;
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
    if (!dateCycleActive && (now - dateCycleLastStartMs) >= 15000UL) {
      dateCycleActive = true;
      dateCyclePhase = 1; // dd/mm
      dateCyclePhaseStartMs = now;
    }

    if (dateCycleActive) {
      // dois pontos apagados durante o ciclo
      leds[startLed + 14] = CRGB::Black;
      leds[startLed + 15] = CRGB::Black;

      if (dateCyclePhase == 1) {
        // Exibir dd/mm nos 4 dígitos
        // Dia (DD) nos displays de milhares e centenas (esquerda): D3 (startLed+23), D2 (startLed+16)
        // Mês (MM) nos displays de dezenas e unidades (direita): D1 (startLed+7), D0 (startLed)
        uint8_t day = (uint8_t)t.tm_mday;         // 1-31
        uint8_t month = (uint8_t)(t.tm_mon + 1);  // 1-12

        // Dia
        setSevenSegmentDisplay(3, day / 10, startLed + 23);
        setSevenSegmentDisplay(2, day % 10, startLed + 16);
        // Mês
        setSevenSegmentDisplay(1, month / 10, startLed + 7);
        setSevenSegmentDisplay(0, month % 10, startLed);
        FastLED.show();

        if ((now - dateCyclePhaseStartMs) >= 3000UL) {
          dateCyclePhase = 2; // aaaa
          dateCyclePhaseStartMs = now;
        }
        return; // não renderiza hora enquanto ciclo ativo
      }
      else if (dateCyclePhase == 2) {
        // Exibir ano aaaa nos 4 displays: D3 (milhar), D2 (centena), D1 (dezena), D0 (unidade)
        uint16_t year = (uint16_t)(t.tm_year + 1900);
        uint8_t thousands = year / 1000;
        uint8_t hundreds  = (year % 1000) / 100;
        uint8_t tens      = (year % 100) / 10;
        uint8_t ones      = year % 10;

        setSevenSegmentDisplay(3, thousands, startLed + 23);
        setSevenSegmentDisplay(2, hundreds,  startLed + 16);
        setSevenSegmentDisplay(1, tens,      startLed + 7);
        setSevenSegmentDisplay(0, ones,      startLed);
        FastLED.show();

        if ((now - dateCyclePhaseStartMs) >= 3000UL) {
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

  // Dois pontos conforme modo
  if (clock12hMode) {
    if (!isPM) {
      leds[startLed + 14] = currentStripColor;
      leds[startLed + 15] = colonOn ? currentStripColor : CRGB::Black;
    } else {
      leds[startLed + 14] = colonOn ? currentStripColor : CRGB::Black;
      leds[startLed + 15] = colonOn ? currentStripColor : CRGB::Black;
    }
  } else {
    leds[startLed + 14] = colonOn ? currentStripColor : CRGB::Black;
    leds[startLed + 15] = colonOn ? currentStripColor : CRGB::Black;
  }

  // Horas (após os dois LEDs dos dois pontos)
  setSevenSegmentDisplay(2, dispHours % 10, startLed + 16);    // unidades das horas
  setSevenSegmentDisplay(3, dispHours / 10, startLed + 23);    // dezenas das horas

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