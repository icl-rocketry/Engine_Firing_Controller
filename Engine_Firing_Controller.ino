#include <Servo.h>

int servo1_pin = 6;    //fuel
int servo2_pin = 5;    //oxidiser

int pyro1_pin  = 7;
int pyro2_pin  = 8;

int engine_fire = A1;

Servo fuelServo;
Servo oxidiserServo;

void setup() {
  
  pinMode(pyro1_pin, OUTPUT);
  pinMode(pyro2_pin, OUTPUT);
  
  pinMode(engine_fire, INPUT);

  pinMode(servo1_pin, OUTPUT);
  pinMode(servo2_pin, OUTPUT);

  digitalWrite(servo1_pin, HIGH);  

  fuelServo.attach(servo1_pin);
  oxidiserServo.attach(servo2_pin);

  fuelServo.write(0);
  oxidiserServo.write(0);
  delay(100);
  
  digitalWrite(pyro1_pin, LOW);
  digitalWrite(pyro2_pin, LOW);  

}  

void loop() {
  
  if (digitalRead(engine_fire) == HIGH){
  
    delay(100);
    long start_time = millis();
    
    // ignites both pyro channels
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 500)){
      digitalWrite(pyro1_pin, HIGH);
      digitalWrite(pyro2_pin, HIGH);
  
    }

    //valves to full bore
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1000)){

      fuelServo.write(185);
      delay(50);
      oxidiserServo.write(185);


    }

    //main operating loop, end when button is released or duration exceeds 10 seconds
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1500)){ 

    }

    

    fuelServo.write(0);
    oxidiserServo.write(0);
    digitalWrite(pyro1_pin, LOW);
    digitalWrite(pyro2_pin, LOW);

     while(digitalRead(engine_fire) == HIGH){ 
      delay(500);
    }

  }

}
