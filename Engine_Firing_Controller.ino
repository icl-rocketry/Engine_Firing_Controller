#include <Servo.h>

int ledb_pin = 10;
int ledg_pin = 11;
int ledr_pin = 12;

int servo1_pin = 6;
int servo2_pin = 5;

int pyro1_pin  = 10;
int pyro2_pin  = 11;

int engine_fire    = A1;

Servo myservo1;
Servo myservo2;

void setup() {

  pinMode(ledb_pin, OUTPUT);
  pinMode(ledg_pin, OUTPUT);
  pinMode(ledr_pin, OUTPUT);

  pinMode(pyro1_pin, OUTPUT);
  pinMode(pyro2_pin, OUTPUT);
  
  pinMode(engine_fire, INPUT);

  pinMode(servo1_pin, OUTPUT);
  pinMode(servo2_pin, OUTPUT);

  digitalWrite(servo1_pin, HIGH);  

  myservo1.attach(servo1_pin);
  myservo2.attach(servo2_pin);

  myservo1.write(0);
  myservo2.write(0);
  delay(1000);
  
  digitalWrite(pyro1_pin, LOW);
  digitalWrite(pyro2_pin, LOW);  
  
  Serial.begin(9600);

  
}

void loop() {
  
  if (digitalRead(engine_fire) == HIGH){

    delay(100);
    long start_time = millis();

    // ignites both pyro channels
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 300)){

      digitalWrite(pyro1_pin, HIGH);
      digitalWrite(pyro2_pin, HIGH);

    }

    //move nitrous valve to pre position
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 700)){
      
      myservo1.write(55);
      delay(200);
      myservo2.write(90);

    }

    //nitrous to full bore
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1000)){ 

      myservo1.write(185);
      delay(200);
      myservo2.write(185);

    }

    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 3000)){ 

    }

    

    myservo1.write(0);
    myservo2.write(0);
    digitalWrite(pyro1_pin, LOW);
    digitalWrite(pyro2_pin, LOW);

     while(digitalRead(engine_fire) == HIGH){ 

    }

  }
}
