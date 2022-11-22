
// This is the opSend function, it allows you to send CAN BUS codes via the interface.
// The addr is the address of the CAN BUS code.
// The bytes a-h are the data bytes required on CAN BUS.
void opSend(short addr, byte a, byte b, byte c, byte d, byte e, byte f, byte g, byte h) {
  unsigned char data[8] = { a, b, c, d, e, f, g, h };
  CAN.sendMsgBuf(addr, 0, 8, data);
}