int red = 45;
int green = 46;
int blue = 44;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  //digitalWrite(green, HIGH);
  //digitalWrite(red, HIGH);
  //digitalWrite(blue, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  float reading = (analogRead(9));

  float mag = reading*5.0;

  mag /= 1024.0;
  
  mag = mag - 2.50;
  mag = mag*5000;
  Serial.print("mag strength = ");
  Serial.print(mag,4);
  Serial.println(" Gauss");
  
  if(mag >= 0){
    setColor(0, 255, 127);
  }
  else{
    setColor(0,0,0);
  }

  delay(1000);
}

void setColor(int r, int g, int b){
  analogWrite(red, 255-r);
  analogWrite(green, 255-g);
  analogWrite(blue, 255-b);
}
