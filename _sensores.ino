//******************************************************************************************************************
// Created by Pedro Braida Neto
// 18/08/2023 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// ENVIA VERSAO DO FIRMWARE
// {"command":"sensors_get","bind":0} 
//******************************************************************************************************************

void sensors_get(int bind,const char* aux_tipo,const IPAddress& aux_remote_ip,int aux_remote_port) {

  DynamicJsonDocument doc(220);
  char messagePayload[220];

  doc["command"]="sensors_get";

  //ana_status(1);

  doc["lux"]=get_sensor_BH1750();
  doc["temp"]=get_sensor_AHT10(0);
  doc["hum"]=get_sensor_AHT10(1);

  doc["bind"]=bind;
  doc["type"]=1;

  serializeJson(doc,messagePayload);

  //retorna_feed_back(aux_tipo,messagePayload,aux_remote_ip,aux_remote_port);  

}


//******************************************************************************************************************
// SET UP SENSORS
//******************************************************************************************************************

void sensors_setup(void) {

  if(lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {

   
    Serial.println(F("BH1750 initialising Ok..."));
    Serial.println("******************************************************************************************************************");
  
  } else {
    
    Serial.println(F("Error initialising BH1750"));
    Serial.println("******************************************************************************************************************");
  
  }

  if(aht.begin()) {

    Serial.println(F("AHT10 initialising Ok..."));
    Serial.println("******************************************************************************************************************");
  
  } else {

    Serial.println(F("Error initialising AHT10"));
    Serial.println("******************************************************************************************************************");
  
  }
    
}

//******************************************************************************************************************
// LE SENSOR DE LUZ(BH1750)
//******************************************************************************************************************

int get_sensor_BH1750(void) {

  float lux=0;  

  if(lightMeter.measurementReady()) {
    
    lux=lightMeter.readLightLevel();
      
  }

  sensores.light=lux;

  //Serial.print("Lux: ");
  //Serial.println(lux);

  return((int)lux);

}

//******************************************************************************************************************
// LE SENSOR DE TEMP/HUMIDADE(AHT10)
//******************************************************************************************************************

float get_sensor_AHT10(int tipo) {

  aht.getEvent(&AHT10_humid,&AHT10_temp);

  if(tipo==0) {

    //Serial.print("Temperature: ");
    //Serial.println(AHT10_temp.temperature);
    sensores.temp=AHT10_temp.temperature-sensores.temp_offset;
    return(sensores.temp);    

  } else {

    //Serial.print("Humidity: ");
    //Serial.println(AHT10_humid.relative_humidity);
    sensores.humid=AHT10_humid.relative_humidity;
    return(sensores.humid);

  }
    
}

//******************************************************************************************************************

