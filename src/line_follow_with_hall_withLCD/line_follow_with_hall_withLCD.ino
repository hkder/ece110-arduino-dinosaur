#include<Servo.h>
#include<SoftwareSerial.h>

Servo servoLeft;
Servo servoRight;

#define Rx 17
#define Tx 16
const int TxPin = 18;
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

int red = 45;
int green = 46;
int blue = 44;

const int buttonPin = 9;
const int signalsendLED = 10;
const int signalreceiveLED = 8;
const int QTIPinL = 47;
const int QTIPinM = 51;
const int QTIPinR = 52;
const int threshold = 200;
int QTIVal_L = 0;
int QTIVal_M = 0;
int QTIVal_R = 0;

String myPrevious ="";

int signalsendLEDState = 0;
int qtileftState = 0;
int qtimiddleState = 0;
int qtirightState = 0;

int lastResultState = -1;
bool previouslyReadIntersection = false;
bool previousRight = false;
unsigned long currentMillis = 0;
unsigned long startMillis;
const unsigned long period = 2000; //the value is a number of milliseconds, i.e. 1 second

int stops = 0;
int location = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(signalsendLED, OUTPUT);
  pinMode(signalreceiveLED, OUTPUT);

  //initialize serial
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(50);

  //initialize pins
  digitalWrite(signalsendLED, LOW);
  digitalWrite(signalreceiveLED, LOW);

  //initialize servo
  servoLeft.attach(12);
  servoRight.attach(11);
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);

  //for color generation
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  setColor(0,0,0);

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

void loop() {
  //initilization before every loop
  QTIVal_M = rcTime(QTIPinM);
  QTIVal_L = rcTime(QTIPinL);
  QTIVal_R = rcTime(QTIPinR);
  
  int result = qtiLogic(QTIVal_L, QTIVal_M, QTIVal_R);
  Serial.print(result);
  currentMillis = millis();
  //Serial.print(stops);

  //initialization for magnetic field
  float reading = (analogRead(9));

  float mag = reading*5.0;

  mag /= 1024.0;
  
  mag = mag - 2.50;
  mag = mag*5000;

  if(stops > 5) servoStop(1000);
  
  //------------------------start main function after initialization-------------------
  if(result==7){
    if (stops <= 5) servoStop(2500);
    if(previouslyReadIntersection == false){
      stops++;
      previouslyReadIntersection = true;
      
      if(mag >= 0){
        setColor(0, 255, 127);
        location = stops;
        mySerial.write(148);
        mySerial.print("Hash ");
        mySerial.print(String(stops-1));
        mySerial.print(" : FOUND");
        delay(100);
        char outgoing = 'A';
        Serial2.print(outgoing);
        delay(100);
      } else{
        mySerial.write(148);
        mySerial.print("Hash ");
        mySerial.print(String(stops-1));
        mySerial.print(" : NO   ");
        delay(100);
        char outgoing = 'B';
        Serial2.print(outgoing);
        delay(100);
      }
     
    }
    
    if(stops <= 5){
    goForward(300);
    setColor(0,0,0);
    }
  }
  
  if(result == 2){
    goForward(100);
    previouslyReadIntersection = false;
    myPrevious = "forward";
  }

  if(result == 6 || result == 4){
    turnLeft(500);
    previouslyReadIntersection = false;
    myPrevious = "left";
  }

  if(result == 3 || result == 1){
    turnRight(700);
    previouslyReadIntersection = false;
    myPrevious = "right";
  }
  
  if(result == 0){
    /*
    if(currentMillis == 0){
      startMillis = millis();
    }
    currentMillis == millis();
    if(currentMillis - startMillis >= period){
      turnRight(700);
      currentMillis = 0;
    }
    */
    //servoStop(100); //1000
    //turnLeft(200); //200
    turnLeftForStop(100);
    previouslyReadIntersection = false;
    
    /*
    if(myPrevious == "right"){
      turnLeft(500);
      myPrevious = "";
    }
    if(myPrevious == "left"){
      turnRight(500);
      myPrevious = "";
    }
    if(myPrevious == "forward"){
      turnRight(3000);
      myPrevious = "";
    }
    goForward(100);
    */
  }
}

//function to determine location
int qtiLogic(int qtileft, int qtimiddle, int qtiright){
  // 0 at white-white-white
  // 1 at white-white-black
  // 2 at white-black-white
  // 3 at white-black-black
  // 4 at black-white-white
  // 5 at black-white-black
  // 6 at black-black-white
  // 7 at black-black-black

  if(qtileft < threshold && qtimiddle < threshold && qtiright < threshold){
    return 0;
  }
  
  if(qtileft < threshold && qtimiddle < threshold && qtiright > threshold){
    return 1;
  }
  
  if(qtileft < threshold && qtimiddle > threshold && qtiright < threshold){
    return 2;
  } 

  if(qtileft < threshold && qtimiddle > threshold && qtiright > threshold){
    return 3;
  }

  if(qtileft > threshold && qtimiddle < threshold && qtiright < threshold){
    return 4;
  }
  if(qtileft > threshold && qtimiddle < threshold && qtiright > threshold){
    return 5;
  }
  if(qtileft > threshold && qtimiddle > threshold && qtiright < threshold){
    return 6;
  }
  if(qtileft > threshold && qtimiddle > threshold && qtiright > threshold){
    return 7;
  }
}

void pulse(int left, int right, int t){
  servoLeft.writeMicroseconds(left);
  servoRight.writeMicroseconds(right);
  delay(t);
}

void goForward(int t){
  servoLeft.writeMicroseconds(1580);
  servoRight.writeMicroseconds(1420);
  delay(t);
}

void goBack(int t){
  servoLeft.writeMicroseconds(1450);
  servoRight.writeMicroseconds(1550);
  delay(t);
}

void servoStop(int t){
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  delay(t);
}

void turnLeft(int t){
  servoLeft.writeMicroseconds(1480); //1400
  servoRight.writeMicroseconds(1480); //1400
  delay(t);
}

void turnLeftForStop(int t){
  servoLeft.writeMicroseconds(1480);
  servoLeft.writeMicroseconds(1480);
  delay(t);
}

void turnRight(int t){
  servoLeft.writeMicroseconds(1520);
  servoRight.writeMicroseconds(1520);
  delay(t);
}

//function for turning
void mainTurn(){
  servoLeft.writeMicroseconds(1750);
  servoRight.writeMicroseconds(1350);
}

void setColor(int r, int g, int b){
  analogWrite(red, 255-r);
  analogWrite(green, 255-g);
  analogWrite(blue, 255-b);
}

void playMario(){
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
  while(digitalRead(pin)); time =
  micros() - time; 
  return time;
} 
