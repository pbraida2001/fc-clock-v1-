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
//{"command":"setAutoBrightness","set":0,"type":0}
//{"command":"setStripEffect","effect":0,"type":0}
//{"command":"setStripEffectSpeed","speed":500,"type":0}

//{"command":"setClockMode","mode":0,"type":0}
//{"command":"setClockDate","date":0,"type":0}

//{"command":"setSensorsMode","mode":0,"type":0}   // 0-Desliga, 1-Temperatura, 2-Umidade, 3-Temperatura+Umidade
//{"command":"setTempScale","scale":0,"type":0} // 0-Celsius, 1-Fahrenheit

//{"command":"setStrip2Effect","effect":1,"type":0}  // 0=off 1=rainbow 2=chase 3=comet 4=breathe 5=strobe 6=colorwipe 7=twinkle 8=fire 9=scanner 10=fillfade 11=alternating 12=solid 13=rainbowchase 14=bounce 15=meteor 16=sparkle 17=runninglights 18=colorcycle 19=pacifica 20=dualscanner 21=lava 22=ice 23=wipeinout 24=fadeinout 25=theaterchase 26=lightning 27=confetti 28=gradientshift 29=plasma 30=ripple 31=cylon 32=sinelon 33=pixelstack 34=pixelrain 35=knightrider 36=colorsegments 37=shootingstar 38=heartbeat 39=marquee 40=cascade 41=sparkleburst 42=dualcolorwipe 43=northernlights 44=rainbowsegments 45=pulsewave 46=orbit 47=colordrip 48=firefly 49=glitterrainbow 50=popcorn 51=sunrise 52=matrix
//{"command":"setStrip2Color","r":255,"g":255,"b":255,"type":0}
//{"command":"setStrip2Brightness","brightness":255,"type":0}
//{"command":"setStrip2Speed","speed":10,"type":0}
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

      preferences.begin("memory_cfg",false);  

      preferences.putString("cmode",String(mode));

      preferences.end();

      cfg.cmode=mode;

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

      preferences.begin("memory_cfg",false);  

      preferences.putString("cdate",String(date));

      preferences.end();

      cfg.cdate=date;

    } else if(comando=="setAutoBrightness") {

      uint8_t set = jsonExtract(pkgbuffer[read_pkg].data,"set").toInt();

      setAutoBrightness(set);
      
      if(set==0) setStripBrightness(255);
      
      String aux_rip;

      aux_rip="{\"command\":\"setAutoBrightness\",\"set\":"+String(set) + ",\"type\":1}";

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

      preferences.putString("cauto",String(set));

      preferences.end();

      cfg.cauto=set;

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

    } else if(comando=="setStrip2Effect") {

      uint8_t effect = jsonExtract(pkgbuffer[read_pkg].data,"effect").toInt();
      if(effect > 52) effect = 52;

      setStrip2Effect(effect);

      String aux_rip;
      aux_rip="{\"command\":\"setStrip2Effect\",\"effect\":"+String(effect) + ",\"type\":1}";

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

      preferences.putString("cstrip2fx",String(effect));

      preferences.end();

      cfg.cstrip2effect=effect;

    } else if(comando=="setStrip2Color") {

      uint8_t r = jsonExtract(pkgbuffer[read_pkg].data,"r").toInt();
      uint8_t g = jsonExtract(pkgbuffer[read_pkg].data,"g").toInt();
      uint8_t b = jsonExtract(pkgbuffer[read_pkg].data,"b").toInt();

      strip2_color_r=r; strip2_color_g=g; strip2_color_b=b;
      setStrip2Color(r, g, b);

      String aux_rip;
      aux_rip="{\"command\":\"setStrip2Color\",\"r\":"+String(r)+",\"g\":"+String(g)+",\"b\":"+String(b)+",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") {
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);
      } else {
        String rc(aux_rip); char aux_aux[300];
        rc.toCharArray(aux_aux,rc.length()+1);
        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();
      }

      preferences.begin("memory_cfg",false);
      preferences.putString("s2cr",String(r));
      preferences.putString("s2cg",String(g));
      preferences.putString("s2cb",String(b));
      preferences.end();

    } else if(comando=="setStrip2Brightness") {

      uint8_t brightness = jsonExtract(pkgbuffer[read_pkg].data,"brightness").toInt();

      strip2Brightness=brightness;
      setStrip2Brightness(brightness);

      String aux_rip;
      aux_rip="{\"command\":\"setStrip2Brightness\",\"brightness\":"+String(brightness)+",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") {
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);
      } else {
        String rc(aux_rip); char aux_aux[300];
        rc.toCharArray(aux_aux,rc.length()+1);
        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();
      }

      preferences.begin("memory_cfg",false);
      preferences.putString("s2brigh",String(brightness));
      preferences.end();

    } else if(comando=="setStrip2Speed") {

      uint8_t spd = jsonExtract(pkgbuffer[read_pkg].data,"speed").toInt();

      strip2Speed=spd;
      setStrip2Speed(spd);

      String aux_rip;
      aux_rip="{\"command\":\"setStrip2Speed\",\"speed\":"+String(spd)+",\"type\":1}";

      if(pkgbuffer[read_pkg].type=="UDP") {
        send_message_udp(aux_rip,pkgbuffer[read_pkg].remote_ip,pkgbuffer[read_pkg].local_port);
      } else {
        String rc(aux_rip); char aux_aux[300];
        rc.toCharArray(aux_aux,rc.length()+1);
        pkgbuffer[read_pkg].client->add(aux_aux,rc.length());
        pkgbuffer[read_pkg].client->send();
      }

      preferences.begin("memory_cfg",false);
      preferences.putString("s2spd",String(spd));
      preferences.end();

    } else if(comando=="setSensorsMode") { // 0-Desliga, 1-Temperatura, 2-Umidade, 3-Temperatura+Umidade

      uint8_t mode = jsonExtract(pkgbuffer[read_pkg].data,"mode").toInt();

      sensores.mode=mode;

      String aux_rip;

      aux_rip="{\"command\":\"setSensorsMode\",\"mode\":"+String(mode) + ",\"type\":1}";

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

      preferences.putString("smode",String(mode));

      preferences.end();

    } else if(comando=="setTempScale") { // 0-Celsius, 1-Fahrenheit

      uint8_t scale = jsonExtract(pkgbuffer[read_pkg].data,"scale").toInt();

      sensores.temp_unit=scale;

      String aux_rip;

      aux_rip="{\"command\":\"setTempScale\",\"scale\":"+String(scale) + ",\"type\":1}";

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

      preferences.putString("tunit",String(scale));

      preferences.end();

    }

    read_pkg++;
    if(read_pkg>PKG_BUFFER_SIZE-2) read_pkg=0; 
      
  }
  
  return;
  
}

//******************************************************************************************************************
