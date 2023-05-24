// Conexión a Realtime Database de Firebase

#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

const char* ssid = "";
const char* pass = "";

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

FirebaseData firebaseData;

bool iterar = true;

String fieldPath = "/c3a8241b";

void setup() 
{
  Serial.begin(115200);

  Serial.println("Conectando a: " + String(ssid));

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado!");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  while (iterar)
  {
    if (Firebase.getInt(firebaseData, fieldPath)) 
    {
      Serial.println("Existe en la base de datos");
      Serial.println(firebaseData.intData());

      Serial.println("Actualización:");
      Firebase.setInt(firebaseData, fieldPath, 22);
      Serial.println(firebaseData.intData());
    } 
    else 
    {
      Serial.println("Error al obtener datos de Firebase");
    }
    Firebase.end(firebaseData);
    
    iterar = false; 
  }
}
