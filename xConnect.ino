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

// Contribution acceptance information: 
// If you want to contribute to the xConnect family of supported clusters, theese are the requirements you will have to meet to be accepted:
// 1) The License and Logo print have to be intact
// 2) The code has to be easy to read, and must have comments in areas that it is unclear what it is doing. (when sending can codes always try to explain what the code is for)
// 3) Do not remove essential features of this program, such as the data formatting. You may modify it.
// 4) You cannot change the format of the dataPacket ( that comes from xConnect ) that this script reads.
// 5) You cannot change the VERSION without making sure that it works with the specified version of xConnect. They may not be intercompatible! 

// Imports:
#include <mcp_can.h> // This is a custom library, if you do not have it installed, read the README.md in the main branch of the xConnect-hardware repo.
#include <SPI.h>
#include <math.h>

// Configuration:
#define VERSION "1.2" // This is the version of xConnect you are using. Do not touch this if you do not know what your doing!
#define SERIAL_SPEED 115200 // This is the serial console speed. We suggest to leave it as is.
#define DEBUGGER_ENABLED // If you want to disable the debug prompts, comment this line out!
#define SPI_CS_PIN 9 // This is the SPI_CS pin of the CAN BUS interface.
#define OK_BTN_PIN 3 // This is the pin for the OK button
#define UP_BTN_PIN 4 // This is the pin for the UP button
#define DOWN_BTN_PIN 5 // This is the pin for the DOWN button
#define ADAPTED_FOR "EXAMPLE SKETCH" // This is a variable that you need to change, this later on shows in the xConnect Desktop software as the cluster name.

// Create a new MCP_CAN instance with the SPI_CS_PIN defined in the config.
MCP_CAN CAN(SPI_CS_PIN);

// This function runs only once during init of the arduino.
void setup() {
  // Initiate Serial bus and print notice:
  Serial.begin(SERIAL_SPEED);
  DebugPrint("Serial output started with debugging");
  // Logo print
  // You should not alter this part, as this is mostly used for the branding, aswell as displaying the license of the code. This license cannot be changed, due to the fact that it is a Share Alike license. Learn more at: http://creativecommons.org/licenses/by-nc-sa/4.0/
  Serial.println("                  ______                            __     "); // Do not touch
  Serial.println("            _  __/ ____/___  ____  ____  ___  _____/ /_    "); // Do not touch
  Serial.println("           | |/_/ /   / __ \\/ __ \\/ __ \\/ _ \\/ ___/ __/    "); // Do not touch 
  Serial.println("          _>  </ /___/ /_/ / / / / / / /  __/ /__/ /_      ");// Do not touch 
  Serial.println("         /_/|_|\\____/\\____/_/ /_/_/ /_/\\___/\\___/\\__/      "); // Do not touch
  Serial.println("                         Version: V" + String(VERSION) + "\n"); 
  Serial.println("   xConnect © 2022 by InfoX1337 is licensed under CC BY-NC-SA 4.0"); // Do not touch. The license cannot be altered!
  Serial.println("Contact hello@xstudios.one if you want to use xConnect commercially."); // Do not touch
  Serial.println("               To view a copy of this license, visit: "); // Do not touch
  Serial.println("         http://creativecommons.org/licenses/by-nc-sa/4.0/"); // Do not touch

  Serial.println("Adapted for Example Sketch by: InfoX#1337"); // This is where you put yourself, and others that helped you during the adaptation for a cluster, or something else.

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

  // Configure interface buttons. Theese can be used for input, such as buttons to controll something on the cluster.
  pinMode(OK_BTN_PIN, INPUT_PULLUP);
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(DOWN_BTN_PIN, INPUT_PULLUP);
  
  // Cluster custom needle sweep
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
int temp = 100; 
int fuel = 0;
String gear = "P";

// Custom variable definitions:
// Here you define your own variables, try and define them here, so that your code is more readable


void loop() {
  // Data reader loop
  if (Serial.available() > 0) {
    // Example data to send: 4000:100:80:100:N:European:
    // To improve program execution speed, please keep your data formatting, such as converting the rpm's here.
    rpm = Serial.readStringUntil(':').toInt();
    speed = Serial.readStringUntil(':').toInt();
    temp = Serial.readStringUntil(':').toInt();
    fuel = Serial.readStringUntil(':').toInt();
    gear = Serial.readStringUntil(':');
    locale = Serial.readStringUntil(':');
  }

  // Cluster opSends to keep cluster alive and functional

  // This is an example on how to send DATA through the CAN bus interface. If you do not know what is the CAN bus interface, go to our wiki and read the page about CAN bus.
  opSend(0x10, 0x91, 0xFF, 0x51, 0x50, 0x21, 0x00, 0xFF, 0xFF);

  // Button Events
  if (digitalRead(OK_BTN_PIN) == LOW) {
    DebugPrint("ok button pressed!"); // DebugPrint is one of the integrated functions, that allows you to print developer only text, that is disabled by a global variable definition.
    while (digitalRead(OK_BTN_PIN) == LOW) {} // This is how to wait until the button is released (debouncer), some clusters need the buttons to be reset after they have been clicked. This is a good way of doing so
    DebugPrint("ok button released!");
  }
  if (digitalRead(UP_BTN_PIN) == LOW) {
    DebugPrint("up button pressed!");
  }
  if (digitalRead(DOWN_BTN_PIN) == LOW) {
    DebugPrint("down button pressed!");
  }
  
}