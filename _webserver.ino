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
