#include<Servo.h>
#include<SoftwareSerial.h>

Servo servoLeft;
Servo servoRight;

#define Rx 17
#define Tx 16
const int TxPin = 18;
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

const int red = 45;
const int green = 46;
const int blue = 44;
const int QTIPinL = 47;
const int QTIPinM = 51;
const int QTIPinR = 52;
const int threshold = 200;
int QTIVal_L = 0;
int QTIVal_M = 0;
int QTIVal_R = 0;
int result = -1000;

bool previouslyReadIntersection = false;
boolean finished_linefollow = false;
int info[4];
int receivedInfo[4];
boolean serialSent = false;

int stops = 0;
int hashMark_Location = 0;
float mag = -1000;   //do nothing
int minBot_Num = 1000;
int minLocation = 1000;

void setup() {
  //initialize serial
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(50);

  //initialize servo
  servoLeft.attach(12);
  servoRight.attach(11);
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);

  //for color generation
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  setColor(0, 0, 0);

  //code for LCD
  pinMode(TxPin, OUTPUT);
  digitalWrite(TxPin, HIGH);

  mySerial.begin(9600);
  delay(100);
  mySerial.write(12); // clear
  mySerial.write(17); // Turn backlight on
  delay(5);
  mySerial.print("YOSHI BEGINS");
  //mySerial.write(13); // change line
  delay(3000);
  mySerial.write(128);
  mySerial.print("Detecting...");

  //mySerial.print()
  //mySerial.write(18); //turn backlight off

}

void initialize_values() {
  //initilization before every loop
  QTIVal_M = rcTime(QTIPinM);
  QTIVal_L = rcTime(QTIPinL);
  QTIVal_R = rcTime(QTIPinR);

  result = qtiLogic(QTIVal_L, QTIVal_M, QTIVal_R);

  //initialization for magnetic field
  float reading = (analogRead(9));

  mag = reading * 5.0;
  mag /= 1024.0;
  mag = mag - 2.50;
  mag = mag * 5000;

  setColor(0,0,0);

}

void line_follow() {
  if (result == 7) {
    if (stops <= 5) servoStop(2500);
    if (previouslyReadIntersection == false) {
      stops++;
      previouslyReadIntersection = true;

      if (mag >= 0) {
        setColor(0, 255, 127);
        hashMark_Location = stops - 1;
        mySerial.write(148);
        mySerial.print("Hash ");
        mySerial.print(String(stops - 1));
        mySerial.print(" : FOUND");
        delay(100);
        char outgoing = 'A';
        //Serial2.print(outgoing);
        //delay(100);
      } else {
        mySerial.write(148);
        mySerial.print("Hash ");
        mySerial.print(String(stops - 1));
        mySerial.print(" : NO   ");
        delay(100);
        char outgoing = 'B';
        //Serial2.print(outgoing);
        //delay(100);
      }

    }

    if (stops <= 5) {
      goForward(300);
      setColor(0, 0, 0);
    }
  }

  if (result == 2) {
    goForward(100);
    previouslyReadIntersection = false;
  }

  if (result == 6 || result == 4) {
    turnLeft(500);
    previouslyReadIntersection = false;
  }

  if (result == 3 || result == 1) {
    turnRight(700);
    previouslyReadIntersection = false;
  }

  if (result == 0) {

    //servoStop(100); //1000
    //turnLeft(200); //200
    turnLeftForStop(100);
    previouslyReadIntersection = false;
  }
}

void writeInfoToArray(int n) {
  //int conversion = (int) n;
  int digit_ten = n / 10;
  int digit_one = n % 10;
  info[(digit_ten) - 1] = digit_one;
  receivedInfo[(digit_ten) - 1] = 1;
}

int parser(char n) {
  int charToInt = 0;
  int easyInt = n - 97;
  charToInt += ((easyInt / 6) + 1) * 10;
  charToInt += easyInt % 6;
  return charToInt;
}

//boolean function for receivedAllData
int receivedAllData() {
  for (int i = 0; i < 4; i++) {
    if (receivedInfo[i] == 0) {
      return 0;
    }
  }
  return 1;
}

void loop() {

  initialize_values();

  if (Serial2.available()) {
    char receivedChar = Serial2.read();
    Serial.print(receivedChar);
    writeInfoToArray(parser(receivedChar));
    
    Serial.print("[");
    for (int i = 0; i < 4; i++) {
      Serial.print(info[i]);
      Serial.print(",");
    }
    Serial.print("]");
    
    Serial.print(":");
    Serial.print(getMin());
    Serial.print(" ");
    delay(50);
  }

  if (stops > 5) {
    if (finished_linefollow == false) {
      //servoStop(1000);
      //Serial2.print(hashMark_Location);
      finished_linefollow = true;
      delay(100);
    }
    else {
      if (receivedAllData() == 1 && !serialSent) {
        int checker = checkForSuccess();
          // 0 if dino is captured
          // -1 if hashmark 5 -> do dance
          // 1 if dino escapes
        for (int i = 0; i < 20; i++) {
          delay(10);
          serialSender_min(checker);
          serialSent = true;
          delay(100);
        }
        mySerial.write(148);
        mySerial.print("Result: ");
        mySerial.print(String(checker));
        mySerial.print("      ");
        delay(100);

        servoStop(1000);
      } else {
        //do something when I didn't receive data.
        Serial.println("did not receive all data or serial already sent once");
        delay(1000);
      }
    }
  } else {
    line_follow();
  }

}

void serialSender_min(int c) {
  // 0 if dino is captured
  // -1 if hashmark 5 -> do dance
  // 1 if dino escapes
  if(c==-1){
    Serial2.print('5');
    delay(100);
    Serial2.print(String(minBot_Num));
  } else if(c==0){
    Serial2.print('6');
    delay(100);
    Serial2.print(String(minBot_Num));
  } else if(c==1){
    Serial2.print('7');
    delay(100);
    Serial2.print(String(minBot_Num));
  }
  /*
  if (minLocation == 1) {
    Serial2.print('z');
  } else if (minLocation == 2) {
    Serial2.print('{');
  } else if (minLocation == 3) {
    Serial2.print('|');
  } else if (minLocation == 4) {
    Serial2.print('}');
  } else if (c == -1) {
    Serial2.print('~');
  } else if (c == 0) {
    Serial2.print('y');
  }
  */
}

int getMin() {
  int minVal = 50;
  for (int i = 0; i < 4; i++) {
    if (info[i] < minVal) {
      minVal = info[i];
      minBot_Num = i+1;    //stores first occurrence of min
    }
  }
  //Serial.print(minVal);
  return minVal;
}

int checkForSuccess() {
  // 0 if dino is captured
  // -1 if hashmark 5 -> do dance
  // 1 if dino escapes
  if (hashMark_Location == 5) return -1;
  if (info[hashMark_Location-1] == getMin()) {
    minLocation = hashMark_Location;
    return 1;
  } else {
    return 0;
  }
}

//function to determine location
int qtiLogic(int qtileft, int qtimiddle, int qtiright) {
  // 0 at white-white-white
  // 1 at white-white-black
  // 2 at white-black-white
  // 3 at white-black-black
  // 4 at black-white-white
  // 5 at black-white-black
  // 6 at black-black-white
  // 7 at black-black-black

  if (qtileft < threshold && qtimiddle < threshold && qtiright < threshold) {
    return 0;
  }

  if (qtileft < threshold && qtimiddle < threshold && qtiright > threshold) {
    return 1;
  }

  if (qtileft < threshold && qtimiddle > threshold && qtiright < threshold) {
    return 2;
  }

  if (qtileft < threshold && qtimiddle > threshold && qtiright > threshold) {
    return 3;
  }

  if (qtileft > threshold && qtimiddle < threshold && qtiright < threshold) {
    return 4;
  }
  if (qtileft > threshold && qtimiddle < threshold && qtiright > threshold) {
    return 5;
  }
  if (qtileft > threshold && qtimiddle > threshold && qtiright < threshold) {
    return 6;
  }
  if (qtileft > threshold && qtimiddle > threshold && qtiright > threshold) {
    return 7;
  }
}

void pulse(int left, int right, int t) {
  servoLeft.writeMicroseconds(left);
  servoRight.writeMicroseconds(right);
  delay(t);
}

void goForward(int t) {
  servoLeft.writeMicroseconds(1580);
  servoRight.writeMicroseconds(1420);
  delay(t);
}

void goBack(int t) {
  servoLeft.writeMicroseconds(1450);
  servoRight.writeMicroseconds(1550);
  delay(t);
}

void servoStop(int t) {
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  delay(t);
}

void turnLeft(int t) {
  servoLeft.writeMicroseconds(1480); //1400
  servoRight.writeMicroseconds(1480); //1400
  delay(t);
}

void turnLeftForStop(int t) {
  servoLeft.writeMicroseconds(1480);
  servoLeft.writeMicroseconds(1480);
  delay(t);
}

void turnRight(int t) {
  servoLeft.writeMicroseconds(1520);
  servoRight.writeMicroseconds(1520);
  delay(t);
}

//function for turning
void mainTurn() {
  servoLeft.writeMicroseconds(1750);
  servoRight.writeMicroseconds(1350);
}

void setColor(int r, int g, int b) {
  analogWrite(red, 255 - r);
  analogWrite(green, 255 - g);
  analogWrite(blue, 255 - b);
}

void playMario() {
  mySerial.write(216);
  mySerial.write(210); //1/16
  mySerial.write(227); //e
  mySerial.write(211); //1/8
  mySerial.write(227); //e
  mySerial.write(211);
  mySerial.write(227); //e
  mySerial.write(210);
  mySerial.write(223); //c
  mySerial.write(211);
  mySerial.write(227); //e
  mySerial.write(212); // 1/4
  mySerial.write(230); //play g
  mySerial.write(215); //change scale
  mySerial.write(211); //1/8
  mySerial.write(230);

  delay(1000);
}

long rcTime(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(230);
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);

  long time = micros();
  while (digitalRead(pin)); time =
    micros() - time;
  return time;
}
