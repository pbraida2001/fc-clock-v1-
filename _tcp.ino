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
