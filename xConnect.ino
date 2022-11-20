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


// Imports:
#include <mcp_can.h>
#include <SPI.h>
#include <math.h>
#define lo8(x) ((int) (x)&0xff)
#define hi8(x) ((int) (x)>>8)

// Configuration:
#define VERSION "1.2"
#define SERIAL_SPEED 115200
#define DEBUGGER_ENABLED true
#define SPI_CS_PIN 9
#define OK_BTN_PIN 3


// Create a new MCP_CAN instance with the SPI_CS_PIN defined in the config.
MCP_CAN CAN(SPI_CS_PIN);

// global debugger print function
void DebugPrint(String output) {
  if(DEBUGGER_ENABLED) Serial.println("DEBUG: " + output);
}

// This is the opSend function, it allows you to send CAN BUS codes via the interface.
// The addr is the address of the CAN BUS code.
// The bytes a-h are the data bytes required on CAN BUS.
void opSend(short addr, byte a, byte b, byte c, byte d, byte e, byte f, byte g, byte h) {
  unsigned char data[8] = { a, b, c, d, e, f, g, h };
  CAN.sendMsgBuf(addr, 0, 8, data);
}

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
  Serial.println("                         Version: V" + String(VERSION)+"\n");
  Serial.println("   xConnect © 2022 by InfoX1337 is licensed under CC BY-NC-SA 4.0");
  Serial.println("Contact hello@xstudios.one if you want to use xConnect commercially.");
  Serial.println("               To view a copy of this license, visit: ");
  Serial.println("         http://creativecommons.org/licenses/by-nc-sa/4.0/");
  Serial.println("\nStarting initiation of program...");
  // Try and initiate the CAN BUS controller.
  CAN_INIT:
    if(CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
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
  
  // Ford cluster custom needle sweep
  DebugPrint("Starting needle sweep service...");
  DebugPrint("Waiting for cluster to start...");
  for(int i = 0; i < 500; i++) {
    opSend(0x3b3, 0x45, 0x00, 0x00, 0x8E, 0x00, 0x00, 0x13, 0x00); //Ignition
    opSend(0x04C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Airbag
    opSend(0x3c3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Parking Brake
    opSend(0x416, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //ABS
    opSend(0x466, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Navigation Compass
    opSend(0x156, 100, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);  //Coolant
    opSend(0x877, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x00, 0x00); //Power Steering
    opSend(0x42c, 0x00, 0x00, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x00); //Charging System?
    opSend(0x421, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Motor stuff
    opSend(0x213, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Parking Brake
    opSend(0x3b4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Tire Pressure
    opSend(0x261, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //AWD
    opSend(0x38D, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Key Status
    opSend(0x3B2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Doors, Turnsignals
  } 
  DebugPrint("Cluster started! Starting needle sweep...");
  opSend(0x202, 0x40, 0x00, 0x00, 115, 115, 255, 255, 0x00);
  delay(150);
  opSend(0x204, 0x00, 0x00, 0x00, 115, 255, 255, 0x00, 0x00);
  delay(200);
  opSend(0x156, 180, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);
  delay(500);
  opSend(0x202, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  delay(150);
  opSend(0x204, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  opSend(0x156, 100, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);
  DebugPrint("Needle sweep completed! Starting program...");
  Serial.println("Program starting...");
}

// This is the 'main' function of the code, as its name suggests, it runs in a continuous loop.
// To avoid laggy CAN BUS, or cluster switching off please do not use the delay() here.

// Basic terminology needed when working with xConnect:
// dataPacket - a data packet sent by the xConnect PC software (not customizable). Its format is: RPM:SPEED:TEMP:FUEL:GEARSELECT:LOCALE:
// customProtocol - A custom protocol sent after a dataPacket, that is customizable to fit the developer needs, it can be formatted however you desire.
// LOCALE - There can be two values: European, American (euro -> Celcious and meters / american -> Fahrenheit and mph)

// Variable definitions needed for input code: (do not touch, user definitions below)
String locale = "European"; // default locale is European
int rpm = 0;
int speed = 0; 
int temp = 100; // Temperature values: (100-200) the formula from celcious is: TEMP+90 the formula for fahrenheit is: TEMP-50
int fuel = 0;
String gear = "P";

// Custom variable definitions:
int rpmgate = 96; // <- rpm gate for ford rpm (96-115)
int finetune = 0; // <- rpm fine tuning inside of gate for ford rpm (0-255) 
int blinkerR = B00001000;
int blinkerL = B01000000;
int distance = 0;
byte speedH;

void loop() {
  // Data reader loop
  if(Serial.available() > 0) {
    // Example data to send: 4000:100:80:100:N:European:
    rpm = Serial.readStringUntil(':').toInt();
    speed = Serial.readStringUntil(':').toInt();
    temp = Serial.readStringUntil(':').toInt();
    if(locale == "American") {
      temp=temp-50;
    } else {
      temp=temp+90;
    }
    fuel = Serial.readStringUntil(':').toInt();
    gear = Serial.readStringUntil(':');
    locale = Serial.readStringUntil(':');
  }

  // Cluster opSends to keep cluster alive and functional

  //Ignition
  opSend(0x3b3, 0x45, 0x00, 0x00, 0x8E, 0x00, 0x00, 0x13, 0x00);

  //Airbag
  opSend(0x04C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Parking Brake und Lampen
  opSend(0x3c3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  // Doors, Turnsignals etc.
  opSend(0x3B2, 0x00, 0x00, 0x00, 0x00, blinkerR, 0x00, blinkerL, 0x00);

  // ABS
  opSend(0x416, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Navigation Compass
  opSend(0x466, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Coolant
  opSend(0x156, temp, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);

  //Power Steering
  opSend(0x877, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x00, 0x00);

  //Charging System?
  opSend(0x42c, 0x00, 0x00, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x00); // First byte is tempomat light (0x90)
  
  //Dieselmotor
  opSend(0x421, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); 

  //Parking Brake
  opSend(0x213, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Tire Pressure
  opSend(0x3b4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //AWD
  opSend(0x261, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  //Key Status
  opSend(0x38D, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  
  // RPM
  rpmgate = 96 + (int)(rpm / 500); // Calculate the gate and cast to int
  finetune = (int)(rpm%500)/2;
  if(rpmgate > 115) {
    rpmgate = 115;
  }
  opSend(0x204, 0x00, 0x00, 0x00, rpmgate, finetune, finetune, 0x00, 0x00);

  // Speedometer INOP
  speedH = hi8(speed);
  distance += speed*1.12;
  if(distance > speed*1.12) distance = 0;
  opSend(0x202, 0x40, 0x00, 0x00, 0x00, 255, 255, speedH, 0x00);

  // Gears
  if(gear.equalsIgnoreCase("P")) {
    opSend(0x171, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  } else if(gear.equalsIgnoreCase("R")) {
    opSend(0x171, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24);
  } else if(gear.equalsIgnoreCase("N")) {
    opSend(0x171, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46);    
  } else if(gear.equalsIgnoreCase("D")) {
    opSend(0x171, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64); 
  } else if(gear.equalsIgnoreCase("S")) {
    opSend(0x171, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96);  
  }
  // Button Events
  if(digitalRead(OK_BTN_PIN) == LOW) {
    opSend(0x881, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    while(digitalRead(OK_BTN_PIN) == LOW) {}
    opSend(0x881, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  }

}
