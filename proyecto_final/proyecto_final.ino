#include<SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Entre menor el retraso, menos probabilidad
// de no registrar una moneda

int retraso = 200;

// Pines de los fototransistores

int pinFototransistorIn = 0;
int pinFototransistorOut = 5;

int pinFototransistor01 = 1;
int pinFototransistor02 = 2;
int pinFototransistor03 = 3;
int pinFototransistor04 = 4;

// Finalizar transacción

int pinRecargar = 6;

// Buzzer conector PC BIOS

int pinSpeaker = 8;

// Saldo pendiente

bool saldoPendiente = false;

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
    // Serial.println("Entrada");

    estadoFototransistorIn = true;
    estadoFototransistorOut = false;
  }
  if (digitalRead(pinFototransistorOut))
  {
    // Serial.println("Salida");

    estadoFototransistorIn = false;
    estadoFototransistorOut = true;
  }
}

// [05] Tiene que entrar la moneda para registrar los fototransistores
void registrarFototransistores()
{
  if (estadoFototransistorIn && !estadoFototransistorOut)
  {
    monedaNueva = true;

    // Debe de asignarse el valor en registrarFotorainsistores
    // y no en registrarMoneda, de lo contrario la primer moneda siempre será 01 peso
    saldoPendiente = true;

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
  if (!estadoFototransistor02)
  {
    monedas01 += 1;
    return;
  }
  if (!estadoFototransistor03)
  {
    monedas02 += 1;
    return;
  }
  if (!estadoFototransistor04)
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
  if (!estadoFototransistorIn && estadoFototransistorOut && monedaNueva)
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
  }
}

// [07] Finalizar el proceso de recarga
void recargarSaldoIngresado()
{
  if (digitalRead(pinRecargar) && saldoPendiente)
  {
    Serial.println("Procesando...");
    Serial.println();
    delay(3000);

    // REALIZAR RECARGA

    // Mostrar el saldo total
    Serial.println("----------------");
    Serial.println("Saldo tot: ");
    Serial.println();

    for (int i = 0; i <= 3; i++)
    {
      tone(pinSpeaker, 200);
      delay(500);
      noTone(pinSpeaker);
      delay(250);
    }

    montoIngresado = 0;

    monedas01 = 0;
    monedas02 = 0;
    monedas05 = 0;
    monedas10 = 0;

    saldoPendiente = false;

    mfrc522.PICC_HaltA();
  }
}

void setup()
{
  Serial.begin(9600);

  SPI.begin();

  mfrc522.PCD_Init();

  pinMode(pinFototransistorIn, INPUT);
  pinMode(pinFototransistorOut, INPUT);

  pinMode(pinFototransistor01, INPUT);
  pinMode(pinFototransistor02, INPUT);
  pinMode(pinFototransistor03, INPUT);
  pinMode(pinFototransistor04, INPUT);

  pinMode(pinRecargar, INPUT);

  pinMode(pinSpeaker, OUTPUT);
}

void loop()
{
  // El While no impide que se ejecute la impresión "LOOP PRINCIPAL".
  // Durante el loop sólo hay que utilizar una vez IsNewCardPresent y ReadCardSerial
  // Serial.println("LOOP PRINCIPAL");

  // [01] Si no hay una tarjeta presente en el módulo lector
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    if (saldoPendiente)
    {
      Serial.println("Regresar Tarjeta");
      Serial.println();

      tone(pinSpeaker, 200);
      delay(500);
      noTone(pinSpeaker);
      delay(250);
      return;
    }
    Serial.println("Ingresar Tarjeta");
    Serial.println();
    delay(retraso);
    return;
  }

  // [02] Mientras haya comuniación con la tarjeta.
  // El while se debe ocupar en lugar de un if
  while (mfrc522.PICC_ReadCardSerial())
  {
    /*
      [03] VERIFICAR SI EL USUARIO EXISTE EN LA BASE DE DATOS

      Si no se encuentra el identificador en la B.D.

        Serial.println("Tarjeta invalida");
        Serial.println();
        delay(retraso);

        for (int i = 0; i <= 3; i++)
        {
          tone(pinSpeaker, 200);
          delay(500);
          noTone(pinSpeaker);
          delay(250);
        }
        mfrc522.PICC_HaltA();
    */

    if (!saldoPendiente)
    {
      Serial.println("Saldo ini: ");
      Serial.println("No pasar de 120");
      Serial.println();
      delay(retraso);
    }

    // [04] Registrar la entrada y salida de la moneda
    registrarInOut();

    // [05] Tiene que entrar la moneda para registrar los fototransistores
    registrarFototransistores();

    // [06] Tiene que salir la moneda y que sea nueva para identificarla y registrarla
    registrarMoneda();

    // No hay que agregar la condición monedaNueva, de lo contrario jamás hará la impresión
    if (!estadoFototransistorIn && estadoFototransistorOut && saldoPendiente)
    {
      Serial.println("Monto ing: " + String(montoIngresado));
      Serial.println("Saldo pen: ");
      Serial.println();
      delay(retraso);
    }

    // [07] Finalizar el proceso de recarga
    recargarSaldoIngresado();
  }
}
