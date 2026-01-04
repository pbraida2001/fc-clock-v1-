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
