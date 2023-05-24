#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

// Pines del módulo lector

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

String identificador = "";

// Entre menor sea el retraso, menos probabilidad
// de no registrar una moneda

int retraso = 200;

// Pines de los fototransistores

int pinFototransistorIn = A5;

int pinFototransistor01 = A4;
int pinFototransistor02 = A3;
int pinFototransistor03 = A2;
int pinFototransistor04 = A1;

int pinFototransistorOut = A0;

// Finalizar transacción

int pinRecargar = 6;

// Buzzer conector PC BIOS

int pinSpeaker = 8;

// Pines del display LCD 16x2

int RS = 0, EN = 1, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Saldo pendiente

bool saldoPendiente = false;

// Estado de la moneda

bool monedaNueva = false;

// Estado de los fototransistores

bool estadoFototransistorIn = false;

bool estadoFototransistor01 = false;
bool estadoFototransistor02 = false;
bool estadoFototransistor03 = false;
bool estadoFototransistor04 = false;

bool estadoFototransistorOut = true;

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
    lcd.clear();
    lcd.print("Procesando...");
    lcd.setCursor(0, 1);
    lcd.print("Mantener tarjeta");
    delay(3000);

    // REALIZAR RECARGA

    // Mostrar el saldo total
    lcd.clear();
    lcd.print("Saldo tot: ");

    for (int i = 0; i <= 2; i++)
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
  // Si se ocupa un objeto tipo LiquidCrystal
  // no puedes incializar el puerto serie, de lo contrario
  // no se mostrarán los mensajes en la pantalla LCD

  lcd.begin(16, 2);

  SPI.begin();

  mfrc522.PCD_Init();

  pinMode(pinFototransistorIn, INPUT);

  pinMode(pinFototransistor01, INPUT);
  pinMode(pinFototransistor02, INPUT);
  pinMode(pinFototransistor03, INPUT);
  pinMode(pinFototransistor04, INPUT);

  pinMode(pinFototransistorOut, INPUT);

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
      lcd.clear();
      lcd.print("Regresar Tarjeta");

      tone(pinSpeaker, 200);
      delay(500);
      noTone(pinSpeaker);
      delay(250);
      return;
    }
    lcd.clear();
    lcd.print("Ingresar Tarjeta");
    delay(retraso);
    return;
  }

  // [02] Mientras haya comuniación con la tarjeta.
  // El while se debe ocupar en lugar de un if
  while (mfrc522.PICC_ReadCardSerial())
  {
    if (!saldoPendiente)
    {
      identificador = "/";

      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
          identificador = identificador + "0" + String(mfrc522.uid.uidByte[i], HEX);
        }
        else
        {
          identificador = identificador + String(mfrc522.uid.uidByte[i], HEX);
        }
      }

      lcd.clear();
      lcd.print("Saldo ini: ");
      lcd.setCursor(0, 1);
      lcd.print("No pasar de 120");
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
      lcd.clear();
      lcd.print("Monto ing: " + String(montoIngresado));
      lcd.setCursor(0, 1);
      lcd.print("Saldo pen: ");
      delay(retraso);
    }

    // [07] Finalizar el proceso de recarga
    recargarSaldoIngresado();
  }
}
