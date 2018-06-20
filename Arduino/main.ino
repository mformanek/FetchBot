#include <Servo.h>
Servo myservo;         //servo lib and variable

//left wheel max speed at 210 900Hz
//righ wheel max speed at 240 900Hz
int pwm_value =0;
double count;
double values;

int lost =0;
int lastturn =0;
bool finish = false;

//serial stuff
String dataString ="";
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// define pins

  int path =0; //distance travelled
  
  const int pinCW_Left = 8; // connect pin 8 to ON/OFF switch, active HIGH
  const int pinCC_Left = 7; // connect pin 7 to clock-wise PMOS gate
  const int pinSpeed_Left = 5; // connect pin 5 to speed reference

  const int pinCW_Right = 12; // connect pin 12 to ON/OFF switch, active HIGH
  const int pinCC_Right = 11; // connect pin 11 to clock-wise PMOS gate
  const int pinSpeed_Right = 6; // connect pin 6 to speed reference
  const int led_pin = 13; // onboard LED
  int a =0;

void setup() {
  attachInterrupt(0,Right_ISR_count,RISING);
  attachInterrupt(1,Left_ISR_count,RISING);

  
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  pinMode(pinSpeed_Left,OUTPUT);

  pinMode(pinCW_Right,OUTPUT);
  pinMode(pinCC_Right,OUTPUT);
  pinMode(pinSpeed_Right,OUTPUT);

  pinMode(13,OUTPUT); // on-board LED
  
  DirectionNone();  // Make sure robot does not move during init
  SetSpeed(0);
  
  //serial stuff
  Serial.begin(115200);
  inputString.reserve(200);

  myservo.attach(9); // servo on pin 4
    RaiseBall();
    delay(2000);

  
}
void loop() {

  if (stringComplete) {
    // clear the string:
    dataString = inputString;
    inputString = "";
    stringComplete = false;
    }
   switch (dataString.toInt()) {
    case 0:    // 0 STOP
      DirectionNone();
      lost +=1;
      delay(50);
      if(lost ==50){
        GoBackward(5);
        delay(500);
        }
        if(lost ==75){
          if(lastturn == 1){ RotateRight(7); delay(500);lastturn =2;}
           else{RotateLeft(7);delay(500);lastturn =1;}    
        }
        if(lost >100){
          if(lastturn == 1){ RotateRight(5);delay(500);}
           else{RotateLeft(5);delay(500);}    
        }
      break;
    case 1:    // 1 GO FORWARD
      digitalWrite(13, HIGH);
      GoForward(5);
      path +=5;
      delay(400);
      lost =0;
      digitalWrite(13, LOW);
      break;
    case 2:    // 2 GO LEFT 
      digitalWrite(13, HIGH);
      RotateLeft(3);
      lastturn =2;
      delay(400);
      lost =0;
      
      digitalWrite(13, LOW);
      break;
    case 3:    // 3 GO RIGHT
      digitalWrite(13, HIGH);
      RotateRight(3);
      lastturn =1;
      delay(400);
      lost =0;
      digitalWrite(13, LOW);
      break;
   case 4:    // GRAB BALL
      lost =0;
      SetSpeed(5);
      digitalWrite(13, HIGH);
      LowerBall();
      GoForward(7);
      delay(2000);
      if(GotBall()==true){
      RaiseBall();
      GoHome(path);
      while(GotBall()==true)
      {
        delay(100);
      }
      path =0; 
      LowerBall();
      delay(50000000); 
      digitalWrite(13, LOW);
      }
      else{
        GoForward(7);
        delay(1000);
        RaiseBall();
        }
      SetSpeed(20);
      break;
    case 5:
    lost =0;
    // 5 GO FO RWARD 2x
      digitalWrite(13, HIGH);
      GoForward(15);
      path +=15;
      delay(1500);
      digitalWrite(13, LOW);
      break;
  }
  }

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    else{inputString += inChar;}
  }
}
