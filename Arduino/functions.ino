
#include <Servo.h>
int fsrAnalogPin = 5; // FSR is connected to analog 5

int pos = 12; //servo starting point at ground

const double radius = 10.25; //radius from center to wheel in cm
volatile unsigned long R_enc_count =0; //counting encoder pulses
volatile unsigned long L_enc_count =0; //counting encoder pulses

  float LmaX = 205 ;
  float RmaX = 210; //motor max speeds
 
void GoBackward(double cmDistance){
  ResetEncoders();
  int speeD =20;
  DirectionBackward();
   if(R_enc_count<L_enc_count){
       float temp =(L_enc_count - R_enc_count)/0.053;
       LmaX = LmaX - temp*1000;
    }
      if(R_enc_count>L_enc_count){
      float temp1 =(R_enc_count - L_enc_count)/0.053;
      RmaX = RmaX - temp1*1000;
    }
    
  while(((R_enc_count+L_enc_count)/2*0.053)<cmDistance){
    SetSpeed(speeD);
    delay(50);
    if(speeD <51){speeD = speeD+1;}
  }
DirectionNone();
SetSpeed(0);
}
  
void GoForward(double cmDistance){
  ResetEncoders();
  int speeD =20;
  DirectionForward();
   if(R_enc_count<L_enc_count){
       float temp =(L_enc_count - R_enc_count)/0.053;
       LmaX = LmaX - temp*1000;
    }
      if(R_enc_count>L_enc_count){
      float temp1 =(R_enc_count - L_enc_count)/0.053;
      RmaX = RmaX - temp1*1000;
    }
    
  while(((R_enc_count+L_enc_count)/2*0.053)<cmDistance){
    SetSpeed(speeD);
    delay(50);
    if(speeD <51){speeD = speeD+1;}
  }
DirectionNone();
SetSpeed(0);
}
void RotateRight(int deg){
 ResetEncoders();
 int speeD =20;
 DirectionCW();
    while(((R_enc_count+L_enc_count)/2*0.053)<(0.01745328*deg*radius)){  // pi/180*deg*radius
      SetSpeed(speeD);
      delay(50);
      if(speeD <31){speeD = speeD+1;}
  }
  DirectionNone();
  SetSpeed(0);
  delay(500);
}
void RotateLeft(int deg){
 ResetEncoders();
 int speeD =20;
 DirectionCC();
    while(((R_enc_count+L_enc_count)/2*0.053)<(0.01745328*deg*radius)){
      SetSpeed(speeD);
      delay(50);
      if(speeD <31){speeD = speeD+1;}
  }
  DirectionNone();
  SetSpeed(0);
  delay(500);
}
void Right_ISR_count(){
  R_enc_count++;
}
void Left_ISR_count(){
  L_enc_count++;
}
void SetSpeedLeft(int spd){
  analogWrite(pinSpeed_Left,(LmaX/100)*spd); // set speed reference, duty-cycle = 100/255 for left wheel
}
void SetSpeedRight(int spd){
  analogWrite(pinSpeed_Right,(RmaX/100)*spd); // set speed reference, duty-cycle = 100/255 for left wheel
}
void SetSpeed(int spd){
   SetSpeedRight(spd);
   SetSpeedLeft(spd);
}
void DirectionForward(){
    digitalWrite(pinCW_Left,HIGH); // start clockwise for left wheel
    digitalWrite(pinCC_Left,LOW); // stop counter-clockwise for left wheel
    digitalWrite(pinCW_Right,HIGH); // start clockwise for right wheel
    digitalWrite(pinCC_Right,LOW); // stop counter-clockwise for right wheel
}
void DirectionBackward(){
    digitalWrite(pinCW_Left,LOW); // stop clockwise for left wheel
    digitalWrite(pinCC_Left,HIGH); // start counter-clockwise for left wheel
    digitalWrite(pinCW_Right,LOW); // stop clockwise for right wheel
    digitalWrite(pinCC_Right,HIGH); // start counter-clockwise for right wheel
}
void DirectionNone(){
    digitalWrite(pinCW_Left,LOW); // stop clockwise for left wheel
    digitalWrite(pinCC_Left,LOW); // stop counter-clockwise for left wheel
    digitalWrite(pinCW_Right,LOW); // stop clockwise for right wheel
    digitalWrite(pinCC_Right,LOW); // stop counter-clockwise for right wheel
}
void DirectionCW(){
    digitalWrite(pinCW_Left,HIGH); // stop clockwise for left wheel
    digitalWrite(pinCC_Left,LOW); // stop counter-clockwise for left wheel
    digitalWrite(pinCW_Right,LOW); // stop clockwise for right wheel
    digitalWrite(pinCC_Right,HIGH); // stop counter-clockwise for right wheel
}
void DirectionCC(){
    digitalWrite(pinCW_Left,LOW); // stop clockwise for left wheel
    digitalWrite(pinCC_Left,HIGH); // stop counter-clockwise for left wheel
    digitalWrite(pinCW_Right,HIGH); // stop clockwise for right wheel
    digitalWrite(pinCC_Right,LOW); // stop counter-clockwise for right wheel
}
void ResetEncoders(){
  L_enc_count = 0;
  R_enc_count = 0;
}
void RaiseBall(){
  for (; pos >= 0; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
void LowerBall(){
  for (; pos <= 155; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);// waits 15ms for the servo to reach the position
  }
  delay(2000);
}
void GoHome(int distance){
  RotateLeft(180);
  GoForward(distance+distance/5+7);
}
bool GotBall(){
   int fsrReading =0;
   fsrReading = analogRead(fsrAnalogPin);
   if(map(fsrReading, 0, 1023, 0, 255)>150){return true;}
   else {return false;}
}


