#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <ZumoMotors.h>
#include "SoftwareSerial.h"
#include <ZumoBuzzer.h>


#define LED_PIN 13
#define MZ80_PIN 6
#define NUM_SENSORS 6
SoftwareSerial MyBlue(2, 4);

//Pushbutton button(ZUMO_BUTTON);
ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;

// Define an array for holding sensor values.
unsigned int sensorValues[NUM_SENSORS];
unsigned int positionVal = 0;
unsigned long BT_check;

bool lineV;
int objCount;


char bluetoothData; // 
long memoryData; 
  //planned format: 1400400
  //                2250250
  // first: 1 means manual, 2 means auto
  // last 6 digits determine speed.


void setup() {

  lineV = false;
  objCount = 0;
  
  MyBlue.begin(9600);
  MyBlue.println("Starting...");
   pinMode(LED_PIN, OUTPUT);
   digitalWrite(LED_PIN, HIGH);
   
  
  // --------------------------- Start Of The Calibration -------------------------
  MyBlue.println("test 1");
  reflectanceSensors.init();
  unsigned long startTime = millis();
  /*
  while (millis() - startTime < 5000)  // make the calibration take 10 seconds
  {
    reflectanceSensors.calibrate();
  }
  */
  // ------------------- End Of The Calibration -----------------------

  MyBlue.println("Calibration done");

  BT_check = millis();


                    
  
}

unsigned int mostLeftSensor() {
  if (sensorValues[0] < 600)
    return 1;
  else
    return 0;
}

unsigned int leftSensor() {
  if (sensorValues[1] < 600)
    return 1;
  else
    return 0;
}

unsigned int midLeftSensor() {
  if (sensorValues[2] < 600)
    return 1;
  else
    return 0;
}

unsigned int midRightSensor() {
  if (sensorValues[3] < 600)
    return 1;
  else
    return 0;
}

unsigned int rightSensor() {
  if (sensorValues[4] < 600)
    return 1;
  else
    return 0;
}

unsigned int mostRightSensor() {
  if (sensorValues[5] < 600)
    return 1;
  else
    return 0;
}

void turnRight() {
  motors.setSpeeds(150, -150);
}

void yolo(int a, int b){
  motors.setSpeeds(a, b);
}

void go() {
  motors.setSpeeds(400, 400);
}

bool objDetect;

int turnBacks = 0;
bool forceLoop;

void autoSearch(bool lineL){
  if (lineL){
    //geri git
    motors.setSpeeds(-200, -200);
    delay(100);
    turnBacks = turnBacks + 1;
    //geri gittikten sonra objeyi arat
    objDetect = false;
    while (!objDetect){
      turnRight();
      if(!digitalRead(MZ80_PIN)){
        objDetect = true;
        digitalWrite(LED_PIN, HIGH);
        objCount = objCount+1;
      }
      turnBacks = 0;
    }
    if (turnBacks == 10){
      //obje bulamadan 10 kere ileri geri yaptı.
      turnRight();
      //WYSI OMG COOKIEZI REFERENCE
      delay(727);
    }
  }
   
  //eğer en sağ ve en solda beyaz çizgi yoksa
  else {
    //eğer obje varsa
    if (!digitalRead(MZ80_PIN)){
      //ilerle
      objCount = objCount+1;
      if (objDetect){
        objCount = objCount - 1;
      }
      objDetect = true; 
      go();
    }
    //obje yoksa
    else{
      //sağa dön
      turnRight();
    } 
  }
  //eğer bir obje yoksa
  while (digitalRead(MZ80_PIN)) {
    //sağa dön
    objDetect = false;
    turnRight();
  }
}

void loop() {
  
  positionVal = reflectanceSensors.readLine(sensorValues);
   //from bluetooth to Terminal. 
  //MyBlue.println(727);
  if (mostLeftSensor() == 1 || mostRightSensor() == 1) {
    lineV = true;
  }
  /*
  if(MyBlue.available()){
    Serial.write(MyBlue.read());
  }
  if(Serial.available()){
    MyBlue.write(Serial.read());
  }*/
  
  if (millis() > BT_check + 7500){
    //Bluetooth not connected for 15 seconds. Connection down, FORCE AUTO MODE
    while (true){
      
      if (mostLeftSensor() == 1 || mostRightSensor() == 1) {
       lineV = true;
      }
      autoSearch(lineV);
    }
  }
   while (MyBlue.available()){
   bluetoothData = MyBlue.read();
   if (millis() > BT_check + 7500){
    //Bluetooth interrupted for 15 seconds. Connection down, FORCE AUTO MODE
    while (true){
      if (mostLeftSensor() == 1 || mostRightSensor() == 1) {
       lineV = true;
      }
      autoSearch(lineV);
    }
   }
   }
   int cter = 0;
   unsigned long asdf = millis();
   while (MyBlue.read() != '\n' && MyBlue.read() != NULL){
    memoryData = memoryData + bluetoothData*(10^cter);
    bluetoothData = MyBlue.read();
    MyBlue.println("Recieving data first loop");
    MyBlue.println(bluetoothData);
    cter = cter+1;
    if (cter > 15){
      while (true){
      if (mostLeftSensor() == 1 || mostRightSensor() == 1) {
       lineV = true;
      }
      autoSearch(lineV);
    }
      
    }
   }
   
   //                 1000000
   if(memoryData > 999999 && memoryData < 2400401){
    int automanual = memoryData / 1000000;
   
   while (automanual == 1 && lineV == false){
    autoSearch(lineV);
    if (MyBlue.available()){
      while (bluetoothData != '\n' && bluetoothData !=  NULL){
      //MyBlue.println("Recieving data second loop");
      memoryData = memoryData + bluetoothData*(10^cter);
      bluetoothData = MyBlue.read();
      cter = cter+1;
      }
   
    }
   }
   while(automanual == 2){
    int leftSpeed = (memoryData % 1000000)/1000;
    int rightSpeed = (memoryData % 1000000) % 1000;
    if (objDetect == false){
      yolo(leftSpeed, rightSpeed);
      if (!digitalRead(MZ80_PIN)){
        objDetect = true;
        objCount = objCount+1;
      }  
    }
    while (objDetect){
      if (mostLeftSensor() == 0 && mostRightSensor() == 0){
        go();
      }
      else{
        yolo(0,0);
      }
      if (MyBlue.read() != NULL){
        memoryData = MyBlue.read();
        automanual = memoryData / 1000000;
    }
    }
    }
   }

   if (bluetoothData == 10){
    //blink mode
    int tmp = objCount;
    while (tmp != 0){
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
      tmp = tmp - 1;
    }
   }
   
  //en sağ veya en solda beyaz çizgi bulunursa
  
  
}
