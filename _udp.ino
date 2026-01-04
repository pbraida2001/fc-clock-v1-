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
