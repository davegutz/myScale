/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/Dave/Documents/GitHub/myScale/myScaleNew/src/myScale.ino"
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

// Constants always defined
// #define CONSTANT
void setup();
void loop();
#line 29 "c:/Users/Dave/Documents/GitHub/myScale/myScaleNew/src/myScale.ino"
#define BUTTON_PIN D2                                          // Button 3-way input momentary 3.3V, steady GND (D2)


//********constants for all*******************
#define PUBLISH_DELAY 1500UL      // Time between cloud updates (), millis
#define CONTROL_DELAY 15UL    // Control law wait (), millis

//
// Dependent includes
#include "myFilters.h"

// Global variables
LiquidCrystal_I2C   *lcd;
Hx711 scale(A2, A3);
LagTustin *gramFilter; // Tustin lag noise filter
const double tau = 2; // Lag value
Debounce *ButtonDebounce; // Pushbutton status
DetectRise *ButtonRise;   // Pushbutton leading edge
TFDelay  *roseDelayed;  // Power wait for Serial turn on
TFDelay  *PowerDelayed;   // ESC wait for boot
bool powerEnable = false; // Turn on ESC power


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

  // Filters
  double T = float(CONTROL_DELAY) / 1000.0;
  gramFilter = new LagTustin(T, tau, -45400, 45400);
  ButtonDebounce = new Debounce(0, 2);
  ButtonRise = new DetectRise();
  roseDelayed = new TFDelay(false, 5.0, 0.0, 2.5);
  PowerDelayed  = new TFDelay(false, 5.0, 0.0, 2.5);

  delay ( 1000 );
}

void loop() {
  int buttonState = 0;                    // Pushbutton
  unsigned long now = millis();           // Keep track of time
  static unsigned long lastButton = 0UL;  // Last button push time, millis
  static bool buttonRose = false;         // Leading edge of button
  static bool buttonRoseDelayed = false;  // remembered button
  static unsigned long lastControl = 0UL;    // Last control law time, micros
  static unsigned long lastPublish = 0UL;    // Last publish time, micros
  static int RESET = 1;                   // Dynamic reset
  static unsigned long start = 0UL;       // Time to start looping, micros
  static double updateTime = 0.0;         // Control law update time, sec
  static double gramFilt = 0;             // Filtered gram reading
  static double gramNull = 0;    // Tare
  double gram   = scale.getGram();
  static double gramNet = gram - gramNull;
  static double oz = gramNet/454*16;
  double elapsedTime;                     // elapsed time, millis
  bool control;                           // Control frame, T/F
  bool publish;                           // Publish, T/F

  // Executive
  if (start == 0UL) start = now;
  elapsedTime = double(now - start) * 1e-3;
  unsigned long deltaTick = now - lastControl;
  control = (deltaTick >= CONTROL_DELAY);
  if (control)
  {
    updateTime = float(deltaTick) / 1000.0;
    lastControl = now;
    powerEnable = PowerDelayed->calculate(true);
    buttonState = ButtonDebounce->calculate(digitalRead(BUTTON_PIN));
    buttonRose = ButtonRise->calculate(buttonState);
    buttonRoseDelayed = roseDelayed->calculate(buttonRose);
    gramFilt = gramFilter->calculate(gram, RESET, updateTime);
    gramNet = gramFilt - gramNull;
    oz = gramNet/454*16;
//  }
//  if ( (buttonRose) && (now - lastButton > 200UL))
//   {
    if (buttonRoseDelayed || !powerEnable)
    {
     lastButton = now;
     gramNull = gramFilt;
     Serial.printf(" -------------%4.1fg\n", gramNull);
     lcd->setCursor(0,0);
     lcd->print("--------------");
     lcd->setCursor(0,1);
     lcd->print("--------------");
     delay(400);
    }
    if (RESET)
      if (RESET>2) RESET = 0;
      else RESET += 1;
  }
  publish = ((now - lastPublish) >= PUBLISH_DELAY);
  if (publish)
  {
    lastPublish = now;
    Serial.printf(" %4.1fg, %4.1fg, %4.1fg, %4.2foz, %6.3fs\n", gram, gramFilt, gramNet, oz, updateTime);
    // Display
    lcd->setCursor(0,0);
    lcd->print("              g");
    lcd->setCursor(0,1);
    lcd->print("              oz");
    lcd->setCursor(8,0);
    lcd->print(gramNet);
    lcd->setCursor(8,1);
    lcd->print(oz);
  }

}
