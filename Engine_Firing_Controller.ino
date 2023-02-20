#include <Servo.h>

int ledb_pin = 10;
int ledg_pin = 11;
int ledr_pin = 12;

int servo1_pin = 6;
int servo2_pin = 5;

int pyro1_pin  = 10;
int pyro2_pin  = 11;

int pre_pin    = A1;
int main_pin   = 19;

Servo myservo1;
Servo myservo2;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledb_pin, OUTPUT);
  pinMode(ledg_pin, OUTPUT);
  pinMode(ledr_pin, OUTPUT);

  pinMode(pyro1_pin, OUTPUT);
  pinMode(pyro2_pin, OUTPUT);
  
  pinMode(pre_pin, INPUT);
  pinMode(main_pin, INPUT);

  pinMode(servo1_pin, OUTPUT);
  pinMode(servo2_pin, OUTPUT);

  digitalWrite(servo1_pin, HIGH);  

  myservo1.attach(servo1_pin);
  myservo2.attach(servo2_pin);

  myservo1.write(0);
  delay(1000);
  
  digitalWrite(pyro1_pin, LOW);
  digitalWrite(pyro2_pin, LOW);  
  
  Serial.begin(9600);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (digitalRead(pre_pin) == HIGH){

    delay(100);
    long start_time = millis();

    
    while((digitalRead(pre_pin) == HIGH) && (millis() - start_time < 300)){ //500
      
      myservo1.write(55);
     
    }

    // ignites both pyros
    while ((digitalRead(pre_pin) == HIGH) && (millis() - start_time < 1000)){  //1200
      
      digitalWrite(pyro1_pin, HIGH);
      digitalWrite(pyro2_pin, HIGH);
      
    }
      
      digitalWrite(pyro1_pin, LOW);
      digitalWrite(pyro2_pin, LOW);
      
    while(digitalRead(pre_pin) == HIGH){ 
      
      myservo1.write(185);

    }
    
  }

  myservo1.write(0);
  
}
