/* sample for digital weight scale of hx711
 * library design: Weihong Guan (@aguegu)
 * library host on
 *https://github.com/aguegu/ardulibs/tree/3cdb78f3727d9682f7fd22156604fc1e4edd75d1/hx711
 */

// Hx711.DOUT - pin #A2
// Hx711.SCK - pin #A3

// Standard
#include "application.h"
#include "Hx711.h"
Hx711 scale(A2, A3);
#define BUTTON_PIN D2                                          // Button 3-way input momentary 3.3V, steady GND (D2)

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
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
  }
  delay(1);
}
