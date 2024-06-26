bool timerUiShowing = false;
bool timerStarted = false;
bool timerDone = false;
unsigned long startTime;
unsigned long endTime;
float timeDelta;
int endSpeed = 60;
int lastProgress = 0;

bool timerArmed() {
  return (!timerStarted && !timerDone) ?true :false;
}
bool timerRunning() {
  return (timerStarted && !timerDone) ?true :false;
}

void timerUi() {
  if (!timerUiShowing) {
    myLCD.clear();
    myLCD.setCursor(0,1);
    myLCD.print("Timer");
    myLCD.setCursor(0,2);
    myLCD.print("0-60");
    myLCD.setCursor(0,3);
    myLCD.print(speedUnitName);
  }
}

void timerMode() {
  timerUi();
  timerStarted = false;
  timerDone = false;
  lastProgress = 0;
}

void startTimer() {
  startTime = micros();
  timerStarted = true;
  myLCD.setCursor(0,1);
  myLCD.print("GO!!!!!");

}

void endTimer() {
  timerDone = true;
  timeDelta = endTime - startTime;
  timeDelta /= 1000000;
  
  myLCD.setCursor(0,0);
  myLCD.print("                    ");
  myLCD.setCursor(0,0);
  myLCD.print(timeDelta,3);
  myLCD.print(" seconds nice.");
  myLCD.setCursor(0,1);
  myLCD.print("Timer    ");
}

void progressBar(float speed) {
  float twentieths = endSpeed / 20;
  int progress = round(speed/twentieths);
  char progressChars[21] = "";

  if (progress != lastProgress){
    lastProgress = progress;
    for (int i = 0; i < 20; i++) {
      if (progress > i) {
        strcat(progressChars, "#" );
      } else {
        strcat(progressChars, " ");
      }
    }
    myLCD.setCursor(0,0);
    myLCD.print(progressChars);
  }
}

void checkSpeed(float speed) {
  if(!timerDone) {
    if (speed >= endSpeed) {
      endTime = micros();
      endTimer();
    } else {
      progressBar(speed);
    }
  }
}

