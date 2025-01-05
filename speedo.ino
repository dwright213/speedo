// display stuff
#include <Arduino.h>
#define USE_SERIAL_2004_LCD
#include "LCDBigNumbers.hpp"
#define LCD_I2C_ADDRESS 0x27
#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C myLCD(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
// Available big number fonts are: BIG_NUMBERS_FONT_1_COLUMN_2_ROWS_VARIANT_1, BIG_NUMBERS_FONT_2_COLUMN_2_ROWS_VARIANT_1,
// BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_1, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_2, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_3,
// BIG_NUMBERS_FONT_2_COLUMN_3_ROWS_VARIANT_1, BIG_NUMBERS_FONT_2_COLUMN_3_ROWS_VARIANT_2, BIG_NUMBERS_FONT_3_COLUMN_3_ROWS_VARIANT_1,
// BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_1, BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_2
LCDBigNumbers bigNumberLCD(&myLCD, BIG_NUMBERS_FONT_3_COLUMN_3_ROWS_VARIANT_1); // Use 3x4 numbers, 1. variant
// end display stuff

#define HALL_SENSOR_PIN 3
#define ENCODER_SW  6

#include "ui.h"
#include "button.h"
#include "timer.h"
#include "env.h"

#ifdef DEBUG
  #define Sprint(a) (Serial.print(a))
  #define Sprintln(a) (Serial.println(a))
#else
  #define Sprint(a)
  #define Sprintln(a)
#endif 

// variables for the business logic
const long circumference = 1857;
const long hourSeconds = 3600;
float secondsElapsed;
float lastBikeSpeed;
float bikeSpeed;

unsigned long lastRead = 0;
unsigned long currentRead;
unsigned long elapsedRead;
int lastState = HIGH;
int speedDigits = 4;
int uiMode = 0;
bool menuOpen = false;
bool zeroedOut = false;
bool cursorMove = false;
bool cursorDirection = false;
unsigned long menuTimer;
int menuTimeout = 3000;
int wheelSpeedTimeout = 3000;
bool timerModeStatus = false;
bool timerButtonStatus = false;
Button timerButton(ENCODER_SW);

bool inSpeedoMode() {
  return (uiMode == 0) ?true :false;
}

bool inMenuMode() {
  return (uiMode == 1) ?true :false;
}

bool inTimerMode() {
  return (uiMode == 2) ?true :false;
}

void setUiMode() {
  
  switch (uiMode) {
  case 0:
    // speedometer
    showMain();
    break;
  case 1:
    // menu
    showMenu();
    break;
  case 2:
    // timer
    timerMode();
    break;
  }
}

// for comparisons, not for actually showing.
int whole(float flotsam) {
  int wholeInt;
  wholeInt = flotsam * 100;
  return wholeInt;
}

float getSpeed(float rpm) {
  float kmh = (((rpm * circumference)/1000) * 60)/1000;
  kmh *= 100;
  kmh = round(kmh);
  kmh /= 100;
  return kmh;
}

void zeroOut() {
  bikeSpeed = 0;
  zeroedOut = true;
}

void showMain() {
  myLCD.clear();
  myLCD.setCursor(0,0);
  myLCD.print("GP shift");
  myLCD.setCursor(0,1);
  myLCD.print(speedUnitName);
}

void showSpeed() {
  int leftMargin = 12;
  int currentSpeedDigits = 2;
  int decimalPlaces = (speedUnitNumber == 2) ? 2 : 1;

  lastBikeSpeed = bikeSpeed;
  if (bikeSpeed != 0) {
    zeroedOut = false;
  }
  
  if (bikeSpeed >= 100) {
    leftMargin = 4;
    currentSpeedDigits = 4;
  } else if (bikeSpeed >= 10 || speedUnitNumber == 2) {
    leftMargin = 8;
    currentSpeedDigits = 3;
  }

  if (currentSpeedDigits < speedDigits) {
    int spaces = (speedDigits - currentSpeedDigits);
    bigNumberLCD.setBigNumberCursor(4, 1);
    if (spaces = 2) {
      bigNumberLCD.print("  ");
    } else {
      bigNumberLCD.print(" ");
    }
  }
  speedDigits = currentSpeedDigits;
  bigNumberLCD.setBigNumberCursor(leftMargin, 1);
  bigNumberLCD.print(bikeSpeed, decimalPlaces);
}

void readEncoder() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime < 100) {
    Sprintln(interruptTime - lastInterruptTime);
    return;
  }

  int dtValue = digitalRead(ENCODER_DT);
  menuTimer = millis();
  cursorMove = true;
  if (dtValue == HIGH) {
    cursorDirection = true;
  }
  if (dtValue == LOW) {
    cursorDirection = false;
  }

  lastInterruptTime = interruptTime;
}

void setup() 
{
  #ifdef WOKWI
    pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  #else
    pinMode(HALL_SENSOR_PIN, INPUT);
  #endif 
  Serial.begin(115200);

  // set up the lcd
  myLCD.init();
  myLCD.clear();
  myLCD.backlight();
  bigNumberLCD.begin();

  // set up the encoder
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, FALLING);

  setUiMode();
  zeroOut();
}

void loop() {
  int pinValue = digitalRead((HALL_SENSOR_PIN));
  
  if (timerButton.isPressed() != timerButtonStatus) {
    timerButtonStatus = timerButton.isPressed();
    if (timerButton.isPressed()) {
      uiMode = (uiMode == 2) ?0 :2;
      setUiMode();
      Sprint("Timer mode status: ");
      Sprint(inTimerMode());
    }
  }

  if (cursorMove) {
    cursorMove = false;
    if (!inMenuMode()) {
      Sprintln("opening menu");
      uiMode = 1;
      setUiMode();
      drawSelector();
    } else {
      redrawSelector(cursorDirection);
    }
  }

  if (inMenuMode()) {
    if ((millis() - menuTimer) > menuTimeout) {
      Sprintln("closing menu");
      uiMode = 0;
      setUiMode();
      showSpeed();
      return;
    }
  }

  if (!zeroedOut && !inMenuMode()) {
    if (((millis() - lastRead) > wheelSpeedTimeout)) {
      zeroOut();
      showSpeed();
      if (inTimerMode() && timerStarted) {
        checkSpeed(bikeSpeed);
      }
      return;
    }
  }
  if (lastState != pinValue) {
    lastState = pinValue;
    currentRead = millis();
    if (pinValue == LOW) {
      if (inTimerMode()){
        if (!timerDone && !timerStarted) {
          startTimer();
        }
      }    

      elapsedRead = currentRead - lastRead;
      lastRead = currentRead;
      secondsElapsed = elapsedRead/1000.0;
      float rpm = 60/secondsElapsed;
      bikeSpeed = getSpeed(rpm);
      bikeSpeed = getConvertedSpeed(bikeSpeed);
            
      // if we are not in menu mode and the speed has changed
      if (!inMenuMode() && (whole(bikeSpeed) != whole(lastBikeSpeed))) {
        showSpeed();   
      }
      if (inTimerMode()){
        if (timerArmed()) {
          startTimer();
        }
        if (timerRunning()) {
          checkSpeed(bikeSpeed);
        }
      }    
    }
  }
}
