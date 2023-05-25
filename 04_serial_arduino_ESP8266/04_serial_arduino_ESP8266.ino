#include<SoftwareSerial.h>

String sCadena;

// String sNick = "Usuario 01: ";
// String sNick = "Usuario 02: ";

// Arduino. Emulación de puerto serie (Rx, Tx)
// SoftwareSerial miSerial(SCL, SDA);

// NodeMCU ESP8266. Emulación de puerto serie (Rx, Tx)
// SoftwareSerial miSerial(14, 12);

void enviar()
{
  sCadena="";
  
  while(Serial.available())
  {
    sCadena = sCadena + char(Serial.read());
    delay(10);  
  }
  Serial.println(sNick + sCadena);
  miSerial.println(sNick + sCadena);
}

void recibir()
{
  sCadena = "";
  
  while(miSerial.available())
  {
    sCadena = sCadena + char(miSerial.read());
    delay(10);
  }
  Serial.println(sCadena);
}

void setup() 
{
  Serial.begin(9600);
  Serial.flush();
  
  miSerial.begin(9600);
  miSerial.flush();
  
  Serial.println("Bienvenido al chat");
  Serial.println();
}

void loop() 
{
  if(Serial.available())
  {
    enviar();
  }  
  if(miSerial.available())
  {
    recibir();
  }
}
