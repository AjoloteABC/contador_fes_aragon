#include <LiquidCrystal.h>

// Creación del objeto tipo LiquidCrystal
// definiedo los pines a utilizar
int RS = 0, EN = 1, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup()
{
  // Inicializa el objeto tipo LiquidCrystal
  // definiendo el número de columnas y filas
  lcd.begin(16, 2);

  // Mensaje mostrado en el Display
  lcd.print("Hola Mundo");
}

void loop()
{
  // Ubicación del cursor en la columna cero de la segunda fila
  lcd.setCursor(0, 1);

  // Mensaje de segundos transcurridos en el Display
  lcd.print(String(millis() / 1000) + " segundos");
  delay(100);
}
