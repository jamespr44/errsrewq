#include <SparkFun_MMA8452Q.h>
#include <Servo.h>
#include <Wire.h>

MMA8452Q accel;
Servo servo1;
Servo servo2;
Servo servo3;
int buttonPin = A1;
unsigned long previousMillis = 0;
int a = 200;
unsigned long unlock = 0;
int m1 = 1500;
int m2 = 1500;
int m3 = 1500;

String serialInput = "";
const String password = "enter";
bool passed = false;
bool button = false;
bool manual = false;
bool sensorvalueprint = false;
bool input = false;

unsigned long flashedRed = 0;
unsigned long flashedBlue = 0;
unsigned long flashStartTime = 0;
int flashed = 0;
int flash = 125;
int flashperiod = 250;
//colours
int R = 7;
int G = 11;
int B = 12;



String inputString = "";      // a string to hold incoming data
bool stringComplete = false;  // whether the string is complete



void setup() {
  Serial.begin(9600);
  accel.init();
  servo1.attach(5);
  servo2.attach(6);
  servo3.attach(9);

  servo1.writeMicroseconds(1500);
  servo2.writeMicroseconds(1500);
  servo3.writeMicroseconds(1500);

  pinMode(buttonPin, INPUT_PULLUP);
}

void checkPassword(String input) {
  if (input == password) {
    passed = true;
    Serial.println("Password correct. Access granted.");
  } else {
    Serial.println("Incorrect password. Access denied.");
  }
}
void RGB(int Red, int Green, int Blue) {
  analogWrite(R, Red);
  analogWrite(G, Green);
  analogWrite(B, Blue);
}
void setRed() {
  RGB(255, 0, 0);
}
void setGreen() {
  RGB(0, 255, 0);
}
void setBlue() {
  RGB(0, 0, 255);
}
void ledoff() {
  RGB(0, 0, 0);
}
void ledflashBG() {
  if (millis() - flashStartTime >= flash) {
    flashStartTime = millis();
    if (flashedBlue == 0) {
      flashedBlue = flashStartTime;
    }
    if (millis() - flashedBlue >= flashperiod) {
      flashedBlue = millis();
    }
  }
  if (millis() - flashedBlue <= flash / 2) {
    setBlue();
  } else {
    setRed();
  }
}
void ledflashRed() {
  if (millis() - flashStartTime >= flash) {
    flashStartTime = millis();
    if (flashedRed == 0) {
      flashedRed = flashStartTime;
    }
    if (millis() - flashedRed >= flashperiod) {
      flashedRed = millis();
    }
  }
  if (millis() - flashedRed <= flash / 2) {
    setRed();
  } else {
    ledoff();
  }
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void serialinputs(String command) {
  if (input = true) {
    if (command == "a+") {
      a += 50;
      Serial.print("Value of a: ");
      Serial.println(a);
    } else if (command == "a-") {
      if (a > 0) {
        a -= 50;
        Serial.print("Value of a: ");
        Serial.println(a);
      } else {
        Serial.println("a cannot go below zero.");
      }
    } else if (command == "Restore") {
      Serial.println("Reset values ");
      a = 200;
    } else if (command == "Exit") {
      Serial.println("Exited");
      passed = false;
    }

    if (command == "manual") {
      manual = true;  // Toggle manual control
      Serial.println("manual mode on");
    }

    if (manual == true) {
      if (command == "m1+") {
        if (m1 < 2000) {
          m1 += 50;
          Serial.print("Value of m1: ");
          Serial.println(m1);
        } else {
          Serial.println("m1 cannot go above 2000.");
        }
      } else if (command == "m2+") {
        if (m2 < 2000) {
          m2 += 50;
          Serial.print("Value of m2: ");
          Serial.println(m2);
        } else {
          Serial.println("m2 cannot go above 2000.");
        }
      } else if (command == "m3+") {
        if (m3 < 2000) {
          m3 += 50;
          Serial.print("Value of m3: ");
          Serial.println(m3);
        } else {
          Serial.println("m3 cannot go above 2000.");
        }
      } else if (command == "m1-") {
        if (m1 > 1000) {
          m1 -= 50;
          Serial.print("Value of m1: ");
          Serial.println(m1);
        } else {
          Serial.println("m1 cannot go below 1000.");
        }
      } else if (command == "m2-") {
        if (m2 > 1000) {
          m2 -= 50;
          Serial.print("Value of m2: ");
          Serial.println(m2);
        } else {
          Serial.println("m2 cannot go below 1000.");
        }
      } else if (command == "m3-") {
        if (m3 > 1000) {
          m3 -= 50;  //
          Serial.print("Value of m3: ");
          Serial.println(m3);
        } else {
          Serial.println("m3 cannot go below 1000.");
        }
      } else if (command == "reset") {
        m1 = 1500;
        m2 = 1500;
        m3 = 1500;
        Serial.println("reset m values");
      }
    }
  }
  if (command == "sensor Values") {
    sensorvalueprint = true;
    Serial.println("serialplotmode ");
    if (command == "input") {
      input = true;
      sensorvalueprint = false;
    }
  }
}
void loop() {
  int buttonState = digitalRead(buttonPin);
  unsigned long currentMillis = millis();
  
   if (buttonState == LOW && !button) {
    button = true;
    unlock = currentMillis; // Record the time when the button was pressed
  }

  if (button && currentMillis - unlock >= 60000) {
    button = false; // Reset the button state after 60 seconds
  }

  if (stringComplete) {
    // trim the string to remove leading/trailing whitespace and newline
    inputString.trim();

    if (!passed) {
      checkPassword(inputString);
    } else {
      serialinputs(inputString);
    }


    inputString = "";
    stringComplete = false;
  }


  if (accel.available()) {
    accel.read();
  }
  if (passed) {
    if (currentMillis - previousMillis >= a && button == true) {
      previousMillis = currentMillis;

      float pitch = atan2(-accel.cx, sqrt(accel.cy * accel.cy + accel.cz * accel.cz)) * 180.0 / PI;
      float roll = atan2(accel.cy, accel.cz) * 180.0 / PI;
      int servo1Position = map(roll, -90, 90, 1000, 2000);
      int servo2Position = map(pitch, -90, 90, 1000, 2000);
      int servo3Position = map(roll + pitch, -180, 180, 1000, 2000);  // This might need adjustment

      servo1.writeMicroseconds(servo1Position);
      servo2.writeMicroseconds(servo2Position);
      servo3.writeMicroseconds(servo3Position);
      ledflashRed();
      Serial.println("w");
    }
  }

  if (manual) {
    servo1.writeMicroseconds(m1);
    servo2.writeMicroseconds(m2);
    servo3.writeMicroseconds(m3);
    ledflashBG();
  }
  if (sensorvalueprint) {
    Serial.print("X: ");
    Serial.print(accel.cx);
    Serial.print("\tY: ");
    Serial.print(accel.cy);
    Serial.print("\tZ: ");
    Serial.println(accel.cz);
  }
}
