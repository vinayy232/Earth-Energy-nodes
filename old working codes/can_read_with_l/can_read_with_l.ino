#include <SPI.h>                  // Required for mcp2515
#include <Wire.h>                 // Support for adafruit mcp23017 library
#include <mcp2515.h>
#include <Adafruit_MCP23017.h>    //defines mcp.read() like functions
struct can_frame canMsg;          //defines can_frame structure -> {ID, DLC, DATA[0], DATA[1], ...,DATA[7] }
MCP2515 mcp2515(10);              // chipselect pin for uno = 10, mega = 52
String msg0 = "";                 //message 0 , Binary message of Data[0] recieved from master
String msg1 = "";                 //message 1 , Binary message of Data[1] recieved from master
String msg2 = "";                 //message 2 , Binary message of Data[2] recieved from master
String sw1;                       //stores two bits for switch 1
String sw2;                       //stores two bits for switch 2
String sw3;
String sw4;
String sw5;
String sw6;
String sw7;
String sw8;
String sw9;                       //stores two bits for switch 9

int MCP_relay[]={0,1,2,3,4,5,6,7,5};          //output port of mcp23017 (port A)
int MCP_INPUT[]= {8,9,10,11,12,13,14,15,7};   //input port of mcp23017 (port B)
int switch_out_stat[] = {1,1,1,1,1,1,1,1,1};    //output state array such that state changes only after completion of one loop
int switch_in_stat[] = {0,0,0,0,0,0,0,0,0};     //input state array updated continuously by switches  
Adafruit_MCP23017 mcp;
void setup() {
  Serial.begin(9600);
  mcp.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);            //sets CAN bitrate to 125kbps
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
  pinMode(5, OUTPUT);                         //sets arduino digital pin 5 to output
  pinMode(7, INPUT);                         //sets arduino digital pin 7 to input
  digitalWrite(5, LOW);                      //sets pin 5 to LOW initially
  
  for(int i = 0;i<8;i++){
    mcp.pinMode(MCP_relay[i], OUTPUT);       //sets mcprelay port A as output pins
    mcp.digitalWrite(MCP_relay[i], LOW);     // sets port A output to LOW
  }
  
  for(int i = 0;i<8;i++){
    mcp.pinMode(MCP_INPUT[i],INPUT);   // sets mcp_input port B as input pins
    mcp.pullUp(MCP_INPUT[i],HIGH);     // Puled high to ~100k  
  }
  

}
void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg.can_dlc; i++)  
    {  
      Serial.print(canMsg.data[i]);   // print the data
      Serial.print(" ");
    }
    Serial.println();
      
    if(canMsg.can_id==0x036)
    {

    msg0 = String(canMsg.data[0], BIN);     //update msg0 with string data 0
    msg1 = String(canMsg.data[1], BIN);     //update msg1 with string data 1
    msg2 = String(canMsg.data[2], BIN);     //update msg2 with string data 2
    msg0 = adjustLength(msg0);              //adjustment of length is neccessary , when we convert any hex message to binary string
    msg1 = adjustLength(msg1);              // if hex value is 0F then binary string will be 1111 but it should be 00001111   
    msg2 = adjustLength(msg2);              // so to adjust the length to 8 adjustLength function is used
//    Serial.println(msg0);
//    Serial.println(msg1);
//    Serial.println(msg2);
    
    sw1 = msg0.substring(0,2);              //sw1 = two bits from msg0
    sw2 = msg0.substring(2,4);
    sw3 = msg0.substring(4,6);
    sw4 = msg0.substring(6,8);
    sw5 = msg1.substring(0,2);              //sw5 = two bits from msg1              
    sw6 = msg1.substring(2,4);
    sw7 = msg1.substring(4,6);
    sw8 = msg1.substring(6,8);
    sw9 = msg2.substring(0,2);              //sw9 = two bits from msg2
 
    check(sw1,0);                           //see function description of check  
    //check(sw2,1);
    check(sw3,2);
    check(sw4,3);
    check(sw5,4);
    check(sw6,5);
    check(sw7,6);
    //check(sw8,7);
    //checkNorm(sw9,8);
    //check_pt(sw8,sw9,7,8);
    check_mcp_switch();
    udp(sw8, sw9, sw2, 1, 7, 8);
    check(sw8,7);
    checkNorm(sw9,8);

    }
  }    
  }
  // check function take switch string and outputs to the corresponding relay pin as per the decision made by switch string
  //11= Manual control ,10 = switch ON, 01 = switch OFF
void check(String switch1,int mcpLed){
  if(switch1 == "11"){
    //Serial.println("Manual Control");
    mcp.digitalWrite(MCP_relay[mcpLed],!switch_out_stat[mcpLed]);
  }
  else if(switch1 == "10"){
    //Serial.println("Switch ON");
    mcp.digitalWrite(MCP_relay[mcpLed],HIGH);
  }
  else if(switch1 == "01"){
    //Serial.println("Switch OFF");
    mcp.digitalWrite(MCP_relay[mcpLed],LOW);
  }
  else{
    delay(1);
  }
}
// checkNorm function take switch string and outputs to the corresponding relay pin as per the decision made by switch string
  //11= Manual control ,10 = switch ON, 01 = switch OFF
  //only difference between check and checkNorm is that here in checkNorm, digitalwrite to arduino pins is done while in check 
  // mcp.digitalWrite is done at the output port of mcp23017
void checkNorm(String switch1,int mcpLed){
  if(switch1 == "11"){
    Serial.println("Manual Control");
    digitalWrite(MCP_relay[mcpLed],!switch_out_stat[mcpLed]);
  }
  else if(switch1 == "10"){
    Serial.println("Switch ON");
    digitalWrite(MCP_relay[mcpLed],HIGH);
  }
  else if(switch1 == "01"){
    Serial.println("Switch OFF");
    digitalWrite(MCP_relay[mcpLed],LOW);
  }
  else{
    delay(1);
  }
}
//adjustLength function adjust each input string length to 8
String adjustLength(String message){
  while(message.length() <= 7){
    message = 0 + message;
  }
  return(message);
}
//check_mcp_switch function read the switch status and updates to switch_out_stat array
void check_mcp_switch(){
  for(int i=0;i<8;i++){
    switch_in_stat[i]=  mcp.digitalRead(MCP_INPUT[i]); 
  }
  switch_in_stat[8]= digitalRead(MCP_INPUT[8]);
  for(int i=0;i<9;i++){
      switch_out_stat[i] = switch_in_stat[i];
      Serial.print(switch_out_stat[i]);
   }
   Serial.println("");
  delay(100);
}
void check_pt(String toggle, String pass, int ud_pin, int pass_pin){
  if (pass == "11" && toggle == "11"){
    if(switch_out_stat[pass_pin] == switch_out_stat[ud_pin]){
       digitalWrite(MCP_relay[pass_pin],HIGH);
       mcp.digitalWrite(MCP_relay[ud_pin],LOW);
    }
    else{
      digitalWrite(MCP_relay[pass_pin],LOW);
      mcp.digitalWrite(MCP_relay[ud_pin],HIGH); 
    }
  }
  else{
    delay(1);
    Serial.println("something wrong");
//      checkNorm(sw9,8);
//      check(sw8,7);
  }
}
//udp(sw8, sw9, sw2, 1, 7, 8);
void udp(String ud, String pass, String head, int head_pin, int ud_pin, int pass_pin){
  if(pass == "11" && ud == "11" && head == "11"){
    if((!switch_out_stat[head_pin] && switch_out_stat[ud_pin] && switch_out_stat[pass_pin]) || (switch_out_stat[head_pin] && !switch_out_stat[ud_pin] && !switch_out_stat[pass_pin]) || (!switch_out_stat[head_pin] && !switch_out_stat[ud_pin] && !switch_out_stat[pass_pin])){
//      digitalWrite(MCP_relay[pass_pin],HIGH);
//      mcp.digitalWrite(MCP_relay[ud_pin],LOW);
      switch_out_stat[pass_pin] = 0;
      switch_out_stat[ud_pin] = 1;
    }
    else if((!switch_out_stat[head_pin] && !switch_out_stat[ud_pin] && switch_out_stat[pass_pin]) || (switch_out_stat[head_pin] && switch_out_stat[ud_pin] && !switch_out_stat[pass_pin]) || (!switch_out_stat[head_pin] && switch_out_stat[ud_pin] && !switch_out_stat[pass_pin])){
//      digitalWrite(MCP_relay[pass_pin],LOW);
//      mcp.digitalWrite(MCP_relay[ud_pin],HIGH);
      switch_out_stat[pass_pin] = 1;
      switch_out_stat[ud_pin] = 0;  
    }
    else{
//      digitalWrite(MCP_relay[pass_pin],LOW);
//      mcp.digitalWrite(MCP_relay[ud_pin],LOW);
      switch_out_stat[pass_pin] = 1;
      switch_out_stat[ud_pin] = 1;  
    }
      
  }
}
