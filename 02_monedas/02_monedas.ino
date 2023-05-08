#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

int retraso = 500;

// Pines de los fototransistores

int pinFototransistorIn = 0;
int pinFototransistorOut = 5;

int pinFototransistor01 = 1;
int pinFototransistor02 = 2;
int pinFototransistor03 = 3;
int pinFototransistor04 = 4;

// Finalizar transacción

int recargar = 6;

// Estado de la moneda

bool monedaNueva = false;

// Estado de los fototransistores

bool estadoFototransistorIn = false;
bool estadoFototransistorOut = true;

bool estadoFototransistor01 = false;
bool estadoFototransistor02 = false;
bool estadoFototransistor03 = false;
bool estadoFototransistor04 = false;

// Conteo de cada denominacion

int monedas01 = 0;
int monedas02 = 0;
int monedas05 = 0;
int monedas10 = 0;

int montoIngresado = 0;

// [04] Registrar la entrada y salida de la moneda
void registrarInOut()
{
  if (digitalRead(pinFototransistorIn))
  {
    estadoFototransistorIn = true;
    estadoFototransistorOut = false;
  }
  if (digitalRead(pinFototransistorOut))
  {
    estadoFototransistorIn = false;
    estadoFototransistorOut = true;
  }
}

// [05] Tiene que entrar la moneda para registrar los fototransistores
void registrarFototransistores()
{
  if (estadoFototransistorIn == true && estadoFototransistorOut == false)
  {
    monedaNueva = true;

    if (digitalRead(pinFototransistor01)) // Moneda 01 peso
    {
      estadoFototransistor01 = true;
    }
    if (digitalRead(pinFototransistor02)) // Moneda 02 pesos
    {
      estadoFototransistor02 = true;
    }
    if (digitalRead(pinFototransistor03)) // Moneda 05 pesos
    {
      estadoFototransistor03 = true;
    }
    if (digitalRead(pinFototransistor04)) // Moneda 10 pesos
    {
      estadoFototransistor04 = true;
    }
  }
}

void identificarMoneda()
{
  if (estadoFototransistor02 == false)
  {
    monedas01 += 1;
    return;
  }
  if (estadoFototransistor03 == false)
  {
    monedas02 += 1;
    return;
  }
  if (estadoFototransistor04 == false)
  {
    monedas05 += 1;
    return;
  }
  monedas10 += 1;
  return;
}

// [06] Tiene que salir la moneda y que sea nueva...
void registrarMoneda()
{
  if (estadoFototransistorIn == false && estadoFototransistorOut == true && monedaNueva == true)
  {
    monedaNueva = false;

    // para identificarla...
    identificarMoneda();

    estadoFototransistor01 = false;
    estadoFototransistor02 = false;
    estadoFototransistor03 = false;
    estadoFototransistor04 = false;

    // y registrarla
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
}

// [07] Finalizar el proceso de recarga
void recargarSaldoIngresado()
{
  if (digitalRead(recargar))
  {
    // REALIZAR RECARGA

    // Mostrar el saldo total
    Serial.println("----------------");
    Serial.println("Saldo tot: ");
    Serial.println();

    montoIngresado = 0;

    monedas01 = 0;
    monedas02 = 0;
    monedas05 = 0;
    monedas10 = 0;

    mfrc522.PICC_HaltA();
  }
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
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    Serial.println("Ingresa Tarjeta");
    Serial.println();
    delay(retraso);
    return;
  }

  // [02] Mientras haya comuniación con la tarjeta
  while (mfrc522.PICC_ReadCardSerial())
  {
    // [03] VERIFICAR SI EL USUARIO EXISTE EN LA BASE DE DATOS

    Serial.println("Saldo ini: ");
    Serial.println("No pasar de 120");
    Serial.println();
    delay(retraso);

    // [04] Registrar la entrada y salida de la moneda
    registrarInOut();

    // [05] Tiene que entrar la moneda para registrar los fototransistores
    registrarFototransistores();

    // [06] Tiene que salir la moneda y que sea nueva para identificarla y registrarla
    registrarMoneda();

    // [07] Finalizar el proceso de recarga
    void recargarSaldoIngresado();
  }
}
