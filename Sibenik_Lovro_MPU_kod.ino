#include <IRremote.hpp> // uključuje ispravnu verziju IR remote biblioteke
#include <Servo.h> // uključuje biblioteku za servo motor
#define ENABLE 5
#define DIRA 3
#define DIRB 4

int i=0; 

const byte IR_RECEIVE_PIN = 2;   // pin za IR senzor
Servo myservo;
int motor_angle = 90;
bool continuousMode = false;
unsigned long lastUpdateTime = 0; // vrijeme zadnjeg ažuriranja
int increment = 1; // smjer pomicanja

void setup()
{
  pinMode(ENABLE,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);
  Serial.begin(115200); // inicijalizacija serijske komunikacije
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  myservo.attach(9); // deklarira pin za servo motor
  myservo.write(motor_angle);

  delay(2000);
}

void loop() 
{
  if (IrReceiver.decode()) // kad je gumb pritisnut i kod primljen
  {
    String ir_code = String(IrReceiver.decodedIRData.command, HEX);

    if (ir_code == "16") // 0
    {
      Serial.println("servo motor kut 0 stupnjeva");
      motor_angle = 0;
      myservo.write(motor_angle); // pomakni motor na 0 stupnjeva
      continuousMode = false;
    }
   
    else if (ir_code == "d") // 9
    {
      Serial.println("servo motor kut 180 stupnjeva");
      motor_angle = 180;
      myservo.write(motor_angle); // pomakni motor na 180 stupnjeva
      continuousMode = false;
    }
    
    else if (ir_code == "19") // EQ
    {
      Serial.println("servo motor kut 90 stupnjeva");
      motor_angle = 90;
      myservo.write(motor_angle); // pomakni motor na 90 stupnjeva -> početni položaj, sredina
      continuousMode = false;
    }
      
    else if (ir_code == "43") // >>|
    {
      if (motor_angle < 180) motor_angle += 5; // povećaj kut motora
      myservo.write(motor_angle);         // pomakni motor na taj kut
      continuousMode = false;
    }

    else if (ir_code == "44") // |<<
    {
      if (motor_angle > 0) motor_angle -= 5; // smanji kut motora
      myservo.write(motor_angle);      // pomakni motor na taj kut
      continuousMode = false;
    }

    else if (ir_code == "47") // ST/REPT
    {
      Serial.println("continuous mode - on");
      continuousMode = true;
      increment = (motor_angle < 90) ? -1 : 1; // Postavi smjer kretanja prema najbližem krajnjem kutu
    }

    else if (ir_code == "c") {
      Serial.println("speed mode - min"); //najsporiji rad DC motora
      analogWrite(ENABLE,175); //enable on
      digitalWrite(DIRA,HIGH); //one way
      digitalWrite(DIRB,LOW);
      delay(250);
    }

    else if (ir_code == "18") { 
      Serial.println("speed mode - mid"); // srednji rad DC motora
      analogWrite(ENABLE,220); //enable on
      digitalWrite(DIRA,HIGH); //one way
      digitalWrite(DIRB,LOW);
      delay(250);
    }

    else if (ir_code == "5e") {
      Serial.println("speed mode - max"); // najbrži rad DC motora
      analogWrite(ENABLE,255); //enable on
      digitalWrite(DIRA,HIGH); //one way
      digitalWrite(DIRB,LOW);
      delay(250);
    }

    else if (ir_code == "45") {
      Serial.println("fan - off");  // gašenje DC motora
      analogWrite(ENABLE,0); //enable on
      digitalWrite(DIRA,HIGH); //one way
      digitalWrite(DIRB,LOW);
      delay(250);
    }

    IrReceiver.resume();     // ponovno spreman za primanje sljedećeg koda
  }

  // Kontinuirani način rada: pomicanje servo motora naprijed-nazad između 0 i 180 stupnjeva
  if (continuousMode && millis() - lastUpdateTime >= 15) {
    lastUpdateTime = millis();
    motor_angle += increment;
    if (motor_angle >= 180) {
      motor_angle = 180;
      increment = -1;
    }
    if (motor_angle <= 0) {
      motor_angle = 0;
      increment = 1;
    }
    myservo.write(motor_angle);
  }
}
