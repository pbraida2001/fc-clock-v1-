#include <Arduino.h>
#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\fc-clock(v1).ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 23/01/2023 All Rights Reserved
//******************************************************************************************************************

#define FIRMWARE_VERSION        "0309202501"
#define FIRMWARE_DATE           "03/09/2025"
#define PRODUCT_MODEL           "FC-Clock"

//******************************************************************************************************************
// TEMPLATE_PLACEHOLDER FOR WEB SERVER
//******************************************************************************************************************

#define TEMPLATE_PLACEHOLDER '$'

//******************************************************************************************************************
// INCLUDE FILES
//******************************************************************************************************************

#include <WiFi.h>
#include <Update.h>
#include <AsyncTCP.h>
#include <AsyncUDP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <ESP32Time.h>
#include <time.h>
#include <jsonlib.h>
#include <ArduinoJson.h>

//******************************************************************************************************************
// STRUCTS
//******************************************************************************************************************

struct PKG_BUFFER {

  String    type;
  String    data;
  uint16_t  data_len;
  IPAddress local_ip;
  long      local_port;
  IPAddress remote_ip;
  long      remote_port;
  AsyncClient* client;

};

struct _WIFI_CFG {

  int status = 2;
  String radio_status = "0";
  String ssid;
  String password;
  String ip;
  String netmask;
  String gtw;
  String ap_password;
  String port;
  String dns;

};

struct _PORT_CFG {

  String udp;
  String tcp;

};

struct _CFG {

   uint8_t cmode=0; 
   uint8_t cdate=0; 

};

//******************************************************************************************************************
// IOs DEFINES
//******************************************************************************************************************

//******************************************************************************************************************
// LEDS
//******************************************************************************************************************

#define LED1    12 
#define LED2    13

//******************************************************************************************************************
// BUTTON
//******************************************************************************************************************

#define BTN1    2
#define BTN2    4

//******************************************************************************************************************
// LED DIGITAL
//******************************************************************************************************************

#define DATA_PIN    19
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define BRIGHTNESS          100
#define FRAMES_PER_SECOND  120
#define TOTAL_EFEITOS  39

//******************************************************************************************************************
// GLOBAL VARS
//******************************************************************************************************************

bool firmware_update=0;

//******************************************************************************************************************
// TASKS
//******************************************************************************************************************

TaskHandle_t Task1;
TaskHandle_t Task2;

//******************************************************************************************************************
// STATION MAC ADDRESS
//******************************************************************************************************************

uint8_t baseMac[6];

//******************************************************************************************************************
// PREFERENCIAS
//******************************************************************************************************************

Preferences preferences;

//******************************************************************************************************************
// CONFIG MODE
//******************************************************************************************************************

String config_mode = "0";

//******************************************************************************************************************
// PORTS VARS
//******************************************************************************************************************

_PORT_CFG port;

//******************************************************************************************************************
// WI-FI VARS
//******************************************************************************************************************

_WIFI_CFG wifi;

uint32_t wifi_interval = 30000;

IPAddress wifi_ip;
IPAddress wifi_gateway;
IPAddress wifi_subnet;
IPAddress wifi_primary_DNS;
IPAddress secondaryDNS(8, 8, 4, 4);

uint32_t wifi_interval_ms;
uint8_t start_wifi_counter = 0;

uint8_t gCurrentPatternNumber = 0;
uint8_t gHue = 0;

//******************************************************************************************************************
// TCP/UDP SERVER VARS
//******************************************************************************************************************

#define PKG_BUFFER_SIZE 10

PKG_BUFFER    pkgbuffer[PKG_BUFFER_SIZE];

int new_pkg = 0;
int read_pkg = 0;

static std::vector<AsyncClient*> clients; // a list to hold all clients

//******************************************************************************************************************
// FORWARD VARS
//******************************************************************************************************************

IPAddress fb_ip_list[3];
uint32_t  fb_port_list[3];

//******************************************************************************************************************
// UPDATE FIRMWARE
//******************************************************************************************************************

unsigned long update_interval_ms=100;
bool update_status=false;

uint8_t last_firmware=0;

//******************************************************************************************************************
// WEB SERVER VARS
//******************************************************************************************************************

char http_username[50];
char http_password[50];

String mio_loc;

#define U_PART U_SPIFFS

size_t content_len;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncUDP udp;

static bool init_web_server=1;

uint16_t pin_number=1234;

//******************************************************************************************************************
// RTC/NTP SYNC VARS
//******************************************************************************************************************

static bool rtc_synced = false;
static unsigned long last_rtc_sync_ms = 0;

// Forward declarations
void syncRTCWithNTP();

//******************************************************************************************************************
// RESETA
//******************************************************************************************************************

uint8_t reseta=0;
uint8_t fboot=1;   

long stripPixels = 7;     // valor inicial
int stripBrightness = 100;  // valor inicial
int color_r = 255;          // valor inicial
int color_g = 255;          // valor inicial
int color_b = 255;          // valor inicial
int efeito = 0;           // valor inicial
int speed = 10;          // valor inicial
int dataPin = 1;          // valor inicial

CRGB* leds = nullptr;

//******************************************************************************************************************
// CONFIGURACAO CLOCK
//******************************************************************************************************************

_CFG cfg;

//******************************************************************************************************************
// SETUP
//******************************************************************************************************************

#line 255 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\fc-clock(v1).ino"
void setup();
#line 344 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\fc-clock(v1).ino"
void wifiTask(void *pvParameters);
#line 376 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\fc-clock(v1).ino"
void fastledTask(void *pvParameters);
#line 398 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\fc-clock(v1).ino"
void loop();
#line 10 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_ios.ino"
void ios_setup(void);
#line 44 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void setStripColor(uint8_t r, uint8_t g, uint8_t b);
#line 63 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void setStripBrightness(uint8_t brightness);
#line 97 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void set_strip(long pixels, int brightness, uint8_t r, uint8_t g, uint8_t b);
#line 130 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void setSevenSegmentDisplay(uint8_t displayIndex, uint8_t digit, uint16_t startLed);
#line 159 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void showNumberOnDisplays(uint32_t number, uint8_t numDisplays, uint16_t startLed);
#line 189 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void sevenSegmentCounter(uint16_t startLed, uint8_t numDisplays, uint32_t maxCount);
#line 214 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void showTimeOnDisplays(uint8_t hours, uint8_t minutes, uint16_t startLed, bool showColon);
#line 240 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void sevenSegmentCounterSimple(uint16_t startLed);
#line 265 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void testSevenSegmentDigit(uint16_t startLed, uint8_t digit);
#line 307 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void twoDisplayCounter0to99(uint16_t startLed);
#line 344 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void threeDisplayCounter0to999(uint16_t startLed);
#line 385 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void fourDisplayCounter0to9999(uint16_t startLed);
#line 436 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void setClockDisplayMode(uint8_t mode);
#line 447 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void setClockDateCycle(uint8_t mode);
#line 463 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void displayTimeFromRTC(uint16_t startLed);
#line 634 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void fiveDisplayCounter0to99999(uint16_t startLed);
#line 676 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
void sixDisplayCounter0to999999(uint16_t startLed);
#line 7 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_memory.ino"
void clear_cfg_memory();
#line 44 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_memory.ino"
void reset_cfg_memory();
#line 59 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_memory.ino"
void check_config_mode(void);
#line 96 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_memory.ino"
void read_config();
#line 539 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_netconfig_page.ino"
String index_processor(const String& var);
#line 22 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_parse.ino"
void parse_new_package(void);
#line 275 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_reset_page.ino"
String reset_processor(const String& var);
#line 583 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_stools_page.ino"
String update_processor(const String& var);
#line 10 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
static void handleDisconnect(void* arg, AsyncClient* client);
#line 22 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
static void handleData(void* arg,AsyncClient* client,void *data,size_t len);
#line 84 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
static void handleError(void* arg, AsyncClient* client, int8_t error);
#line 94 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
static void handleNewClient(void* arg,AsyncClient* client);
#line 113 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
static void handleTimeOut(void* arg, AsyncClient* client, uint32_t time);
#line 125 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
void set_tcp_server(void);
#line 10 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_udp.ino"
void set_udp_server(void);
#line 84 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_udp.ino"
void send_message_udp(String msg,IPAddress addr,uint16_t port);
#line 60 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_utils.ino"
void check_efeito(void);
#line 81 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_utils.ino"
void check_update(void);
#line 159 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_utils.ino"
void envia_firmware(void);
#line 190 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_utils.ino"
void rip(void);
#line 10 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_webserver.ino"
void printProgress(size_t prg, size_t sz);
#line 39 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_webserver.ino"
void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
#line 99 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_webserver.ino"
void webserver_start(void);
#line 10 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_websocket.ino"
void notifyClients(String sensorReadings);
#line 20 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_websocket.ino"
void initWebSocket();
#line 31 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_websocket.ino"
void onEvent(AsyncWebSocket *server,AsyncWebSocketClient *client,AwsEventType type,void *arg,uint8_t *data,size_t len);
#line 56 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_websocket.ino"
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
#line 10 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_wifi.ino"
void wifi_operation_mode(void);
#line 130 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_wifi.ino"
void check_wifi(void );
#line 255 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\fc-clock(v1).ino"
void setup() {

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // SETUP RS232(DEBUG)
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  Serial.begin(115200);

  Serial.println("---------------------------------------------------------------------------------");
  Serial.println("FC-Digital Led Starting Configuration...");
  Serial.println("---------------------------------------------------------------------------------");

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // SETUP IOS
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  ios_setup();

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // START SPIFFS
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  if (!SPIFFS.begin(true)) {

    Serial.println("---------------------------------------------------------------------------------");
    Serial.println("An Error has occurred while mounting SPIFFS");
    Serial.println("---------------------------------------------------------------------------------");

  } else {

    Serial.println("---------------------------------------------------------------------------------");
    Serial.println("SPIFFS Mounting ok...");
    Serial.println("---------------------------------------------------------------------------------");
    Serial.printf("Total Space:%lu\n\r", SPIFFS.totalBytes());
    Serial.printf("Used  Space:%lu\n\r", SPIFFS.usedBytes());
    Serial.printf("Free  Space:%lu\n\r", SPIFFS.totalBytes() - SPIFFS.usedBytes());
    Serial.println("---------------------------------------------------------------------------------");

  }

  check_config_mode();
  read_config();
  wifi_operation_mode();

  set_tcp_server();
  set_udp_server();

  set_strip(stripPixels,stripBrightness,color_r,color_g,color_b);
  setClockDisplayMode(cfg.cmode);
  setClockDateCycle(cfg.cdate);

  initWebSocket();

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // TASK FAST LED
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    xTaskCreatePinnedToCore(
        fastledTask,
        "FastLEDTask",
        8192,
        NULL,
        3,
        NULL,
        0 // core 0
    );

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // TASK WI-FI
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    xTaskCreatePinnedToCore(
        wifiTask,
        "WiFiTask",
        8192,
        NULL,
        1,
        NULL,
        1 // core 1
    );

  delay(500);

}

//******************************************************************************************************************
// WIFI Task
//******************************************************************************************************************

void wifiTask(void *pvParameters) {

    for (;;) {

        if(!firmware_update) {

            check_wifi();
            parse_new_package();
            //check_efeito();

          // Sincroniza RTC via NTP quando Wi-Fi conectado e não sincronizado ou após 1h
          if (WiFi.status() == WL_CONNECTED && (!rtc_synced || (millis() - last_rtc_sync_ms) > 3600000)) {
            
            syncRTCWithNTP();
          
          }
        
        }

        //feedTheDog();
        ws.cleanupClients();
        check_update();
        vTaskDelay(10);

  }

}

//******************************************************************************************************************
// FastLED Task
//******************************************************************************************************************

void fastledTask(void *pvParameters) {

  //set_strip(stripPixels,255,128,0,128);

    for (;;) {

      if(!firmware_update) {
        // Exibe relógio HH:MM com dois pontos piscando a cada 500ms
        displayTimeFromRTC(0);

      }

      vTaskDelay(10);

    }

}

//******************************************************************************************************************
// loop
//******************************************************************************************************************

void loop() {
}

//******************************************************************************************************************


#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_ios.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// FUNCTION SETUP IOs
//******************************************************************************************************************

void ios_setup(void) {

  pinMode(BTN1,INPUT_PULLUP);
  pinMode(BTN2,INPUT_PULLUP);

  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
   
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_leds.ino"
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
#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_memory.ino"

//******************************************************************************************************************
// FUNCTION CLEAR CONFIGURATION MEMORY
//{"command":"clear_cfg_memory","type":0}
//******************************************************************************************************************

void clear_cfg_memory() {

  char aux_rip[100];

  Serial.println("Clear Config Memory...");

  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);  
  
  preferences.begin("memory_cfg",false); 
  preferences.clear();  
  preferences.end();   

  sprintf(aux_rip,"{\"command\":\"clear_config_memory\",\"type\":1}");

  if(pkgbuffer[read_pkg].type=="UDP") { 
      
    send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

   } else {

    String rc(aux_rip);
    char aux_aux[300];

    rc.toCharArray(aux_aux,rc.length()+1);  

    pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
    pkgbuffer[read_pkg].client->send();

  }  
  
}

//******************************************************************************************************************
// FUNCTION RESET CONFIGURATION DATA
//******************************************************************************************************************

void reset_cfg_memory() {

  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);

  preferences.begin("memory_cfg",false); 
  preferences.clear();  
  preferences.end();  
  
}

//******************************************************************************************************************
// CONFIGURACAO DE REDE.HTML PAGE
//******************************************************************************************************************

void check_config_mode(void) {

  digitalWrite(LED1,HIGH); 
      
  if(digitalRead(BTN2)==LOW) { 

    delay(1000);

    if(digitalRead(BTN2)==LOW) {  

      config_mode="1";

      preferences.begin("memory_cfg",false); 
      
      preferences.putString("config_mode","1");
      
      preferences.putString("ap_password","Flex12345");
      
      preferences.putString("acc_user","admin");
      preferences.putString("acc_password","Flex12345");
         
      preferences.end();

    } else {

      config_mode="0";
      
    }

  }
    
}

//******************************************************************************************************************
// READ CONFIG MEMORY
//******************************************************************************************************************

void read_config() {

  bool x;

  preferences.begin("memory_cfg",false); 

  config_mode=preferences.getString("config_mode",""); 

  if(config_mode=="") {

    preferences.putString("config_mode","1");
    config_mode=preferences.getString("config_mode",""); 
    
  }

  wifi.radio_status=preferences.getString("radio_status",""); 

  if(wifi.radio_status=="") {

    preferences.putString("radio_status","0");
    wifi.radio_status=preferences.getString("radio_status",""); 
    
  } 

  preferences.getString("acc_user","").toCharArray(http_username,preferences.getString("acc_user","").length()+1); 

  if(preferences.getString("acc_user","")=="") {

    preferences.putString("acc_user","admin");
    preferences.getString("acc_user","").toCharArray(http_username,preferences.getString("acc_user","").length()+1); 
    
  }
  
  preferences.getString("acc_password","").toCharArray(http_password,preferences.getString("acc_password","").length()+1); 

  if(preferences.getString("acc_password","")=="") {

    preferences.putString("acc_password","Flex12345");
    preferences.getString("acc_password","").toCharArray(http_password,preferences.getString("acc_password","").length()+1); 
    
  } 

  wifi.ssid=preferences.getString("wifi_ssid",""); 
  wifi.password=preferences.getString("wifi_password",""); 
  wifi.ap_password=preferences.getString("ap_password",""); 

  if(wifi.ap_password=="") {

    preferences.putString("ap_password","Flex12345");
    wifi.ap_password=preferences.getString("ap_password",""); 
    
  }    

  wifi.ip=preferences.getString("wifi_ip",""); 
  wifi.netmask=preferences.getString("wifi_nm","");
  wifi.gtw=preferences.getString("wifi_gtw","");
  wifi.dns=preferences.getString("wifi_dns",""); 

  if(wifi.dns=="") {

    preferences.putString("wifi_dns","8.8.8.8");
    wifi.dns=preferences.getString("wifi_dns",""); 
    
  } 
  
  port.tcp=preferences.getString("tcp_port","");

  if(port.tcp=="") {

    preferences.putString("tcp_port","6666");
    port.tcp=preferences.getString("tcp_port",""); 
    
  } 
  
  port.udp=preferences.getString("udp_port","");

  if(port.udp=="") {

    preferences.putString("udp_port","9999");
    port.udp=preferences.getString("udp_port",""); 
    
  }  

  if(config_mode!="1") {

    fb_ip_list[0].fromString(preferences.getString("fb_ip1",""));
    fb_ip_list[1].fromString(preferences.getString("fb_ip2",""));
    fb_ip_list[2].fromString(preferences.getString("fb_ip3",""));

    if(preferences.getString("fb_pt1","")=="") { 

      preferences.putString("fb_pt1",port.udp);

    }

    fb_port_list[0]=preferences.getString("fb_pt1","").toInt();

    if(preferences.getString("fb_pt2","")=="") { 

      preferences.putString("fb_pt2",port.udp);
   
    }

    fb_port_list[1]=preferences.getString("fb_pt2","").toInt();   

    if(preferences.getString("fb_pt3","")=="") { 

      preferences.putString("fb_pt3",port.udp);

    } 
  
    fb_port_list[2]=preferences.getString("fb_pt3","").toInt();

  }  

  stripPixels=preferences.getString("n_leds","").toInt();

  if(preferences.getString("n_leds","")=="") {

    preferences.putString("n_leds","10");
    stripPixels=preferences.getString("n_leds","").toInt();

  }

  stripBrightness=preferences.getString("brigh","").toInt();

  if(preferences.getString("brigh","")=="") {

    preferences.putString("brigh","10");
    stripBrightness=preferences.getString("brigh","").toInt();

  }

  color_r=preferences.getString("cr","").toInt();

  if(preferences.getString("cr","")=="") {

    preferences.putString("cr","0");
    color_r=preferences.getString("cr","").toInt();

  }

  color_g=preferences.getString("cg","").toInt();

  if(preferences.getString("cg","")=="") {

    preferences.putString("cg","0");
    color_g=preferences.getString("cg","").toInt();

  }

  color_b=preferences.getString("cb","").toInt();

  if(preferences.getString("cb","")=="") {

    preferences.putString("cb","0");
    color_b=preferences.getString("cb","").toInt();

  }

  efeito=preferences.getString("efeito","").toInt();

  if(preferences.getString("efeito","")=="") {

    preferences.putString("efeito","0");
    efeito=preferences.getString("efeito","").toInt();

  }

  speed=preferences.getString("speed","").toInt();

  if(preferences.getString("speed","")=="") {

    preferences.putString("speed","0");
    speed=preferences.getString("speed","").toInt();

  }

  dataPin=preferences.getString("dataPin","").toInt();

  if(preferences.getString("dataPin","")=="") {

    preferences.putString("dataPin","1");
    dataPin=preferences.getString("dataPin","").toInt();

  }

  cfg.cmode=preferences.getString("cmode","").toInt();

  if(preferences.getString("cmode","")=="") {

    preferences.putString("cmode","0");
    cfg.cmode=preferences.getString("cmode","").toInt();

  }

  cfg.cdate=preferences.getString("cdate","").toInt();

  if(preferences.getString("cdate","")=="") {

    preferences.putString("cdate","0");
    cfg.cdate=preferences.getString("cdate","").toInt();

  }  

  size_t tmhpref=preferences.freeEntries();

  Serial.println("---------------------------------------------------------------------------------");

  preferences.end();

  Serial.println("---------------------------------------------------------------------------------");
  Serial.println("Configuration Data Save in memory:");
  Serial.println("---------------------------------------------------------------------------------"); 
  Serial.printf("Livre : %u\n\r",tmhpref);
  Serial.println("---------------------------------------------------------------------------------"); 
  Serial.print("Config Mode : ");
  Serial.println(config_mode);

  Serial.print("ACCOUNT USER : ");
  Serial.println(http_username);
  Serial.print("ACCOUNT PASSWORD : ");
  Serial.println(http_password);  
  Serial.print("Radio Status : ");
  Serial.println(wifi.radio_status);
  Serial.print("Wi-Fi SSID : ");
  Serial.println(wifi.ssid);
  Serial.print("Wi-Fi Password : ");
  Serial.println(wifi.password); 
  Serial.print("AP Password : ");
  Serial.println(wifi.ap_password);   
  Serial.print("Wi-Fi IP : ");
  Serial.println(wifi.ip);  
  Serial.print("Wi-Fi NETMASK : ");
  Serial.println(wifi.netmask);  
  Serial.print("Wi-Fi GTW IP : ");
  Serial.println(wifi.gtw);
  Serial.print("Wi-Fi DNS : ");
  Serial.println(wifi.dns);  
 
  Serial.print("UDP PORT : ");
  Serial.println(port.udp);
  Serial.print("TCP PORT : ");
  Serial.println(port.tcp);

  Serial.print("FeedBack IP-1 : ");
  Serial.println(fb_ip_list[0]);
  Serial.print("FeedBack Port-1 : ");
  Serial.println(fb_port_list[0]);
  Serial.print("FeedBack IP-2 : ");
  Serial.println(fb_ip_list[1]);
  Serial.print("FeedBack Port-2 : ");
  Serial.println(fb_port_list[1]);
  Serial.print("FeedBack IP-3 : ");
  Serial.println(fb_ip_list[2]); 
  Serial.print("FeedBack Port-3 : ");
  Serial.println(fb_port_list[2]); 

  Serial.print("Data Pin : ");
  Serial.println(dataPin);
  Serial.print("Strip Pixels : ");
  Serial.println(stripPixels);
  Serial.print("Strip Brightness : ");
  Serial.println(stripBrightness);
  Serial.print("Color R : ");
  Serial.println(color_r);
  Serial.print("Color G : ");
  Serial.println(color_g);
  Serial.print("Color B : ");
  Serial.println(color_b);
  Serial.print("Effect : ");
  Serial.println(efeito);
  Serial.print("Effect Speed : ");
  Serial.println(speed);

  Serial.print("Clock Mode : ");
  Serial.println(cfg.cmode);
  Serial.print("Clock Date : ");
  Serial.println(cfg.cdate);  

  Serial.println("---------------------------------------------------------------------------------"); 
  
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_netconfig_page.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// CONFIGURACAO DE REDE.HTML PAGE
//******************************************************************************************************************
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html>
  <head>
    <title>DBX-Digital Led</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      * {
        box-sizing: border-box;
      }
      body {
        font-family: "Lato", sans-serif;
        background: #f1f1f1;        
      }
      .container {
        display: inline-block;
        cursor: pointer;
        margin: 20px 20px;
      }
      .bar1, .bar2, .bar3 {
        width: 35px;
        height: 5px;
        background-color: #333;
        margin: 6px 0;
        transition: 0.4s;
      }
      .change .bar1 {
        -webkit-transform: rotate(-45deg) translate(-9px, 6px);
        transform: rotate(-45deg) translate(-9px, 6px);
      }
      .change .bar2 {opacity: 0;}
      .change .bar3 {
        -webkit-transform: rotate(45deg) translate(-8px, -8px);
        transform: rotate(45deg) translate(-8px, -8px);
      }      
      .sidebar {
        height: 100%;
        width: 0;
        position: fixed;
        z-index: 1;
        top: 0;
        right: 0;
        background-color: #1D8348;
        overflow-x: hidden;
        transition: 0.5s;
        padding-top: 60px;
      }
      .sidebar a {
        padding: 8px 8px 8px 32px;
        text-decoration: none;
        font-size: 21px;
        color: white;
        display: block;
        transition: 0.3s;
      }
      .sidebar a:hover {
        color: black;
      }
      .sidebar .closebtn {
        position: absolute;
        top: 0;
        right: 25px;
        font-size: 36px;
        margin-left: 50px;
      }
      #main {
        transition: margin-right .5s;
        padding: 16px;
      }      
      .logo {
        padding: 13px;
        float: left;
        width: 78%;     
        text-align: left;           
      }
      .logo_1 {
        padding: 13px;
        float: right;
        width: 90%;     
        text-align: right;           
      }
      .logo_2 {
        padding: 4px;
        float: center;
        width: 100%;     
        text-align: center;           
      } 
      .header {
        padding: 5px;
        background: white;
        margin-top: 0px;
        position: relative;
        display: flex;
        align-items: center;
      }
      .texto_1 {
        float: left;
        font-size: 50px;
        width: 100%; 
        text-align: center;
      }  
      .texto_2 {
        padding: 10px;
        font-size: 25px;
        text-align: center;
        color: white;
        background: #1D8348;
      }
      .texto_3 {
        padding: 20px;
        font-size: 25px;
        text-align: center;
      } 
      .texto_4 {
        float: right;
        font-size: 50px;
        width: 48%; 
        text-align: center;
      }  
      .footer {
        padding: 1px;
        text-align: center;
        background: #1D8348;
        margin-top: 15px;
      }
      .row:after {
        content: "";
        display: table;
        clear: both;
      } 
      .rightcolumn_1 {
        float: left;
        width: 33.3%;
        padding-left: 20px;
      }        
      .card {
        background-color: white;
        padding: 20px;
        margin-top: 20px;
        margin-right: 20px;
        border-radius: 20px;
      }
      .fakeimg_1 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: normal; 
        font-size: 20px;
        border-radius: 20px;  
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);                   
      }  
      .fakeimg_2 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: normal; 
        font-size: 20px;
        border-radius: 20px; 
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);          
      }  
      .fakeimg_3 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: normal; 
        font-size: 20px;
        border-radius: 20px; 
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);          
      }  
      input[type=text],select,textarea {
        width: 98%;
        padding: 12px;
        border: 1px solid #ccc;
        border-radius: 10px;
        resize: vertical;
        font-size: 15px;
        max-lenght: 120;
      }
      input[type=number] {
        width: 98%;
        padding: 12px;
        border: 1px solid #ccc;
        border-radius: 10px;
        resize: vertical;
      }
      input[type=submit] {
        background-color: black;
        color: white;
        padding: 10px 10%;
        border: none;
        border-radius: 10px;
        cursor: pointer;
        margin: 8px 5px;
        margin-right: 20px;
        float: right;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
      }
      input[type=submit]:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);
      }
      .col-25 {
        float: left;
        width: 40%;
        margin-top: 5px;
      }
      .col-75 {
        float: left;
        width: 60%;
        margin-top: 5px;
      }  
      .bcol-r {
        float: right;
        width: 90%;
        margin-top: 2px;
      } 
      img {
        max-width: 100%;
        height: auto;
      }
      .espaco{
        margin: 40px 0;
      }   
      @media screen and (max-height: 1199pxpx) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }                        
      @media screen and (max-width: 1199px) {
        .logo {
          float: center;
          width: 100%;
        }
        .logo_1 {
          float: center;
          width: 100%;
        }
        .logo_2 {
          float: center;
          width: 100%;
        }
        .fakeimg_1,.fakeimg_2,.fakeimg_3{
          font-weight: normal;  
          font-size: 18px;
          height: auto;
        }
        .rightcolumn_1 {   
          width: 100%;
          padding: 0;
        }  
      }  
      @media screen and (max-width: 800px) {
        .texto_1 {
          font-size: 15px;
        }                                                             
      }               
    </style>
  </head>
  <body>
    <div id="mySidebar" class="sidebar">
      <a href="javascript:void(0)" class="closebtn" onclick="closeNav()">x</a>
      <a href="/">Setup</a>             
      <a href="/stools">Tools</a>
    </div>
    <div id="main">
      <div class="header">
        <div class="logo">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div>  
        <div class="texto_4">
          <div class="logo_2">
            <img src="eng" width="94" height="90">
          </div>  
        </div>
        <div class="logo_1">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div> 
        <div>
          <div id="mh" class="container" onclick="openNav(this)">
            <div class="bar1"></div>
            <div class="bar2"></div>
            <div class="bar3"></div>
          </div>
        </div>
      </div>  
      <div>
        <div class="texto_2">
          Network Settings
        </div>
      </div>    
      <div class="row">
        <div class="rightcolumn_1">
          <form action="/apnetcfg1.php">
            <div class="card">
              <div class="fakeimg_1">
                <div class="texto_3">            
                  Wi-Fi Settings
                </div>              
                <div class="row">
                  <div class="col-25">
                    <label for="wifiradio">Wi-Fi Radio</label>
                  </div>
                  <div class="col-75">
                    <select id="wifiradio" name="wifi_radio" onchange="wifi_show(this)">
                      <option value="radio_enable"$RADIO1_ST$>Enable</option>
                      <option value="radio_disable"$RADIO2_ST$>Disable</option>
                    </select>
                  </div> 
                </div>                        
                <div class="row" id="wifiid" $SHOW1$>                  
                    <div class="col-25">
                      <label for="wifissid">SSID</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifissid" name="wifissid_cfg" value="$WIFI_SSID$">
                    </div> 
                </div>                  
                <div class="row" id="wifipa" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifipass">Wi-Fi Password</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifipass" name="wifipass_cfg" value="$WIFI_PASS$">
                    </div>
                </div>
                <div class="row" id="wifiap" $SHOW1$>  
                    <div class="col-25">
                      <label for="acppass">AP Password</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="acppass" name="acppass_cfg" value="$ACPT_PASS$">
                    </div>
                </div>
                <div class="row" id="wifiip" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifiip">IP Address</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifiip" name="wifiip_cfg" value="$WIFI_IPAD$">
                    </div>
                </div>
                <div class="row" id="wifinm" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifinm">Network Mask</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifinm" name="wifinm_cfg" value="$WIFI_NTMA$">
                    </div>
                </div>
                <div class="row" id="wifigw" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifigw">Gateway IP Address</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifigw" name="wifigw_cfg" value="$WIFI_GATE$">
                    </div>
                </div>
                <div class="row" id="wifidn" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifidn">DNS IP Address</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifidn" name="wifidn_cfg" value="$WIFI_DNS$">
                    </div>
                </div>                                                     
              </div> 
              <div class="bcol-r">
                <input type="submit" value=" Save ">
              </div> 
              <div class="espaco">
              </div>
            </div>
          </form>
        </div>
        <div class="rightcolumn_1">
          <form action="/apnetcfg3.php">
            <div class="card">
              <div class="fakeimg_3">
                <div class="texto_3">            
                  Comunications Port
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="ethecp">UDP Port</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="ethecp" name="ethecp_cfg" value="$ETHE_CPORT$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="wificp">TCP Port</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="wificp" name="wificp_cfg" value="$WIFI_CPORT$">
                  </div>
                </div> 
                <div class="texto_3">
                  UDP FeedBack
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbip1">IP1 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbip1" name="fbip_1" value="$FDIP_1$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbpt1">Port1 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbpt1" name="fbpt_1" value="$FDPT_1$">
                  </div>
                </div>               
                <div class="row">  
                  <div class="col-25">
                    <label for="fbip2">IP2 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbip2" name="fbip_2" value="$FDIP_2$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbpt2">Port2 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbpt2" name="fbpt_2" value="$FDPT_2$">
                  </div>
                </div>                
                <div class="row">  
                  <div class="col-25">
                    <label for="fbip3">IP3 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbip3" name="fbip_3" value="$FDIP_3$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbpt3">Port3 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbpt3" name="fbpt_3" value="$FDPT_3$">
                  </div>
                </div>   
              </div> 
              <div class="bcol-r">
                <input type="submit" value=" Save ">
              </div> 
              <div class="espaco">
              </div>
            </div>             
          </form>
        </div>                
      </div> 
      <div class="footer">
        <div class="texto_2">
          Flex Control
        <div>
      </div>
    </div>
    <script>
      function wifi_show(x) {
        var selected=x.value;
        if(selected=="radio_disable") {
          document.getElementById("wifiid").style.display="none";
          document.getElementById("wifipa").style.display="none";
          document.getElementById("wifiap").style.display="none";
          document.getElementById("wifiip").style.display="none";
          document.getElementById("wifinm").style.display="none";
          document.getElementById("wifigw").style.display="none";
          document.getElementById("wifidn").style.display="none";                                                            
        } else {
          document.getElementById("wifiid").style.display="block";
          document.getElementById("wifipa").style.display="block";
          document.getElementById("wifiap").style.display="block";
          document.getElementById("wifiip").style.display="block";
          document.getElementById("wifinm").style.display="block";
          document.getElementById("wifigw").style.display="block";
          document.getElementById("wifidn").style.display="block"          
        }
      }         
      function openNav(x) {
        x.classList.toggle("change");
        if(document.getElementById("mySidebar").style.width=="250px") {
          document.getElementById("mySidebar").style.width = "0";
          document.getElementById("main").style.marginRight= "0";         
        } else {
          document.getElementById("mySidebar").style.width = "250px";
          document.getElementById("main").style.marginRight = "250px";
        }
      }
      function closeNav() {
        document.getElementById("mh").classList.toggle("change");
        document.getElementById("mySidebar").style.width = "0";
        document.getElementById("main").style.marginRight= "0";
      }
    </script>  
  </body>
</html>

)rawliteral";

//******************************************************************************************************************
// FUNCTION PROCESSOR
//******************************************************************************************************************

String index_processor(const String& var) {

  preferences.begin("memory_cfg",false);

  if(var=="SHOW1") {

    if(wifi.radio_status=="0") {
      
      return("style=\"display:none\"");         

    } else {

      return("style=\"display:block\"");
      
    }    

  } else if(var=="RADIO1_ST") {

    if(wifi.radio_status=="1") {

      return("selected");
      
    } else {

      return(" ");
      
    }

  } else if(var=="RADIO2_ST") {

    if(wifi.radio_status=="1") {

      return(" ");
      
    } else {

      return("selected");
      
    } 

  } else if(var=="WIFI_SSID") {    

    return preferences.getString("wifi_ssid",""); 

  } else if(var=="WIFI_PASS") {

    return preferences.getString("wifi_password",""); 

  } else if(var=="ACPT_PASS") {

    return preferences.getString("ap_password","");   

  } else if(var=="WIFI_IPAD") {

    return preferences.getString("wifi_ip",""); 

  } else if(var=="WIFI_NTMA") {

    return preferences.getString("wifi_nm",""); 

  } else if(var=="WIFI_GATE") {

    return preferences.getString("wifi_gtw",""); 

  } else if(var=="WIFI_DNS") {

    return preferences.getString("wifi_dns","");     

  } else if(var=="WIFI_CPORT") {

    return preferences.getString("tcp_port",""); 

  } else if(var=="ETHE_CPORT") {

    return preferences.getString("udp_port","");   

  } else if(var=="FDIP_1") {

    return(preferences.getString("fb_ip1",""));

  } else if(var=="FDIP_2") {

    return(preferences.getString("fb_ip2",""));
       
  } else if(var=="FDIP_3") {

    return(preferences.getString("fb_ip3",""));

  } else if(var=="FDPT_1") {

    return(preferences.getString("fb_pt1",""));

  } else if(var=="FDPT_2") {

    return(preferences.getString("fb_pt2",""));

  } else if(var=="FDPT_3") {
      
    return(preferences.getString("fb_pt3",""));

  }
  
  preferences.end();

  return(" ");
  
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_parse.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 02/03/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// PARSE NEW PACKAGES
//{"command":"RIP","bd_id":"0","type":0}
//{"command":"get_unique_id","bd_id":"0","type":0}

//{"command":"setStripDataPin","pin":1,"type":0}
//{"command":"setStripPixels","pixels":10,"type":0}
//{"command":"setStripColor","r":255,"g":255,"b":255,"type":0}
//{"command":"setStripBrightness","brightness":128,"type":0}
//{"command":"setStripEffect","effect":0,"type":0}
//{"command":"setStripEffectSpeed","speed":500,"type":0}
//{"command":"setClockMode","mode":0,"type":0}
//{"command":"setClockDate","date":0,"type":0}

//******************************************************************************************************************

void parse_new_package(void) {

  String retorno;
  char aux_cmd[200];
  char gc_return[50];
  char aux_rip[200];

  if(new_pkg!=read_pkg) {

    String comando=jsonExtract(pkgbuffer[read_pkg].data,"command");
          
    if(comando=="RIP") { 

      rip();

    } else if(comando=="setClockMode") {

      uint8_t mode = jsonExtract(pkgbuffer[read_pkg].data,"mode").toInt();

      setClockDisplayMode(mode);

      String aux_rip;

      aux_rip="{\"command\":\"setClockMode\",\"mode\":"+String(mode) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

    } else if(comando=="setClockDate") {

      uint8_t date = jsonExtract(pkgbuffer[read_pkg].data,"date").toInt();

      setClockDateCycle(date);
      String aux_rip;

      aux_rip="{\"command\":\"setClockDate\",\"date\":"+String(date) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

    } else if(comando=="get_unique_id") { 

      envia_firmware();  

    } else if(comando=="setStripColor") {

      uint8_t r = jsonExtract(pkgbuffer[read_pkg].data,"r").toInt();
      uint8_t g = jsonExtract(pkgbuffer[read_pkg].data,"g").toInt();
      uint8_t b = jsonExtract(pkgbuffer[read_pkg].data,"b").toInt();

      color_r=r;
      color_g=g;
      color_b=b;

      String aux_rip;

      aux_rip="{\"command\":\"setStripColor\",\"r\":" + String(r) + ",\"g\":" + String(g) + ",\"b\":" + String(b) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

      preferences.begin("memory_cfg",false); 

      preferences.putString("cr",String(r));
      preferences.putString("cg",String(g));
      preferences.putString("cb",String(b));

      preferences.end();

      setStripColor(r,g,b);

    } else if(comando=="setStripBrightness") {

      uint8_t brightness = jsonExtract(pkgbuffer[read_pkg].data,"brightness").toInt();

      stripBrightness=brightness;

      String aux_rip;

      aux_rip="{\"command\":\"setStripBrightness\",\"brightness\":"+String(brightness) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

      preferences.begin("memory_cfg",false); 

      preferences.putString("brigh",String(brightness));

      preferences.end();

      setStripBrightness(brightness);

    } else if(comando=="setStripPixels") {

      uint16_t pixels=jsonExtract(pkgbuffer[read_pkg].data,"pixels").toInt();

      stripPixels=pixels;

      String aux_rip;

      aux_rip="{\"command\":\"setStripPixels\",\"pixels\":"+String(pixels) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

      preferences.begin("memory_cfg",false); 

      preferences.putString("n_leds",String(pixels));

      preferences.end();

      delay(1000);

      ESP.restart();

      //set_strip_pixels(pixels);

    } else if(comando=="setStripEffect") {

      efeito=jsonExtract(pkgbuffer[read_pkg].data,"effect").toInt();

      if(efeito>TOTAL_EFEITOS) efeito=TOTAL_EFEITOS-1;

      String aux_rip;

      aux_rip="{\"command\":\"setStripEffect\",\"effect\":"+String(efeito) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

      preferences.begin("memory_cfg",false); 

      preferences.putString("efeito",String(efeito));

      preferences.end();

    } else if(comando=="setStripEffectSpeed") {
      
      speed=jsonExtract(pkgbuffer[read_pkg].data,"speed").toInt();

      String aux_rip;

      aux_rip="{\"command\":\"setStripEffectSpeed\",\"speed\":"+String(speed) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 
  
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

      preferences.begin("memory_cfg",false); 

      preferences.putString("speed",String(speed));

      preferences.end();

    } else if(comando=="setStripDataPin") {

      dataPin=jsonExtract(pkgbuffer[read_pkg].data,"pin").toInt();

      String aux_rip;

      aux_rip="{\"command\":\"setStripDataPin\",\"pin\":"+String(dataPin) + ",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") { 

        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);

      } else {

        String rc(aux_rip);
        char aux_aux[300];

        rc.toCharArray(aux_aux,rc.length()+1);  

        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();

      }

      preferences.begin("memory_cfg",false); 

      preferences.putString("dataPin",String(dataPin));

      preferences.end();

      delay(1000);

      ESP.restart();

    }

    read_pkg++;
    if(read_pkg>PKG_BUFFER_SIZE-2) read_pkg=0; 
      
  }
  
  return;
  
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_reset_page.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// RESET.HTML PAGE
//******************************************************************************************************************
const char reset_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html>
  <head>
    <title>DBX-Mio1012</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      * {
        box-sizing: border-box;
      }
      body {
        font-family: "Lato", sans-serif;
        background: #f1f1f1;        
      }
      .container {
        display: inline-block;
        cursor: pointer;
        margin: 20px 20px;
      }
      .bar1, .bar2, .bar3 {
        width: 35px;
        height: 5px;
        background-color: #333;
        margin: 6px 0;
        transition: 0.4s;
      }
      .change .bar1 {
        -webkit-transform: rotate(-45deg) translate(-9px, 6px);
        transform: rotate(-45deg) translate(-9px, 6px);
      }
      .change .bar2 {opacity: 0;}
      .change .bar3 {
        -webkit-transform: rotate(45deg) translate(-8px, -8px);
        transform: rotate(45deg) translate(-8px, -8px);
      }      
      .sidebar {
        height: 100%;
        width: 0;
        position: fixed;
        z-index: 1;
        top: 0;
        right: 0;
        background-color: #1D8348;
        overflow-x: hidden;
        transition: 0.5s;
        padding-top: 60px;
      }
      .sidebar a {
        padding: 8px 8px 8px 32px;
        text-decoration: none;
        font-size: 21px;
        color: white;
        display: block;
        transition: 0.3s;
      }
      .sidebar a:hover {
        color: black;
      }
      .sidebar .closebtn {
        position: absolute;
        top: 0;
        right: 25px;
        font-size: 36px;
        margin-left: 50px;
      }
      #main {
        transition: margin-right .5s;
        padding: 16px;
      }      
      .logo {
        padding: 13px;
        float: left;
        width: 78%;     
        text-align: left;           
      }
      .logo_1 {
        padding: 13px;
        float: right;
        width: 90%;     
        text-align: right;           
      }      
      .header {
        padding: 5px;
        background: white;
        margin-top: 0px;
        position: relative;
        display: flex;
        align-items: center;
      }
      .texto_1 {
        float: left;
        font-size: 50px;
        width: 100%; 
        text-align: center;
      }  
      .texto_2 {
        padding: 10px;
        font-size: 25px;
        text-align: center;
        color: white;
        background: #1D8348;
      }
      .texto_3 {
        padding: 20px;
        font-size: 25px;
        text-align: center;
      }                   
      .footer {
        padding: 1px;
        text-align: center;
        background: #1D8348;
        margin-top: 15px;
      }
      .row:after {
        content: "";
        display: table;
        clear: both;
      }  
      .leftcolumn {
        text-align: center;   
        float: left;
        width: 100%;
      }  
      .bcol-r {
        text-align: center;
        float: center;
        margin-top: 2px;
        font-size: 20px;
      }          
      input[type=submit] {
        background-color: black;
        color: white;
        font-size: 25px;
        padding: 10px 30px;
        border: none;
        border-radius: 8px;
        cursor: pointer;
        margin: 8px 5px;
        float: center;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
      } 
      input[type=submit]:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }
      img {
        max-width: 100%;
        height: auto;
      } 
      @media screen and (max-height: 1200pxpx) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }       
      @media screen and (max-width: 800px) {
        .bcol-r {
          width: 100%;
          height: auto;
          text-align: center;
          float: center;
        }
      }        
      @media screen and (max-width: 800px) {
        .leftcolumn {   
          width: 100%;
          padding: 0;
        }
      }                    
      @media screen and (max-height: 800px) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }
      @media screen and (max-width: 800px) {
        .texto_1 {
          font-size: 15px;
        }
      }      
      @media screen and (max-width: 800px) {
        .logo {
          float: center;
          width: 100%;
        }
        .logo_1 {
          float: center;
          width: 100%;
        }
      }                                                      
    </style>
  </head>
  <body>
    <div id="mySidebar" class="sidebar">
      <a href="javascript:void(0)" class="closebtn" onclick="closeNav()">x</a>
      <a href="/">Status</a>           
      <a href="/stools">Tools</a>
    </div>
    <div id="main">
      <div class="header">
        <div class="logo">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div>  
        <div class="texto_1">
          DBX-Mio1012
        </div>
        <div class="logo_1">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div> 
        <div>
          <div id="mh" class="container" onclick="openNav(this)">
            <div class="bar1"></div>
            <div class="bar2"></div>
            <div class="bar3"></div>
          </div>
        </div>
      </div>  
      <div>
        <div class="texto_2">
          Reset $ACCO_LOC$
        </div>
      </div> 
      <div class="row">  
        <div class="leftcolumn"> 
          <form action="/apreset.php">
            <h2>It is necessary to reset the controller for the changes to take effect !!!</h2>
            <h2>This operation takes approximately 30 seconds, please press the reset button and wait...</h2>
            <div class="bcol-r">
              <input type="submit" value="Reset">
            </div> 
          </form>
        </div>                 
      </div>
      <div class="footer">
        <div class="texto_2">
          Flex Control
        <div>
      </div>
    </div>
    <script>   
      function openNav(x) {
        x.classList.toggle("change");
        if(document.getElementById("mySidebar").style.width=="250px") {
          document.getElementById("mySidebar").style.width = "0";
          document.getElementById("main").style.marginRight= "0";         
        } else {
          document.getElementById("mySidebar").style.width = "250px";
          document.getElementById("main").style.marginRight = "250px";
        }
      }
      function closeNav() {
        document.getElementById("mh").classList.toggle("change");
        document.getElementById("mySidebar").style.width = "0";
        document.getElementById("main").style.marginRight= "0";
      }
    </script>   
  </body>
</html>
)rawliteral";

//******************************************************************************************************************
// FUNCTION PROCESSOR_UPDATE
//******************************************************************************************************************

String reset_processor(const String& var) {

  return "";
  
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_stools_page.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// INDEX.HTML PAGE
//******************************************************************************************************************
const char stools_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html>
  <head>
    <title>DBX-Digital Led</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      * {
        box-sizing: border-box;
      }
      body {
        font-family: "Lato", sans-serif;
        background: #f1f1f1;        
      }
      .container {
        display: inline-block;
        cursor: pointer;
        margin: 20px 20px;
      }
      .bar1, .bar2, .bar3 {
        width: 35px;
        height: 5px;
        background-color: #333;
        margin: 6px 0;
        transition: 0.4s;
      }
      .change .bar1 {
        -webkit-transform: rotate(-45deg) translate(-9px, 6px);
        transform: rotate(-45deg) translate(-9px, 6px);
      }
      .change .bar2 {opacity: 0;}
      .change .bar3 {
        -webkit-transform: rotate(45deg) translate(-8px, -8px);
        transform: rotate(45deg) translate(-8px, -8px);
      }      
      .sidebar {
        height: 100%;
        width: 0;
        position: fixed;
        z-index: 1;
        top: 0;
        right: 0;
        background-color: #1D8348;
        overflow-x: hidden;
        transition: 0.5s;
        padding-top: 60px;
      }
      .sidebar a {
        padding: 8px 8px 8px 32px;
        text-decoration: none;
        font-size: 21px;
        color: white;
        display: block;
        transition: 0.3s;
      }
      .sidebar a:hover {
        color: black;
      }
      .sidebar .closebtn {
        position: absolute;
        top: 0;
        right: 25px;
        font-size: 36px;
        margin-left: 50px;
      }
      #main {
        transition: margin-right .5s;
        padding: 16px;
      }      
      .logo {
        padding: 13px;
        float: left;
        width: 78%;     
        text-align: left;           
      }
      .logo_1 {
        padding: 13px;
        float: right;
        width: 90%;     
        text-align: right;           
      }
      .logo_2 {
        padding: 4px;
        float: center;
        width: 100%;     
        text-align: center;           
      } 
      .header {
        padding: 5px;
        background: white;
        margin-top: 0px;
        position: relative;
        display: flex;
        align-items: center;
      }
      .texto_1 {
        float: left;
        font-size: 50px;
        width: 100%; 
        text-align: center;
      }  
      .texto_2 {
        padding: 10px;
        font-size: 25px;
        text-align: center;
        color: white;
        background: #1D8348;
      }
      .texto_3 {
        padding: 20px;
        font-size: 25px;
        text-align: center;
      } 
      .texto_4 {
        float: right;
        font-size: 50px;
        width: 48%; 
        text-align: center;
      }
      .texto_5 {
        color: white;
        font-size: 20px;
        text-align: center;
      }       
      .footer {
        padding: 1px;
        text-align: center;
        background: #1D8348;
        margin-top: 15px;
      }
      .row:after {
        content: "";
        display: table;
        clear: both;
      } 
      .rightcolumn_1 {
        float: left;
        width: 20.0%;
        padding-left: 20px;
      }         
      .card {
        background-color: white;
        padding: 20px;
        margin-top: 20px;
        margin-right: 20px;
        border-radius: 20px;
      }
      .fakeimg_1 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: bold;
        font-size: 15px;
        border-radius: 20px;  
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);                     
      } 
      progress {
        float: center;
        border-radius: 7px; 
        width: 80%;
        height: 22px;
        box-shadow: 1px 1px 4px rgba( 0, 0, 0, 0.2 );
      }
      progress::-webkit-progress-bar {
        background-color: white;
        border-radius: 7px;
      }
      progress::-webkit-progress-value {
        background-color: red;
        border-radius: 7px;
      }
      progress::-moz-progress-bar {
        /* style rules */
      }          
      label {
        float: left;
        text-align: center;  
        background-color: black;
        color: white;
        font-size: 20px;
        padding: 14px 20px;
        margin: 8px 0;
        border: none;
        cursor: pointer;
        width: 100%;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      } 
      label:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }  
      input[type=submit] {
        float: left;
        text-align: center;  
        background-color: black;
        color: white;
        font-size: 20px;
        padding: 14px 20px;
        margin: 8px 0;
        border: none;
        cursor: pointer;
        width: 100%;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      } 
      input[type=submit]:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }        
      input[type=file] {
        display: none;
      } 
      .container_1 {
        padding: 16px;
        text-align: center;
      }
      .modal {
        display: none;
        position: fixed;
        z-index: 1; 
        left: 30%;
        top: 30%;
        width: 40%; 
        height: auto;
        overflow: auto;
        background-color: #474e5d;
        padding-top: 40px;
        padding-bottom: 40px;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      }
      .modal-content {
        background-color: #fefefe;
        margin: 1% auto 1% auto; 
        border: 1px solid #888;
        width: 80%; 
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      } 
      hr {
        border: 1px solid #f1f1f1;
        margin-bottom: 25px;
      }
      .close {
        position: absolute;
        right: 35px;
        top: 15px;
        font-size: 40px;
        font-weight: bold;
        color: #f1f1f1;
      }
      .close:hover,.close:focus {
        color: #f44336;
        cursor: pointer;
      }
      .clearfix::after {
        content: "";
        clear: both;
        display: table;
      }
      .container_1 {
        padding: 16px;
        text-align: center;
      }      
      button {
        background-color: black;
        color: white;
        font-size: 20px;
        padding: 14px 20px;
        margin: 8px 0;
        border: none;
        cursor: pointer;
        width: 100%;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      }
      button:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }
      .cancelbtn, .deletebtn {
        float: center;
        width: 40%;
      }
      .cancelbtn {
        background-color: #ccc;
        color: black;
      }
      .deletebtn {
        background-color: #f44336;
      }         
      img {
        max-width: 100%;
        height: auto;
      }
      .espaco1{
        margin: 20px 0;
      }        
      .espaco2{
        margin: 30px 0;
      }      
      @media screen and (max-height: 1200pxpx) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }      
      @media screen and (max-width: 300px) {
        .cancelbtn, .deletebtn {
          width: 100%;
        }
      }                  
      @media screen and (max-width: 1199px) {
        .logo {
          float: center;
          width: 100%;
        }
        .logo_1 {
          float: center;
          width: 100%;
        }
        .logo_2 {
          float: center;
          width: 100%;
        }
        .fakeimg_1 {
          font-weight: normal;  
          font-size: 12px;
          height: auto;
        }
        .rightcolumn_1 {   
          width: 100%;
          padding: 0;
        }  
        .container {
          margin: 5px 5px;                     
        }       
      }
      @media screen and (max-width: 800px) {
        .texto_1 {
          font-size: 15px;
        }                                                           
      }
    </style>
  </head>
  <body>
    <div id="mySidebar" class="sidebar">
      <a href="javascript:void(0)" class="closebtn" onclick="closeNav()">x</a>
      <a href="/">Setup</a>
      <a href="/stools">Tools</a>
    </div>
    <div id="main">
      <div class="header">
        <div class="logo">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div>  
        <div class="texto_4">
          <div class="logo_2">
            <img src="eng" width="94" height="90">
          </div>  
        </div>
        <div class="logo_1">
          <a href="/stools">
          <img src="logo" width="213" height="90">
          </a>
        </div> 
        <div>
          <div id="mh" class="container" onclick="openNav(this)">
            <div class="bar1"></div>
            <div class="bar2"></div>
            <div class="bar3"></div>
          </div>
        </div>
      </div>  
      <div>
        <div class="texto_2">
          System Tools $ACCO_LOC$
        </div>
      </div> 
      <div class="row">  
        <div class="rightcolumn_1">
          <div class="card">
            <div class="texto_3">            
              Update Firmware
            </div>
            <div class="fakeimg_1">
              <div class="row">
                <form method='POST' action='/doUpdate' enctype='multipart/form-data'>
                  <label for='input_1'>Choose File</label>              
                  <input type="file" id="input_1" name="update">
                  <div id="id01" class="modal">
                    <span onclick="document.getElementById('id01').style.display='none'" class="close" title="Close Modal">X</span>
                    <div class="container_1">
                      <div class="modal-content">
                        <h1>Update Firmware</h1>
                        <p>Are you sure you want to update the firmware ?</p>
                        <p>This operation takes approximately 1 minute and cannot be stopped !</p> 
                        <p>Please keep this window open until the Gateway LED stops flashing</p>                      
                        <div class="clearfix">
                          <button type="button" name="cancel1_cfg" onclick="document.getElementById('id01').style.display='none'" class="cancelbtn">No</button>
                          <button type="submit" id="bupdate" class="deletebtn" >Yes</button>                                               
                        </div>                        
                      </div> 
                      <div class="espaco1" id="progress1" $SHOWA$>
                      </div> 
                      <div class="row" id="progress3" $SHOWA$> 
                        <div class="texto_5">
                          <span id="prgval"></span>                                                      
                        </div>                      
                      </div> 
                      <div class="espaco2" id="progress1" $SHOWA$>
                      </div>                                              
                      <div class="row" id="progress2" $SHOWA$> 
                        <progress id="progress" value="0" max="100"></progress>
                      </div>
                      <div class="espaco1" id="progress1" $SHOWA$>
                      </div>                      
                      <div class="texto_5">
                        <span id="prgval1"></span>                                                      
                      </div>                                                                      
                    </div>                  
                  </div>
                </form>                
              </div>             
            </div>           
          </div>
        </div>
        <div class="rightcolumn_1">
          <div class="card">
            <div class="texto_3">            
              Reboot System
            </div>
            <div class="fakeimg_1">
              <div class="row">
                <form action="/apreset.php">
                  <label onclick="document.getElementById('id02').style.display='block'">Reboot</label>                   
                  <div id="id02" class="modal">
                    <span onclick="document.getElementById('id02').style.display='none'" class="close" title="Close Modal">X</span>               
                    <div class="container_1">
                      <div class="modal-content">
                        <h1>Reboot System</h1>
                        <p>Are you sure you want to Reboot System ?</p> 
                        <p>This operation takes approximately 30 seconds and cannot be stopped !</p> 
                        <p>Please wait...</p>                      
                        <div class="clearfix">
                          <button type="button" name="cancel2_cfg" onclick="document.getElementById('id02').style.display='none'" class="cancelbtn">No</button>
                          <button type="submit" id="bupdate" class="deletebtn" >Yes</button>                                               
                        </div>
                      </div>                   
                    </div>
                  </div>
                </form>           
              </div>
            </div>
          </div>
        </div>             
      </div>
      <div class="footer">
        <div class="texto_2">
          Flex Control
        <div>
      </div>
    </div>
    <script> 
      var gateway=`ws://${window.location.hostname}/ws`;
      var websocket;
      window.addEventListener('load',onload);
      function onload(event) {
        initWebSocket();
      }
      function getReadings(){
      }      
      function initWebSocket() {
        websocket=new WebSocket(gateway);
        websocket.onopen=onOpen;
        websocket.onclose=onClose;
        websocket.onmessage=onMessage;
      }
      function onOpen(event) {
      }
      function onClose(event) {
        setTimeout(initWebSocket,2000);
      }
      function onMessage(event) {
        var myObj=JSON.parse(event.data);
        var keys=Object.keys(myObj); 
        const progressElement=document.getElementById("progress");    
        if(keys[0]=="pgval") {
          document.getElementById("progress1").style.display="block"; 
          document.getElementById("progress2").style.display="block"; 
          document.getElementById("progress3").style.display="block";                   
          var key=keys[0];
          progressElement.value+=3;
          document.getElementById("prgval").innerHTML=progressElement.value+" %";
          if(progressElement.value>=99) document.getElementById("prgval1").innerHTML="Wait until device reboot...";
        }                     
      }                     
      const fileInput=document.getElementById('input_1'); 
      var modal_1=document.getElementById('id01');
      var modal_2=document.getElementById('id02');
      var modal_3=document.getElementById('id03');
      var modal_4=document.getElementById('id04');
      var modal_5=document.getElementById('id05');      
      var x=document.getElementById('bupdate');
      const handleFiles =()=>{        
        var selectedFiles=fileInput.files[0];
        if(selectedFiles.name!="") {
          modal_1.style.display="block";   
          x.style.background="#f44336";
          x.disabled=false;     
        }
      }            
      fileInput.addEventListener("change",handleFiles); 
      window.onbeforeunload=function () {
        x.style.background="black";
        x.disabled=true;        
      }         
      window.onclick=function(event) {
        if(event.target==modal_1) {
          modal_1.style.display="none";
        } else if (event.target==modal_2) {
          modal_2.style.display="none";
        } else if (event.target==modal_3) {
          modal_3.style.display="none"; 
        } else if (event.target==modal_4) {
          modal_4.style.display="none"; 
        } else if (event.target==modal_5) {
          modal_5.style.display="none";          
        }        
      }
      function st_bt1(x) {
        let newloc="/log";
        window.location.assign(newloc);       
      }
      function st_bt2(x) {
        let newloc="/stemplog";
        window.location.assign(newloc);       
      }      
      function openNav(x) {
        x.classList.toggle("change");
        if(document.getElementById("mySidebar").style.width=="250px") {
          document.getElementById("mySidebar").style.width = "0";
          document.getElementById("main").style.marginRight= "0";         
        } else {
          document.getElementById("mySidebar").style.width = "250px";
          document.getElementById("main").style.marginRight = "250px";
        }
      }
      function closeNav() {
        document.getElementById("mh").classList.toggle("change");
        document.getElementById("mySidebar").style.width="0";
        document.getElementById("main").style.marginRight="0";
      }
    </script>  
  </body>
</html>
)rawliteral";

//******************************************************************************************************************
// FUNCTION PROCESSOR_UPDATE
//******************************************************************************************************************

String update_processor(const String& var) {
 
  return("");
  
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_tcp.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 03/02/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// HANDLE DISCONNECT
//******************************************************************************************************************

static void handleDisconnect(void* arg, AsyncClient* client) {

  Serial.println("\n\r---------------------------------------------------------------------------------");  
  Serial.printf("Client %s disconnected : \n",client->remoteIP().toString().c_str());
  Serial.println("\n\r---------------------------------------------------------------------------------"); 
  
}

//******************************************************************************************************************
// HANDLE GC DATA
//******************************************************************************************************************

static void handleData(void* arg,AsyncClient* client,void *data,size_t len) {

  Serial.println("\n\r---------------------------------------------------------------------------------");  
  Serial.println("TCP Packet");     
  Serial.print  ("Remote IP     : ");
  Serial.println(client->remoteIP().toString().c_str());
  Serial.print  ("Remote Port   : ");
  Serial.println(client->remotePort());
  Serial.print  ("Local IP      : ");
  Serial.println(client->localIP().toString().c_str());
  Serial.print  ("Local Port    : ");
  Serial.println(client->localPort());
  Serial.print  ("Length        : ");
  Serial.println(len);
  Serial.print  ("New Package   : ");
  Serial.println(new_pkg);
  Serial.print  ("Read Package  : ");
  Serial.println(read_pkg);
  Serial.print  ("Data          : ");
  Serial.write((char*)data,len);
  Serial.println();
  Serial.println("---------------------------------------------------------------------------------");

  char* dataStr = (char*)data;
  char tempBuffer[len + 1];
  memcpy(tempBuffer, dataStr, len);
  tempBuffer[len] = '\0';
  
  // Verifica se contÃ©m "type:0"
  if (strstr(tempBuffer, "type:0") == NULL) {

    Serial.println("Comando rejeitado - nao contem type:0");
    Serial.println("---------------------------------------------------------------------------------");
    return;
  
  }
  
  Serial.println("Comando aceito - contem type:0");
  Serial.println("---------------------------------------------------------------------------------");

  pkgbuffer[new_pkg].data="";

  pkgbuffer[new_pkg].type="TCP";
  pkgbuffer[new_pkg].data=(char*)data;  
  pkgbuffer[new_pkg].data_len=len;
  pkgbuffer[new_pkg].local_ip=client->localIP();
  pkgbuffer[new_pkg].local_port=client->localPort(); 
  pkgbuffer[new_pkg].remote_ip=client->remoteIP();
  pkgbuffer[new_pkg].remote_port=client->remotePort();   
  pkgbuffer[new_pkg].client=client;

  new_pkg++;
  if(new_pkg>PKG_BUFFER_SIZE-2) new_pkg=0;

  return;

}

//******************************************************************************************************************
// HANDLE ERROR GC CONNECTION
//******************************************************************************************************************
 
static void handleError(void* arg, AsyncClient* client, int8_t error) {

  Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());

}

//******************************************************************************************************************
// SERVER EVENTS
//******************************************************************************************************************

static void handleNewClient(void* arg,AsyncClient* client) {

  Serial.println("\n\r---------------------------------------------------------------------------------");     
  Serial.printf("New client has been connected to server, ip: %s",client->remoteIP().toString().c_str());
  Serial.println("\n\r---------------------------------------------------------------------------------");    

  clients.push_back(client);  
  
  client->onData(&handleData,NULL);
  client->onError(&handleError, NULL);
  client->onDisconnect(&handleDisconnect,NULL);
  client->onTimeout(&handleTimeOut, NULL);
  
}

//******************************************************************************************************************
// HANDLE TIMEOUT
//******************************************************************************************************************

static void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) {

  Serial.println("\n\r---------------------------------------------------------------------------------");    
  Serial.printf("Client ACK timeout ip: %s \n",client->remoteIP().toString().c_str());
  Serial.println("\n\r---------------------------------------------------------------------------------");  
  
}

//******************************************************************************************************************
// CONFIG AND START TCP SERVER
//******************************************************************************************************************

void set_tcp_server(void) {

  AsyncServer* tcp_server=new AsyncServer(port.tcp.toInt()); 
  tcp_server->onClient(&handleNewClient,tcp_server);
  tcp_server->begin();
    
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_udp.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 03/02/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// UDP SERVER
//******************************************************************************************************************

void set_udp_server(void) {

  if(udp.listen(port.udp.toInt())) {

    Serial.print("UDP Listening on IP: ");
    
    Serial.println(WiFi.localIP());
    
    udp.onPacket([](AsyncUDPPacket packet) {

      Serial.println("\n\r---------------------------------------------------------------------------------");  
      Serial.println("UDP Packet");     
      Serial.print("UDP Packet Type : ");
      Serial.println(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
      Serial.print("Remote IP       : ");
      Serial.println(packet.remoteIP());
      Serial.print("Remote Port     : ");
      Serial.println(packet.remotePort());
      Serial.print("Local IP        : ");
      Serial.println(packet.localIP());
      Serial.print("Local Port      : ");
      Serial.println(packet.localPort());
      Serial.print("Length          : ");
      Serial.println(packet.length());
      Serial.print("New Package     : ");
      Serial.println(new_pkg);
      Serial.print("Read Package    : ");
      Serial.println(read_pkg);
      Serial.print("Data            : ");
      Serial.write(packet.data(),packet.length());
      Serial.println();
      Serial.println("---------------------------------------------------------------------------------");

      // Filtro por type -  processa se type for 0
      String receivedData = String((char*)packet.data(), packet.length());
      String typeValue = jsonExtract(receivedData, "type");
        
      if(typeValue != "0" && typeValue != "") {
      
        Serial.println("Comando rejeitado - nao contem type:0");
        Serial.println("---------------------------------------------------------------------------------");      
        return; // Ignora o pacote se type nao for 0
      
      }

      Serial.println("Comando aceito - contem type:0");
      Serial.println("---------------------------------------------------------------------------------");

      pkgbuffer[new_pkg].data="";

      pkgbuffer[new_pkg].type="UDP";
      pkgbuffer[new_pkg].data=(char*)packet.data();
      pkgbuffer[new_pkg].data_len=packet.length();
      pkgbuffer[new_pkg].local_ip=packet.localIP();
      pkgbuffer[new_pkg].local_port=packet.localPort(); 
      pkgbuffer[new_pkg].remote_ip=packet.remoteIP();
      pkgbuffer[new_pkg].remote_port=packet.remotePort();  
      pkgbuffer[new_pkg].client=NULL;   

      new_pkg++;
      if(new_pkg>PKG_BUFFER_SIZE-2) new_pkg=0;
        
    });  

  }

  return;

}

//******************************************************************************************************************
// UDP SEND MESSAGE
//******************************************************************************************************************

void send_message_udp(String msg,IPAddress addr,uint16_t port) {

  char aux[msg.length()+1];
          
  msg.toCharArray(aux,msg.length()+1);
        
  udp.writeTo((uint8_t *)aux,msg.length(),addr,port); 
        
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_utils.ino"
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
    // Garante strip suficiente para todos os dígitos e dois pontos (4x7 + 2 = 30 LEDs)
    if (!stripChecked) {

      long required = 30;
      
      if (stripPixels < required) {
      
        set_strip_pixels(required);
      
      }
      
      stripChecked = true;
    
    }

    // Mantém dígitos de unidades/dezenas e dois pontos apagados durante update
    for (uint8_t seg = 0; seg < 7; seg++) {
    
      if (stripPixels > (0 + 7 + seg)) leds[0 + 7 + seg] = CRGB::Black;     // dígito 1 (dezenas de minutos)
      // NÃO apagar dígitos 2 e 3 aqui; letras F/U são estáticas nesses dígitos
    }
    
    if (stripPixels > (0 + 14)) leds[0 + 14] = CRGB::Black;                 // dois pontos
    if (stripPixels > (0 + 15)) leds[0 + 15] = CRGB::Black;

    // Adiciona letras durante update: D3 (milhares) = 'F', D2 (centenas) = 'U'
    // Mapeamento de segmentos: 0=SupDir,1=Topo,2=SupEsq,3=Meio,4=InfDir,5=Base,6=InfEsq
    // 'F' = Topo, SupEsq, Meio, InfEsq
    // 'U' = SupDir, SupEsq, InfEsq, Base, InfDir (inclui segmento 0)
    const uint16_t startThousands = 0 + 23;
    const uint16_t startHundreds  = 0 + 16;
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
    if (millis() - lastAnimUpdate >= (unsigned long)(speed * 50)) { // ~500ms com speed=10
      // Apaga todos os segmentos do primeiro dígito (apenas dígito 0)
      for (uint8_t s = 0; s < 7; s++) {
        leds[0 + s] = CRGB::Black;
      }
      // Acende segmento atual em vermelho
      leds[0 + animOrder[animIdx]] = CRGB::Red;

      animIdx = (animIdx + 1) % 6;
      lastAnimUpdate = millis();
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

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_webserver.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// FUNCTION SHOW PROGRESS
//******************************************************************************************************************

void printProgress(size_t prg, size_t sz) {

  DynamicJsonDocument doc(50);
  char messagePayload[50];

  uint8_t aux_aux=0;

  aux_aux=(prg*100)/content_len;

  Serial.printf("Progress: %d%%\n",aux_aux);

  if(aux_aux!=last_firmware&&aux_aux>=last_firmware+2) {

    doc["pgval"]=aux_aux;

    serializeJson(doc,messagePayload);
    notifyClients(messagePayload);

    last_firmware=aux_aux;

  }

}


//******************************************************************************************************************
// FUNCTION UPDATE FIRMWARE
//******************************************************************************************************************

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  
  if(!index) {
    
    Serial.println("Update");

    update_status=true;
    update_interval_ms=millis();
    
    content_len=request->contentLength();
        
    int cmd=(filename.indexOf("spiffs")>-1) ? U_PART : U_FLASH;
    
    if (!Update.begin(UPDATE_SIZE_UNKNOWN,cmd)) {
      
      Update.printError(Serial);
    
    }
    
  }

  if(Update.write(data, len)!=len) {
  
    Update.printError(Serial);
  
  }

  if(final) {
    
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
    
    response->addHeader("Refresh", "20");  
    response->addHeader("Location", "/");
    
    request->send(response);
    
    if(!Update.end(true)){
    
      Update.printError(Serial);
    
    } else {
  
      Serial.println("Update complete");
      Serial.flush();  
      digitalWrite(LED2,LOW);    

      delay(1000);
      
      ESP.restart();
    
    }
    
  }
  
}

//******************************************************************************************************************
// FUNCTION CONFIG WEB SERVER
//******************************************************************************************************************

void webserver_start(void) {

  if(init_web_server) {

    Serial.println("---------------------------------------------------------------------------------");
    Serial.println("Web Server Starting...");
    Serial.println("---------------------------------------------------------------------------------");

    init_web_server=0;

    //******************************************************************************************************************
    // PAGINAS HTML
    //******************************************************************************************************************

    server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    
      if(!request->authenticate(http_username,http_password)) return request->requestAuthentication();
      request->send_P(200,"text/html",index_html,index_processor);
  
    });  

    server.on("/stools",HTTP_GET,[](AsyncWebServerRequest *request){
    
      if(!request->authenticate(http_username,http_password)) return request->requestAuthentication();
      request->send_P(200,"text/html",stools_html,update_processor);
  
    });

    server.on("/reset",HTTP_GET,[](AsyncWebServerRequest *request) {
     
      if(!request->authenticate(http_username,http_password)) return request->requestAuthentication();
      request->send_P(200,"text/html",reset_html,reset_processor);
  
    });        
    
    //******************************************************************************************************************
    // POST FUNCTIONS
    //******************************************************************************************************************

    server.on("/doUpdate",HTTP_POST,[](AsyncWebServerRequest *request) {},
    
      [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
      size_t len, bool final) {handleDoUpdate(request, filename, index, data, len, final);}
                  
    );

    //******************************************************************************************************************
    // IMAGENS
    //******************************************************************************************************************

    server.on("/logo",HTTP_GET,[](AsyncWebServerRequest *request){
    
      request->send(SPIFFS,"/logo.png","image/png");
    
    }); 

    server.on("/eng",HTTP_GET,[](AsyncWebServerRequest *request){
    
      request->send(SPIFFS,"/eng.png","image/png");
    
    }); 
     
    //******************************************************************************************************************
    // ACTIONS PAGES
    //******************************************************************************************************************

    server.on("/apnetcfg1.php",HTTP_GET,[](AsyncWebServerRequest *request) {

      String inputMessage;

      preferences.begin("memory_cfg",false);

      if(request->hasParam("wifi_radio")) {

        inputMessage=request->getParam("wifi_radio")->value();

        if(inputMessage=="radio_enable") {

          preferences.putString("radio_status","1");
          wifi.radio_status="1";
          
        } else {

          preferences.putString("radio_status","0");
          wifi.radio_status="0";
          
        }        
             
      } 

      if(request->hasParam("wifissid_cfg")) {

        inputMessage=request->getParam("wifissid_cfg")->value();

        preferences.putString("wifi_ssid",inputMessage.c_str());
        wifi.ssid=preferences.getString("wifi_ssid","");
          
      } 

      if(request->hasParam("wifipass_cfg")) {

        inputMessage=request->getParam("wifipass_cfg")->value();

        preferences.putString("wifi_password",inputMessage.c_str());
        wifi.password=preferences.getString("wifi_password",""); 
          
      }   

      if(request->hasParam("acppass_cfg")) {

        inputMessage=request->getParam("acppass_cfg")->value();

        preferences.putString("ap_password",inputMessage.c_str());
        wifi.ap_password=preferences.getString("ap_password",""); 

      }  

      if(request->hasParam("wifiip_cfg")) {

        inputMessage=request->getParam("wifiip_cfg")->value();

        preferences.putString("wifi_ip",inputMessage.c_str());
        wifi.ip=preferences.getString("wifi_ip",""); 

      }  

      if(request->hasParam("wifinm_cfg")) {

        inputMessage=request->getParam("wifinm_cfg")->value();

        preferences.putString("wifi_nm",inputMessage.c_str());
        wifi.netmask=preferences.getString("wifi_nm",""); 

      } 

      if(request->hasParam("wifigw_cfg")) {

        inputMessage=request->getParam("wifigw_cfg")->value();

        preferences.putString("wifi_gtw",inputMessage.c_str());
        wifi.gtw=preferences.getString("wifi_gtw",""); 

      } 

      if(request->hasParam("wifidn_cfg")) {

        inputMessage=request->getParam("wifidn_cfg")->value();

        preferences.putString("wifi_dns",inputMessage.c_str());
        wifi.dns=preferences.getString("wifi_dns",""); 

      }         

      preferences.end();

      request->redirect("/reset");
      
    });  

    server.on("/apnetcfg3.php",HTTP_GET,[](AsyncWebServerRequest *request) {

      String inputMessage;

      preferences.begin("memory_cfg",false);      

      if(request->hasParam("ethecp_cfg")) {

        inputMessage=request->getParam("ethecp_cfg")->value();

        preferences.putString("udp_port",inputMessage.c_str());
        port.udp=preferences.getString("udp_port",""); 

      }  

      if(request->hasParam("wificp_cfg")) {

        inputMessage=request->getParam("wificp_cfg")->value();

        preferences.putString("tcp_port",inputMessage.c_str());
        port.tcp=preferences.getString("tcp_port",""); 

      }   


      if(request->hasParam("fbip_1")) {

        inputMessage=request->getParam("fbip_1")->value();
        
        preferences.putString("fb_ip1",inputMessage.c_str());
        fb_ip_list[0].fromString(preferences.getString("fb_ip1",""));

      }

      if(request->hasParam("fbip_2")) {

        inputMessage=request->getParam("fbip_2")->value();
        
        preferences.putString("fb_ip2",inputMessage.c_str());
        fb_ip_list[1].fromString(preferences.getString("fb_ip2",""));

      }

      if(request->hasParam("fbip_3")) {

        inputMessage=request->getParam("fbip_3")->value();
        
        preferences.putString("fb_ip3",inputMessage.c_str());
        fb_ip_list[2].fromString(preferences.getString("fb_ip3",""));

      }

      if(request->hasParam("fbpt_1")) {

        inputMessage=request->getParam("fbpt_1")->value();

        if(inputMessage=="") preferences.putString("fb_pt1",port.udp);
        else preferences.putString("fb_pt1",inputMessage.c_str());
        
        fb_port_list[0]=preferences.getString("fb_pt1","").toInt();         

      }

      if(request->hasParam("fbpt_2")) {

        inputMessage=request->getParam("fbpt_2")->value();
        
        if(inputMessage=="") preferences.putString("fb_pt2",port.udp);
        else preferences.putString("fb_pt2",inputMessage.c_str());
        
        fb_port_list[1]=preferences.getString("fb_pt2","").toInt();         

      }

      if(request->hasParam("fbpt_3")) {

        inputMessage=request->getParam("fbpt_3")->value();
        
        if(inputMessage=="") preferences.putString("fb_pt3",port.udp);
        else preferences.putString("fb_pt3",inputMessage.c_str());
        
        fb_port_list[2]=preferences.getString("fb_pt3","").toInt();         

      }           

      preferences.end();
      
      request->redirect("/reset");
      
    });  

    server.on("/action_page_3.php",HTTP_GET,[](AsyncWebServerRequest *request) {

      request->redirect("/");

      ESP.restart();
      
    }); 

    server.on("/apreset.php",HTTP_GET,[](AsyncWebServerRequest *request) {

      reseta=1;
      request->redirect("/");
      
      ESP.restart();
      
    });

    server.begin();

    Update.onProgress(printProgress);

  }
      
}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_websocket.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// FUNCTION NOTIFY SENSORS TO WEBSERVER
//******************************************************************************************************************

void notifyClients(String sensorReadings) {
  
  ws.textAll(sensorReadings);

}

//******************************************************************************************************************
// FUNCTION INIT SOCKET
//******************************************************************************************************************

void initWebSocket() {

  ws.onEvent(onEvent);
  server.addHandler(&ws);

}

//******************************************************************************************************************
// FUNCTION ONEVENT WEBSOCKET
//******************************************************************************************************************

void onEvent(AsyncWebSocket *server,AsyncWebSocketClient *client,AwsEventType type,void *arg,uint8_t *data,size_t len) {
  
  switch (type) {
    
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n",client->id(),client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n",client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg,data,len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  
  }

}

//******************************************************************************************************************
// FUNCTION HANDLE WEBSOCKET MESSAGE
//******************************************************************************************************************

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  
  AwsFrameInfo *info=(AwsFrameInfo*)arg;
  
  if(info->final&&info->index==0&&info->len==len&&info->opcode==WS_TEXT) {

    data[len]=0;
    String message=(char*)data;

    Serial.println(message);

    if(strcmp((char*)data,"getReadings")==0) {
           
    }

  }

}

//******************************************************************************************************************

#line 1 "D:\\projetos\\flex controls\\fc-clock\\firmware\\fc-clock(v1)\\_wifi.ino"
//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// FUNCTION SETA COMO O WIFI VAI FUNCIONAR.
//******************************************************************************************************************

void wifi_operation_mode(void) {

  bool x;

  WiFi.mode(WIFI_OFF);

  preferences.begin("memory_cfg",false); 

  if((config_mode=="1")||(wifi.radio_status=="0")) {

    digitalWrite(LED1,LOW); 
    
    Serial.println("---------------------------------------------------------------------------------");
    Serial.println("AP Mode:");
    Serial.println("---------------------------------------------------------------------------------"); 
    
    WiFi.mode(WIFI_AP);   
    
    char macStr[20]={0};
    sprintf(macStr,"DBX-DLED:%04d",pin_number);

    Serial.println(macStr);
    Serial.println(wifi.ap_password); 
    
    WiFi.softAP(macStr,wifi.ap_password.c_str());
    
    wifi.status=1;

    preferences.putString("config_mode","0");
 
    webserver_start(); 
    
  } else {

    if(wifi.radio_status=="1") {

      Serial.println("---------------------------------------------------------------------------------");
      Serial.println("Station Mode:");
      Serial.println("---------------------------------------------------------------------------------"); 

      x=wifi_ip.fromString(wifi.ip);
      x=wifi_gateway.fromString(wifi.gtw);
      x=wifi_subnet.fromString(wifi.netmask);
      x=wifi_primary_DNS.fromString(wifi.dns);

      WiFi.config(wifi_ip,wifi_gateway,wifi_subnet,wifi_primary_DNS,secondaryDNS);  
      
      WiFi.mode(WIFI_MODE_STA);
      WiFi.begin(wifi.ssid.c_str(),wifi.password.c_str());

      Serial.println("Connecting to WiFi...");

      unsigned long previousMillis=millis();

      while(WiFi.status()!=WL_CONNECTED&&(millis()-previousMillis<=wifi_interval)) {
     
        Serial.print('.');

        digitalWrite(LED2,HIGH);           
        delay(500);
        digitalWrite(LED2,LOW);
        delay(500);
            
      }

      if(WiFi.status()!=WL_CONNECTED) {
        
        digitalWrite(LED2,LOW);

        Serial.println("\n\rRe-Starting can't connected...");
      
        wifi.status=0;

        preferences.putString("config_mode","1");
        config_mode="1";

        preferences.end();

        delay(1000);

        ESP.restart();
      
      } else {

        digitalWrite(LED2,HIGH);

        Serial.println("\n\r Wi-Fi Connected...");
        Serial.println(WiFi.localIP());

        preferences.putString("config_mode","0");
        config_mode="0";
    
        webserver_start();
    
        wifi.status=2;

      }
    
    } else {

      Serial.println("---------------------------------------------------------------------------------");
      Serial.println("Wi-Fi Radio OFF...");
      Serial.println("---------------------------------------------------------------------------------"); 

      wifi.status=0;

      WiFi.mode(WIFI_OFF);
      
    }

  }

  preferences.end();
  
}

//******************************************************************************************************************
// CHECA ETHERNET
//******************************************************************************************************************

void check_wifi(void ) {

  if(wifi.radio_status=="1") {

    if(config_mode=="0") {

      if(WiFi.status()!=WL_CONNECTED) {

        if(start_wifi_counter==0) {

          wifi_interval_ms=millis();
          start_wifi_counter=1;

        }

        if((millis()-wifi_interval_ms)>=1000*60) {

          if(WiFi.status()!=WL_CONNECTED) { 
        
            ESP.restart();

          } else {

            start_wifi_counter=0;

          }    
        
        }

      } else {

        start_wifi_counter=0;

      }

    }

  }

}

//******************************************************************************************************************

