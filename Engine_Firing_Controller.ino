#include <Servo.h>
#include <SPI.h>
#include <SD.h>

File myFile;

Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;

int ledb_pin = 10;
int ledg_pin = 11;
int ledr_pin = 12;

int servo1_pin = 6;    //fuel
int servo2_pin = 5;    //oxidiser

int pyro1_pin  = 10;

int pyro2_pin  = 11;

int engine_fire = A1;

int chamber_pressure_pin = A5;
int fuel_injector_pressure_pin = A7; 

String dataString;
float chamberP;
float fuelP;
float demandFuelP;
float error;
float FuelServoAnglePrev = 185;
float FuelServoAngleDemand;
const int Kp = 5;
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
  
  // Open //Serial communications and wait for port to open:
  //Serial.begin(9600);
//  while (!//Serial) {
//    ; // wait for //Serial port to connect. Needed for native USB port only
//  }

  //Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    //Serial.println("initialization failed. Things to check:");
    //Serial.println("* is a card inserted?");
    //Serial.println("* is your wiring correct?");
    //Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    //Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  //Serial.println();
  //Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      //Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      //Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      //Serial.println("SDHC");
      break;
    default:
      //Serial.println("Unknown");
      break;
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    //Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  //Serial.print("Clusters:          ");
  //Serial.println(volume.clusterCount());
  //Serial.print("Blocks x Cluster:  ");
  //Serial.println(volume.blocksPerCluster());

  //Serial.print("Total Blocks:      ");
  //Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  //Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  //Serial.print("Volume type is:    FAT");
  //Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  //Serial.print("Volume size (Kb):  ");
  //Serial.println(volumesize);
  //Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  //Serial.println(volumesize);
  //Serial.print("Volume size (Gb):  ");
  //Serial.println((float)volumesize / 1024.0);

  //Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop() {
  
  if (digitalRead(engine_fire) == HIGH){
    delay(100);
    myFile = SD.open("EngineConrollerData.txt", FILE_WRITE);
    long start_time = millis();
    
    // ignites both pyro channels
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 500)){
      digitalWrite(pyro1_pin, HIGH);
      digitalWrite(pyro2_pin, HIGH);


      
      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst;      //returns fuel pressure in bar
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      //Serial.println(dataString);
      myFile.println(dataString);

    }

    //move valves to pre position
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1300)){
      
      fuelServo.write(60);
      oxidiserServo.write(90);


      
      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst;      //returns fuel pressure in bar
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      //Serial.println(dataString);
      myFile.println(dataString);

    }

    //valves to full bore
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 1600)){

      fuelServo.write(185);
      oxidiserServo.write(185);


      
      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = (analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst;      //returns fuel pressure in bar
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      //Serial.println(dataString);
      myFile.println(dataString);

    }

    //wait for steady state
    delay(500);

    //main operating loop, end when button is released or duration exceeds 10 seconds
    while((digitalRead(engine_fire) == HIGH) && (millis() - start_time < 10000)){ 

      chamberP = (analogRead(chamber_pressure_pin)*ptcalgrad)+ptcalconst;         //returns chamber pressure in bar
      fuelP = ((analogRead(fuel_injector_pressure_pin)*ptcalgrad)+ptcalconst);      //returns fuel pressure in bar
      ////Serial.println(analogRead(fuel_injector_pressure_pin));
      if (chamberP > 2){  //(chamberP > 23) && (chamberP < 17) && (fuelP < 30) && (fuelP > 50)){  //condition for abnormal operation, can make them tighter or wider
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

      
      dataString = String(millis()) + "," + String(chamberP) + "," + String(fuelP) + "," + String(demandFuelP) + "," + String(FuelServoAngleDemand);
      //Serial.println(dataString);
      myFile.println(dataString);
    }

    

    fuelServo.write(0);
    oxidiserServo.write(0);
    digitalWrite(pyro1_pin, LOW);
    digitalWrite(pyro2_pin, LOW);

     while(digitalRead(engine_fire) == HIGH){ 

    }

  }
  myFile.close();
}
