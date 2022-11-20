void DebugPrint(String output) {
  #ifdef DEBUGGER_ENABLED;
  Serial.println("DEBUG: " + output);
  #endif
}