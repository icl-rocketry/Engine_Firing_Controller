#include <Servo.h>

int ledb_pin = 10;
int ledg_pin = 11;
int ledr_pin = 12;

int servo1_pin = 6;    //fuel
int servo2_pin = 5;    //oxidiser

int pyro1_pin  = 7;

int pyro2_pin  = 8;

int engine_fire = A1;

int chamber_pressure_pin = A5;
int fuel_injector_pressure_pin = A7; 

float chamberP;
float fuelP;
float demandFuelP;
float error;
float FuelServoAnglePrev = 185;
float FuelServoAngleDemand;
const int Kp = 2.5;
float ptcalgrad = 0.1363;
float ptcalconst = -13.008;

Servo fuelServo;
Servo oxidiserServo;

void setup() {

  pinMode(ledb_pin, OUTPUT);
  pinMode(ledg_pin, OUTPUT);
  pinMode(ledr_pin, OUTPUT);

  pinMode(pyro1_pin, OUTPUT);
  pinMode(pyro2_pin, OUTPUT);
  
  pinMode(engine_fire, INPUT);

  pinMode(chamber_pressure_pin, INPUT);
  pinMode(fuel_injector_pressure_pin, INPUT);

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
  
      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst;      //returns fuel pressure in bar

    }

    //move valves to pre position
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1300)){
      
      fuelServo.write(60);
      delay(50);
      oxidiserServo.write(90);      
      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst;      //returns fuel pressure in bar

    }

    //valves to full bore
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1600)){

      fuelServo.write(185);
      delay(50);
      oxidiserServo.write(185);
        
      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst;      //returns fuel pressure in bar

    }

    //wait for steady state
    delay(500);

    //main operating loop, end when button is released or duration exceeds 10 seconds
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 10000)){ 

      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = ((analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst);      //returns fuel pressure in bar
      if (chamberP > 10){  //(chamberP > 23) && (chamberP < 17) && (fuelP < 30) && (fuelP > 50)){  //condition for abnormal operation, can make them tighter or wider
        demandFuelP = 0.016 * pow(chamberP,2) + chamberP;    //function relating demanded fuel pressure to chamber pressure
        error = demandFuelP - fuelP;
        FuelServoAngleDemand = FuelServoAnglePrev + Kp * error;        //Any PID controller may work
        if (FuelServoAngleDemand > 185){
          fuelServo.write(185);               //check whether this is teh correct servo
          FuelServoAnglePrev = 185;
        }
        else if (FuelServoAngleDemand < 90){
          fuelServo.write(90);
          FuelServoAnglePrev = 90;
        }
        else{
          fuelServo.write(FuelServoAngleDemand);
          FuelServoAnglePrev = FuelServoAngleDemand;
        }
      }
      
      delay(50); //delay to give the servo chance to start moving

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
