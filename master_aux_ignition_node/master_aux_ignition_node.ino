#include <SPI.h>
#include <mcp2515.h>
struct can_frame canMsg1;     //defines can_frame structure -> {ID, DLC, DATA[0], DATA[1], ...,DATA[7] }
struct can_frame canMsg2;


MCP2515 mcp2515(10);                // chipselect pin for uno = 10, mega = 52
void setup() {
  canMsg1.can_id  = 0x026;          // Ignition node ID
  canMsg1.can_dlc = 2;
  canMsg1.data[0] = 0b11111111;
  canMsg1.data[1] = 0b11111111;

  canMsg2.can_id  = 0x036;          //Aux node ID
  canMsg2.can_dlc = 3;
  canMsg2.data[0] = 0b11111111;//01010101
  canMsg2.data[1] = 0b11111111;
  canMsg2.data[2] = 0b11111111;//AA=10101010 // Let the 2nd and 3rd bit to 11, it si acting as a key gor aux node
  Serial.begin(9600);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);                      //sets CAN bitrate to 125kbps
  mcp2515.setNormalMode();
  
  Serial.println("Example: Write to CAN");
}
void loop() {

  mcp2515.sendMessage(&canMsg1);
  delay(10);
  mcp2515.sendMessage(&canMsg2);
  delay(10);
  Serial.println("Messages sent");

}
