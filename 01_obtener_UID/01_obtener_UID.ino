#include <SPI.h>
#include <MFRC522.h>

// Definimos qué pines del arduino que se ocuparán
// en los pines SDA(SS) y RST del módulo lector
#define SS_PIN   10
#define RST_PIN   9

MFRC522 mfrc522(SS_PIN, RST_PIN);

int retraso = 500;

void setup()
{
  // Inicializa la transmisión de datos en serie
  Serial.begin(9600);

  // Inicialización del bus SPI configurando
  // SCK, MOSI, and SS como salidas
  // SCK y MOSI como low, y SS como high
  SPI.begin();

  // Inicialización del módulo lector
  mfrc522.PCD_Init();
}

void loop()
{
  // [01] Si no hay una tarjeta presente en el módulo lector
  
  // Si la misma tarjeta permanece en el lector varios ciclos,
  // la función devolverá falso después de la primera vez que se detecte 
  if (! mfrc522.PICC_IsNewCardPresent())
  {
    Serial.println("Ingresa Coin!");
    delay(retraso);

    // Vuelve a ejecutar el loop
    return;
  }

  // [02] Si no podemos obtener información del lector
  if (! mfrc522.PICC_ReadCardSerial())
  {
    Serial.println("Sin comunicacion");
    delay(retraso);

    return;
  }

  Serial.println("Unique Identification Number:");

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    /*
      Si el número almacenado en el byte "i" es menor a 0x10 = 16 (decimal)
      
      implica que sólo requiere de un dígito,
      por lo tanto, agregamos con anterioridad un espacio y un cero;
      
      de lo contrario, implica que requiere dos dígtitos
      y solo agregamos con anterioridad un espacio
    */
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  delay(retraso);

  // Finaliza la comunicación de la tarjeta
  // por lo que si se quiere volver a utilizar,
  // será necesario retirar y colocar nuevamente la tarjeta
  // mfrc522.PICC_HaltA();
}
