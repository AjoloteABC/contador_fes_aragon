#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN   10
#define RST_PIN   9

MFRC522 mfrc522(SS_PIN, RST_PIN);

int retraso = 500;

// Pines de los fototransistores

int pinFototransistorIn = 0;
int pinFototransistorOut = 5;

int pinFototransistor01 = 1;
int pinFototransistor02 = 2;
int pinFototransistor03 = 3;
int pinFototransistor04 = 4;

// Estado de los fototransistores

bool monedaNueva = false;

bool estadoFototransistorIn = false;
bool estadoFototransistorOut = true;

bool estadoFototransistor01 = false;
bool estadoFototransistor02 = false;
bool estadoFototransistor03 = false;
bool estadoFototransistor04 = false;

// Conteo de cada denominacion

int monedas50 = 0;
int monedas01 = 0;
int monedas02 = 0;
int monedas05 = 0;
int monedas10 = 0;

int montoIngresado = 0;

// Finalizar transacción

bool recargar = false;


void contadorMonedas()
{
  if (estadoFototransistor02 == 0)
  {
    monedas01 += 1;
    return;
  }

  if (estadoFototransistor03 == 0)
  {
    monedas02 += 1;
    return;
  }

  if (estadoFototransistor04 == 0)
  {
    monedas05 += 1;
    return;
  }

  monedas10 += 1;
  return;
}


void setup()
{
  Serial.begin(9600);

  SPI.begin();

  mfrc522.PCD_Init();
}

void loop()
{
  // [01] Si no hay una tarjeta presente en el módulo lector
  if (! mfrc522.PICC_IsNewCardPresent())
  {
    // Serial.println("Ingresa Coin!");
    // delay(retraso);

    return;
  }

  // [02] Si no podemos obtener información del lector
  if (! mfrc522.PICC_ReadCardSerial())
  {
    // Serial.println("Sin comunicacion");
    // delay(retraso);

    return;
  }

  // VERIFICAR SI EL USUARIO EXISTE EN LA BASE DE DATOS

  Serial.println("Saldo ini: ");
  Serial.println("No pasar de 120");
  Serial.println();
  delay(retraso);

  // [03] Monitoreo del contador
  if (digitalRead(pinFototransistorIn)) // Entrada de moneda
  {
    estadoFototransistorIn = true;
    estadoFototransistorOut = false;
  }
  if (digitalRead(pinFototransistorOut)) // Salida de moneda
  {
    estadoFototransistorIn = false;
    estadoFototransistorOut = true;
  }


  // [04] Hay que esperar a que entre la moneda
  // para registrar los fototransistores
  if (estadoFototransistorIn == true && estadoFototransistorOut == false)
  {
    monedaNueva = true;

    if (digitalRead(pinFototransistor01))
    {
      estadoFototransistor01 = true;
    }

    if (digitalRead(pinFototransistor02))
    {
      estadoFototransistor02 = true;
    }

    if (digitalRead(pinFototransistor03))
    {
      estadoFototransistor03 = true;
    }

    if (digitalRead(pinFototransistor04))
    {
      estadoFototransistor04 = true;
    }
  }

  // [05] Hay que esperar a que salga la moneda y que sea nueva
  if (estadoFototransistorIn == false && estadoFototransistorOut == true && monedaNueva == true)
  {
    monedaNueva = false;

    // para identificarla y registrarla
    contadorMonedas();

    estadoFototransistor01 = false;
    estadoFototransistor02 = false;
    estadoFototransistor03 = false;
    estadoFototransistor04 = false;

    montoIngresado = (monedas01 * 1) + (monedas02 * 2) + (monedas05 * 5) + (monedas10 * 10);

    // CAMBIAR A: SI EL MONTO INGRESADO + EL SALDO INICIAL ES > 120
    if (montoIngresado < 0 || montoIngresado > 120)
    {
      montoIngresado = 120;
    }

    Serial.println("Monto ing: " + String(montoIngresado));
    Serial.println("Saldo pen: ");
    Serial.println();
    delay(retraso);
  }


  if (digitalRead(recargar))
  {
    // REALIZAR RECARGA
    
    // MOSTRAR SALDO TOTAL
    Serial.println("Saldo tot: ");

    montoIngresado = 0;

    monedas01 = 0;
    monedas02 = 0;
    monedas05 = 0;
    monedas10 = 0;

    recargar = false;

    mfrc522.PICC_HaltA();
  }
}
