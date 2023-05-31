#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>

// Emulación de puerto serie (Rx, Tx)

SoftwareSerial miSerial(5, 4);

String sCadena;
String fieldPath;

// Conexión a WiFi

const char* ssid = "";
const char* pass = "";

// Conexión a Realtime Database de Firebase

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

FirebaseData firebaseData;

void setup()
{
  Serial.begin(9600);
  Serial.flush();

  miSerial.begin(9600);
  miSerial.flush();

  // Conexión a WiFi
 
  Serial.println("Conectando a: " + String(ssid));

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado!\n");

  // Conexión a Realtime Database de Firebase

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  while (!miSerial.available())
  {
    Serial.println(".");
    delay(500);
  }
  sCadena = "";

  while (miSerial.available())
  {
    sCadena += char(miSerial.read());
    delay(10);
  }

  Serial.println("Recibido por miSerial: " + sCadena);

  if (sCadena.length() == 8)
  {
    fieldPath = "/" + sCadena;

    if (Firebase.getInt(firebaseData, fieldPath))
    {
      Serial.println("Existe en B.D.");

      miSerial.print(firebaseData.intData());

      // Se envía como una cadena
      Serial.print("Enviado a Arduino: ");
      Serial.println(firebaseData.intData());
    }
    else
    {
      Serial.println("Error al obtener datos de Firebase");
      miSerial.print("404");
    }
  }
  else
  {
    Firebase.setInt(firebaseData, fieldPath, sCadena.toInt());
    fieldPath = "";
  }
}
