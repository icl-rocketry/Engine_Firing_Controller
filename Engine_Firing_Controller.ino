#include <Servo.h>
#include <SPI.h>
#include <SD.h>

File myFile;
const int chipSelect = 10;

int ledb_pin = 10;
int ledg_pin = 11;
int ledr_pin = 12;

int servo1_pin = 6;
int servo2_pin = 5;

int pyro1_pin  = 10;
int pyro2_pin  = 11;

int engine_fire = A1;

int chamber_pressure_pin = A2;
int fuel_injector_pressure_pin = A3; 

String dataString;
float chamberP;
float fuelP;
float demandFuelP;
float error;
float FuelServoAnglePrev = 185;
float FuelServoAngleDemand;
const int Kp = 5;

Servo myservo1;
Servo myservo2;

void setup() {

  SD.begin();
  
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
    myFile = SD.open("EngineConrollerData.txt", FILE_WRITE);
    delay(100);
    long start_time = millis();

    // ignites both pyro channels
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 500)){
      digitalWrite(pyro1_pin, HIGH);
      digitalWrite(pyro2_pin, HIGH);
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      myFile.println(dataString);

    }

    //move valves to pre position
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1300)){
      
      myservo1.write(60);
      myservo2.write(90);
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      myFile.println(dataString);

    }

    //valves to full bore
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1600)){

      myservo1.write(185);
      myservo2.write(185);
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      myFile.println(dataString);

    }

    //wait for steady state
    delay(500);

    //main operating loop, end when button is released or duration exceeds 10 seconds
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 10000)){ 

      chamberP = (analogRead(chamber_pressure_pin)/1024)*100;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)/1024)*100;      //returns fuel pressure in bar
      if ((chamberP > 33) && (chamberP < 27) && (fuelP > 43) && (fuelP < 37)){  //condition for abnormal operation, can make them tighter or wider
        demandFuelP = 0.016 * pow(chamberP,2) + chamberP;    //function relating demanded fuel pressure to chamber pressure
        error = demandFuelP - fuelP;
        FuelServoAngleDemand = FuelServoAnglePrev + Kp * error;        //Any PID controller may work
        if (FuelServoAngleDemand > 185){
          myservo1.write(185);               //check whether this is teh correct servo
          FuelServoAnglePrev = 185;
        }
        else if (FuelServoAngleDemand < 90){
          myservo1.write(90);
          FuelServoAnglePrev = 90;
        }
        else{
          myservo1.write(FuelServoAngleDemand);
          FuelServoAnglePrev = FuelServoAngleDemand;
        }
      }
      
      delay(50); //delay to give the servo chance to start moving
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      myFile.println(dataString);
    }

    

    myservo1.write(0);
    myservo2.write(0);
    digitalWrite(pyro1_pin, LOW);
    digitalWrite(pyro2_pin, LOW);

     while(digitalRead(engine_fire) == HIGH){ 

    }

  }
  myFile.close();
}
