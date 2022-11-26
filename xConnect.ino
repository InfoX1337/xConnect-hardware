//                   ______                            __
//             _  __/ ____/___  ____  ____  ___  _____/ /_
//            | |/_/ /   / __ \/ __ \/ __ \/ _ \/ ___/ __/
//           _>  </ /___/ /_/ / / / / / / /  __/ /__/ /_
//          /_/|_|\____/\____/_/ /_/_/ /_/\___/\___/\__/
//
//   xConnect © 2022 by InfoX1337 is licensed under CC BY-NC-SA 4.0
// Contact hello@xstudios.one if you want to use xConnect commercially.
//                To view a copy of this license, visit:
//          http://creativecommons.org/licenses/by-nc-sa/4.0/
// Special thanks to Vampyr Yannik#0001 for providing the CAN CODES

// Imports:
#include <mcp_can.h>
#include <SPI.h>
#include <math.h>

// Configuration:
#define VERSION "1.3"
#define SERIAL_SPEED 115200
#define DEBUGGER_ENABLED
#define SPI_CS_PIN 9
#define OK_BTN_PIN 3
#define UP_BTN_PIN 4
#define DOWN_BTN_PIN 5
#define ADAPTED_FOR "FORD FIESTA MK8"
#define HANDBRAKE_FLASH_PERIOD 50

// Create a new MCP_CAN instance with the SPI_CS_PIN defined in the config.
MCP_CAN CAN(SPI_CS_PIN);

// This function runs only once during init of the arduino.
void setup() {
  // Initiate Serial bus and print notice:
  Serial.begin(SERIAL_SPEED);
  DebugPrint("Serial output started with debugging");
  // Logo print
  Serial.println("                  ______                            __     ");
  Serial.println("            _  __/ ____/___  ____  ____  ___  _____/ /_    ");
  Serial.println("           | |/_/ /   / __ \\/ __ \\/ __ \\/ _ \\/ ___/ __/    ");
  Serial.println("          _>  </ /___/ /_/ / / / / / / /  __/ /__/ /_      ");
  Serial.println("         /_/|_|\\____/\\____/_/ /_/_/ /_/\\___/\\___/\\__/      ");
  Serial.println("                         Version: V" + String(VERSION) + "\n");
  Serial.println("   xConnect © 2022 by InfoX1337 is licensed under CC BY-NC-SA 4.0");
  Serial.println("Contact hello@xstudios.one if you want to use xConnect commercially.");
  Serial.println("               To view a copy of this license, visit: ");
  Serial.println("         http://creativecommons.org/licenses/by-nc-sa/4.0/");
  Serial.println("\nStarting initiation of program...");
// Try and initiate the CAN BUS controller.
CAN_INIT:
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    DebugPrint("CAN BUS CONTROLLER initiation ok");
  } else {
    DebugPrint("CAN BUS CONTROLLER initiation failed, retrying in 500ms");
    delay(500);
    goto CAN_INIT;
  }
  // Set CAN BUS mode to MCP_ANY
  CAN.setMode(MCP_ANY);

  // Configure interface buttons
  pinMode(OK_BTN_PIN, INPUT_PULLUP);
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(DOWN_BTN_PIN, INPUT_PULLUP);
  // Ford cluster custom needle sweep
  DebugPrint("Starting needle sweep service...");
  opSend(0x42F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0, 0);
  FordSweep();
  DebugPrint("Needle sweep completed! Starting program...");
  Serial.println("Program starting...");
  Serial.println("Welcome to xConnect, adapted for: " + String(ADAPTED_FOR));
}

// This is the 'main' function of the code, as its name suggests, it runs in a continuous loop.
// To avoid laggy CAN BUS, or cluster switching off please do not use the delay() here.

// Basic terminology needed when working with xConnect:
// dataPacket - a data packet sent by the xConnect PC software (not customizable). Its format is: RPM:SPEED:TEMP:FUEL:GEARSELECT:LOCALE:
// customProtocol - A custom protocol sent after a dataPacket, that is customizable to fit the developer needs, it can be formatted however you desire.
// LOCALE - There can be two values: European, American (euro -> Celcious and meters / american -> Fahrenheit and mph)

// Variable definitions needed for input code: (do not touch, user definitions below)
String locale = "European";  // default locale is European
int rpm = 0;
int speed = 0;
int temp = 100;  // Temperature values: (100-200) the formula from celcious is: TEMP+60 the formula for fahrenheit is: TEMP-50
int fuel = 0;
int blinkerR = 0x00;
int blinkerL = 0x00;
int fullBeam = 0x00;
int handbrake = 0x00;
int brakeApplied = 0x00;
bool handbrakeOn = false;
int tc = 0x00;
int oilWarn = 0x00;
int battWarn = 0x00;
int absLight = 0x00;
int tpms = 0x00;
int counter = 0;
String gear = "P";

// Custom variable definitions:
int rpmgate = 96;  // <- rpm gate for ford rpm (96-115)
int finetune = 0;  // <- rpm fine tuning inside of gate for ford rpm (0-255)
int finetuner = 0; // <- speed fine tuning for the ford mk8 clusters
int distance = 0; // something with distance for the speed on the ford mk8 cluster

void readSerial() {
   if (Serial.available() > 0) {
    // Example data to send: S:4000:100:90:100:N:0:0:0:0:0:0:0:0:0:European:
    if(Serial.readStringUntil(':') != "S") {
      Serial.flush();
      return;
    }
    rpm = Serial.readStringUntil(':').toInt();
    if(rpm < 0) {
      rpm = 0;
    }
    rpmgate = 96 + (int)(rpm / 500);  // Calculate the gate and cast to int
    finetune = (int)(rpm % 500) / 2;
    if (rpmgate > 115) {
      rpmgate = 115;
    }
    speed = Serial.readStringUntil(':').toInt();
    int ogSpeed = speed;
    speed  = ( speed / 2.66 ); // for kmph
    finetuner = ((speed * 3) - ogSpeed)*10; // Revisit in future due to inconsistency
    finetuner = finetuner - finetuner%100;    
    //DebugPrint("Speed = " + String(speed) + " finetuner? " + String(finetuner)) ;
    temp = Serial.readStringUntil(':').toInt();
    if (locale == "American") {
      temp = temp - 50;
    } else {
      temp = temp + 60;
    }
    fuel = Serial.readStringUntil(':').toInt();
    gear = Serial.readStringUntil(':');
    if(Serial.readStringUntil(':') == "1") {
      blinkerL = B11111100;
    } else {
      blinkerL = B10111100;
    }
    if(Serial.readStringUntil(':') == "1") {
      blinkerR = B00001000;
    } else {
      blinkerR = 0x00;
    }
    if(Serial.readStringUntil(':') == "1") {
      fullBeam = B00110000;
    } else {
      fullBeam = 0x00;
    }
    if(Serial.readStringUntil(':') == "1") {
      handbrakeOn = true;
    } else {
      handbrakeOn = false;
    }
    if(Serial.readStringUntil(':') == "1") {
      tc = B00000010;
    } else {
      tc = 0x00;
    }
    if(Serial.readStringUntil(':') == "1") {
      oilWarn = B00001000;
    } else {
      oilWarn = 0x00;
    }
    if(Serial.readStringUntil(':') == "1") {
      battWarn = B00001000;
    } else {
      battWarn = 0x00;
    }
    if(Serial.readStringUntil(':') == "1") {
      absLight = B01000000;
    } else {
      absLight = 0x00;
    }
    if(Serial.readStringUntil(':') == "1") {
      tpms = B00000100;
    } else {
      tpms = 0x00;
    }
    locale = Serial.readStringUntil(':');
  }
}

void loop() {
  // Data reader loop
  
  readSerial();

  // Cluster opSends to keep cluster alive and functional

  // Keep alive signal
  opSend(0x591, 0x91, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

  //Ignition
  opSend(0x3B2, B01000000, 0x00, 0x00, B1000000, blinkerR, 0x00, blinkerL, 0x00);
  // Byte 4 B00000001 means dim, B0000100 means less dim, B0001000 means even less dim B0010000 means full brightness 
  // Last byte: B00000001 fog lights
  // First byte: B01000010 fog light 2, B01000100 means day lights, B01000000 screen on, B10000000 screen off, B01000001 door ajar light
  
  //Odometer and Service
  opSend(0x179, 0x00, 0x00, 0x00, 0x00, B00010000, 0x00, 0x00, 0x00);
  //Physical Wrench Light disappears, "See Manual" persist if nothing else sent

  //Coolant
  opSend(0x156, temp, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);
  //Byte 1: Temperature

  //Airbag
  opSend(0x04C, 0x00, B01010101, B01010101, 0x00, 0x00, 0x00, 0x00, 0x00);
  //Byte 2&3: 85=All Seatbelts applied (5)

  //Parking Brake (Fusion 2013)
  opSend(0x213, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  if(speed > 5 && handbrakeOn && counter < HANDBRAKE_FLASH_PERIOD) {
    handbrake = B00010000;
    brakeApplied = B11111111;
    counter++;
  } else if(speed > 5 && handbrakeOn && counter < HANDBRAKE_FLASH_PERIOD * 2) {
    handbrake = 0x00;
    brakeApplied = B11111111;
    counter++;
  } else if(counter >= HANDBRAKE_FLASH_PERIOD * 2) {
    counter = 0;
  } else if(handbrakeOn) {
    handbrake = B00010000;
    brakeApplied = 0x00;
  } else {
    handbrake = 0x00;
    brakeApplied = 0x00;
  }
  //Vehicle Alarm + Parking Brake
  opSend(0x3C3, 0x00, B11111111, handbrake, brakeApplied, 0x00, 0x00, 0x00, 0x00);
  //Bytes 1/2: B11111111=Alarm
  //Byte 3: B00010000=Park Brake

  //ABS
  opSend(0x416, 0x0E, 0x00, 0x00, 0x00, 0x00, tc, 0x00, 0x00);
  //Byte 6: B00000100=Slow flashing TC, B00000010=Solid TC Light
  //Byte 7: B10000000=Rapid flashing ABS, B01000000=Solid ABS

  //TPMS
  opSend(0x3b4, tpms, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  //Byte 1: 49=Shows Low Tire Pressure Menu, B00000100 shows still tire warning, B00001000 shows flashing tire warning
  //Byte 2: Highlights Front R and L tires
  //Byte 3: Same as above but rear

  //ECU
  opSend(0x421, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  //Byte 1: B00001000=Flashing CEL, B00000100=Solid CEL

  //Charging System
  opSend(0x42C, 0x00, 0x00, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x00);
  //Byte 2: 17=Charge Light (No Message on Screen)

  //Power Steering
  opSend(0x877, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Language?
  opSend(0x191, 65, 17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  //Byte 1: Units; 65=KM and CELSIUS
  //Byte 2: 17=English, 35=German

  //Outside Temp
  opSend(0x3B3, 0x00, 0x00, 0x00, 0x00, 125, 0x00, 0xFF, 0x00);
  //Byte 5: Outside Temp Value -> 125=22°C

  //Front Camera
  opSend(0x3D8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Tractioncontrol
  opSend(0x46A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //High Beam + Auto Hed
  opSend(0x46B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, fullBeam);
  //Byte 8: B00110000=High Beam
  
  // RPM
  opSend(0x42F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, rpmgate, finetune);

  // Speedometer INOP
  opSend(0x202, 0x40, 0x00, 0x00, 0x00, distance, distance, speed, finetuner);

  distance++;
  if(distance > 200) { // Make sure the distance doesnt get out of range
    distance = 0;
  }

  // Gears do not exist on this cluster so this section is empty

  // Button Events
  if (digitalRead(OK_BTN_PIN) == LOW) {
    opSend(0x082, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    
  } else if (digitalRead(UP_BTN_PIN) == LOW) {
    opSend(0x082, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  } else if (digitalRead(DOWN_BTN_PIN) == LOW) {
    opSend(0x082, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  } else {
    opSend(0x082, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  }
  //opSend(0xc2d, 20, 20, 20, 20, 20, 20, 20, 20); // Power reduced to lower engine temp
  // 130 - 140
  // 170-180
  // Charging system blinking light
  // byte data = B00000001;
  // // Charging system
  // opSend(0xc2c, 0x00, data, data, data, 0x00, 0x00, 0x00, 0x00);
  //Experimental
  if(false) {
    byte toCheck = 200;
    for(int x = 0; x < 1000; x++) {
      //for(int y = 0; y < 256; y++) {
        opSend(0x591, 0x91, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF); // Keep alive signal
        opSend(0x3B2, 0x45, 0x00, 0x00, 0x8E, B00000000, 0x00, B00000000, 0x00); //Ignition
        opSend(0x179, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00); //Odometer and Service
        opSend(0x156, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00); //Coolant
        opSend(0x04C, 0x00, 85, 85, 0x00, 0x00, 0x00, 0x00, 0x00); //Airbag
        opSend(0x213, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Parking Brake (Fusion 2013)
        opSend(0x3C3, 0x00, B11111111, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);  //Vehicle Alarm + Parking Brake 
        opSend(0x416, 0x0E, 0x00, 0x00, 0x00, 0x00, B00000000, B00000000, 0x00); //absLight  
        opSend(0x3b4, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //TPMS
        opSend(0x421, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //ECU 
        opSend(0x42C, 0x00, 0x00, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x00); //Charging System  
        opSend(0x877, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Power Steering   
        opSend(0x191, 65, 17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Language?   
        opSend(0x3B3, 0x00, 0x00, 0x00, 0x00, 100, 0x00, 0xFF, 0x00); //Outside Temp
        opSend(0x3D8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Front Camera
        opSend(0x46A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Tractioncontrol
        opSend(0x46B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, B00000000); //High Beam + Auto Hed
        //opSend(toCheck, y, y, y, y, y, y, y, y);
        //DebugPrint("Checking byte: " + String(toCheck) + " and y: " + String(y));
        opSend(toCheck, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        DebugPrint("Checking byte: " + String(toCheck) + " X: " + String(x));
        //129 //
        delay(100);
      //}
      toCheck++;
    }
  }
  
}
