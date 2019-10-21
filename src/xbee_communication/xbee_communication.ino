#include<Servo.h>

Servo servoLeft;
Servo servoRight;

#define Rx 17
#define Tx 16

int buttonPin = 9;
int signalsendLED = 10;
int signalreceiveLED = 8;
int QTIPinL = 47;
int QTIPinM = 51;
int QTIPinR = 52;
int threshold = 200;
int QTIVal_L = 0;
int QTIVal_M = 0;
int QTIVal_R = 0;

int signalsendLEDState = 0;
int qtileftState = 0;
int qtimiddleState = 0;
int qtirightState = 0;

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
}

void loop() {
  signalsendLEDState = digitalRead(buttonPin);

  /*
  if(signalsendLEDState == HIGH){
    digitalWrite(signalsendLED, HIGH);
    //if(Serial.available()){
    char outgoing = 'A';
    Serial2.print(outgoing);
    delay(100);
    //}
    
  } else{
    digitalWrite(signalsendLED, LOW);
  }

  if(Serial2.available()){
    char incoming = Serial2.read();
    Serial.println(incoming);
    digitalWrite(signalreceiveLED, HIGH);
    delay(100); 
  } else{
    digitalWrite(signalreceiveLED, LOW);
  }

  */
  //----------------------------------------------

  QTIVal_M = rcTime(QTIPinM);
  QTIVal_L = rcTime(QTIPinL);
  QTIVal_R = rcTime(QTIPinR);
  
  if(qtiLogic(QTIVal_L, QTIVal_M, QTIVal_R)==3){
    mainTurn();
  }
  else if(qtiLogic(QTIVal_L, QTIVal_M, QTIVal_R)==2){
    goForward();
  }

  /*
  Serial.println(rcTime(QTIPinM));
  if(rcTime(QTIPinM) > threshold){
    goForward();
    delay(1000);
  } else{
    servoStop();
    delay(1000);
  }
  */
  
  delay(1000);
}

void forwardTest1(){
  goForward();
  
  if(qtiLogic(QTIVal_L, QTIVal_M, QTIVal_R)==0){
    servoStop();
  }

  goForward();

  if(qtiLogic(QTIVal_L, QTIVal_M, QTIVal_R)==0){
    servoStop();
  }
}

//function to determine location
int qtiLogic(int qtileft, int qtimiddle, int qtiright){
  // 0 at short hashmark (beginning)
  // 1 at long hashmark (stop points)
  // 2 at blackline
  // 3 at leftwhitespace
  // 4 at left and middle whitespace
  
  if(qtileft > threshold && qtimiddle > threshold && qtiright > threshold){
    return 1;
  }
  if(qtileft > threshold && qtimiddle > threshold && qtiright < threshold){
    return 0;
  }

  if(qtimiddle > threshold && qtileft < threshold){
    return 3;
  }

  if(qtimiddle < threshold && qtileft < threshold){
    return 4;
  }
  
  // blackline
  if( qtimiddle > threshold){
    return 2;
  }
}

void goForward(){
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
}

void servoStop(){
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
}

//function for turning
void mainTurn(){
  servoLeft.writeMicroseconds(1750);
  servoRight.writeMicroseconds(1350);
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
