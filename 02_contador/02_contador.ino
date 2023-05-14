
int retraso = 500;

// Pines de los fototransistores

int pinFototransistorIn = 0;
int pinFototransistorOut = 5;

int pinFototransistor01 = 1;
int pinFototransistor02 = 2;
int pinFototransistor03 = 3;
int pinFototransistor04 = 4;

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

    if (montoIngresado < 0 || montoIngresado > 120)
    {
      montoIngresado = 120;
    }

    Serial.println("Monto ing: " + String(montoIngresado));
    Serial.println();
    delay(retraso);
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(pinFototransistorIn, INPUT);
  pinMode(pinFototransistorOut, INPUT);

  pinMode(pinFototransistor01, INPUT);
  pinMode(pinFototransistor02, INPUT);
  pinMode(pinFototransistor03, INPUT);
  pinMode(pinFototransistor04, INPUT);
}

void loop()
{
  // [04] Registrar la entrada y salida de la moneda
  registrarInOut();

  // [05] Tiene que entrar la moneda para registrar los fototransistores
  registrarFototransistores();

  // [06] Tiene que salir la moneda y que sea nueva para identificarla y registrarla
  registrarMoneda();
}
