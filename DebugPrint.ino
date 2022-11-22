// This is the included DebugPrint function, it is a simple debugger system used by the developers of xConnect
void DebugPrint(String output) {
  #ifdef DEBUGGER_ENABLED;
  Serial.println("DEBUG: " + output);
  #endif
}