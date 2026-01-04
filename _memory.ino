
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
