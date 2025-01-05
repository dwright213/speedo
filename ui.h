#define ENCODER_CLK 2
#define ENCODER_DT  5


int lastClk = HIGH;
int optionSelected = 0;

int speedUnitNumber = 0;
float speedUnitLengthFactor = 1;
float speedUnitTimeFactor = 1;
char speedUnitName[5] = "km/h";

// given a number, sets the relevant unit from the menu.
void setUnit(int menuChoice) {
  switch (menuChoice) {
  case 0:
    speedUnitNumber = 0;
    strncpy(speedUnitName,"km/h",5);
    speedUnitLengthFactor = 1;
    speedUnitTimeFactor = 1;
    break;
  case 1:
    speedUnitNumber = 1;
    strncpy(speedUnitName,"mph",4);
    speedUnitLengthFactor = 0.621371;
    speedUnitTimeFactor = 1;
    break;
  case 2:
    speedUnitNumber = 2;
    strncpy(speedUnitName,"mach",5);
    speedUnitLengthFactor = 0.00081;
    speedUnitTimeFactor = 1;
    break;
  case 3:
    speedUnitNumber = 3;
    strncpy(speedUnitName,"hd/s",5);
    speedUnitLengthFactor = 6666.666666666666667;
    speedUnitTimeFactor = 0.000277778;
    break;
  }
  Serial.println(speedUnitName);
}

float getConvertedSpeed(float kmh) {
  if (speedUnitNumber == 0) {
    return kmh;
  }
  float convertedSpeed; 
  convertedSpeed = ((kmh * speedUnitTimeFactor) * speedUnitLengthFactor);
  convertedSpeed *= 1000;
  convertedSpeed = round(convertedSpeed);
  convertedSpeed /= 1000;
  return convertedSpeed;
}

void drawSelector() {
  myLCD.setCursor(0,optionSelected);
  myLCD.write(B01111110);
}

void redrawSelector(bool direction) {
  myLCD.setCursor(0,optionSelected);
  if (direction) {
    Serial.println("cursor down");
    if ((optionSelected + 1) > 3){
      optionSelected = 0;
    } else {
      optionSelected += 1;
    }
  } else {
    Serial.println("cursor up");
    if ((optionSelected - 1) < 0){
      optionSelected = 3;
    } else {
      optionSelected -= 1;
    }
  }
  setUnit(optionSelected);
  myLCD.print(" ");
  myLCD.setCursor(0,optionSelected);
  myLCD.write(B01111110);
}

void showMenu() {
  myLCD.clear();
  myLCD.setCursor(1,0);
  myLCD.print("Km/h");
  myLCD.setCursor(1,1);
  myLCD.print("Mph");
  myLCD.setCursor(1,2);
  myLCD.print("Mach");
  myLCD.setCursor(1,3);
  myLCD.print("Hotdogs/second");
}