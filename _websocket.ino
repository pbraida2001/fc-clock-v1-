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
