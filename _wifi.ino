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
