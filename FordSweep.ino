
void FordSweep() {
  DebugPrint("Waiting for cluster to start...");
  DebugPrint("Starting prefab");
  for(int i = 0; i < 50; i++) {
    opSend(0x591, 0x91, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    opSend(0x3B2, 0x00, 0x00, 0x00, 0x00, B00000000, 0x00, B00000000, 0x00);
  }
  DebugPrint("Prefab ended! Starting normally");
  for(int i = 0; i < 100; i++) {
    opSend(0x591, 0x91, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF); // Keep alive signal
    opSend(0x3B2, B01000000, 0x00, 0x00, 0x8E, B00000000, 0x00, B00000000, 0x00); //Ignition
    opSend(0x179, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00); //Odometer and Service
    opSend(0x156, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00); //Coolant
    opSend(0x04C, 0x00, B01010101, B01010101, 0x00, 0x00, 0x00, 0x00, 0x00); //Airbag
    opSend(0x213, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Parking Brake (Fusion 2013)
    opSend(0x3C3, 0x00, B11111111, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);  //Vehicle Alarm + Parking Brake 
    opSend(0x416, 0x0E, 0x00, 0x00, 0x00, 0x00, B00000000, B00000000, 0x00); //ABS  
    opSend(0x3b4, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //TPMS
    opSend(0x421, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //ECU 
    opSend(0x42C, 0x00, 0x00, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x00); //Charging System  
    opSend(0x877, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Power Steering   
    opSend(0x191, 65, 17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Language?   
    opSend(0x3B3, 0x00, 0x00, 0x00, 0x00, 100, 0x00, 0xFF, 0x00); //Outside Temp
    opSend(0x3D8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Front Camera
    opSend(0x46A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //Tractioncontrol
    opSend(0x46B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, B00000000); //High Beam + Auto Hed
  } 
  DebugPrint("Cluster started! Starting needle sweep...");
  opSend(0x42F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 115, 255);
  opSend(0x202, 0x00, 0x00, 0x00, 100, 100, 100, 95, 200); 
  opSend(0x156, 189, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);
  delay(1000);
  opSend(0x156, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00);
  opSend(0x42F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  opSend(0x202, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01);
}