/* sample for digital weight scale of hx711
 * library design: Weihong Guan (@aguegu)
 * library host on
 *https://github.com/aguegu/ardulibs/tree/3cdb78f3727d9682f7fd22156604fc1e4edd75d1/hx711
 */


// Standard
#include "application.h"
#include "Hx711.h"
#include "LiquidCrystal_I2C.h"
/*
* Pin Connections:
 * Hx711.DOUT A2
 * Hx711.SCK  A3
 * LCD  SCL = D1
 *  PULL 5K = D1 to VIN
 * LCD  SDA = D0
 *  PULL 5K = D0 to VIN
 * LCD  VCC = VIN
 * LCD  GND = GND
 * BUTT HI  = D2
 * BUTT LO1 - 3v3
 * BUTT LO2 - 10k to GND
*/
LiquidCrystal_I2C   *lcd;
Hx711 scale(A2, A3);
#define BUTTON_PIN D2                                          // Button 3-way input momentary 3.3V, steady GND (D2)

void setup() {
  scale.setScale(214.7);
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  lcd = new LiquidCrystal_I2C(0x3F, 16, 2);  // Set the LCD I2C address
  lcd->init();                      // initialize the lcd
  lcd->backlight();
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print("              g");
  lcd->setCursor(0,1);
  lcd->print("              oz");
  delay ( 1000 );
}

void loop() {
  int buttonState = 0;                    // Pushbutton
  unsigned long now = millis();           // Keep track of time
  static unsigned long lastButton = 0UL;  // Last button push time, millis
  static double        gramNull   = 0;    // Tare
  double gram   = scale.getGram();
  double gramNet = gram - gramNull;
  double oz = gramNet/454*16;
  Serial.printf(" %4.1fg, %4.2foz\n", gramNet, oz);
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState==HIGH && (now - lastButton > 200UL))
  {
    lastButton = now;
    gramNull = gram;
    Serial.printf(" -------------%4.1fg\n", gramNull);
    lcd->setCursor(0,0);
    lcd->print("--------------");
    lcd->setCursor(0,1);
    lcd->print("--------------");
    delay(400);
  }

  // Display
  lcd->setCursor(0,0);
  lcd->print("              g");
  lcd->setCursor(0,1);
  lcd->print("              oz");
  lcd->setCursor(8,0);
  lcd->print(gramNet);
  lcd->setCursor(8,1);
  lcd->print(oz);

  delay(1);
}
