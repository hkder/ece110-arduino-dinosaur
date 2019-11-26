#include<Servo.h>

Servo servoLeft;
Servo servoRight;

void setup() {
  // put your setup code here, to run once:
  servoLeft.attach(12);
  servoRight.attach(11);
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);

  dance();
}

void loop() {
  // put your main code here, to run repeatedly:
  servoStop(1000);
}

void dance(){
  //move forward
  goForward(500);
  servoLeft.writeMicroseconds(1350);
  servoRight.writeMicroseconds(1350);
  delay(580);
  goForward(2800);
  
  
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
