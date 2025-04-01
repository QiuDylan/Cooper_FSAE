//Modified version of CANReceiver by Sandeep Mistry 


const int CTX = 28; //CAN Tx and Rx
const int CRX = 27;
const int OutputPin = 8;//output should go to a relay that turns on shutdown circ

#include <CAN.h>    //does Can start here?
#include <Arduino.h>

//can prob use char instead of integer for memory constraints 

const int TempIndex = 6; //T_Cell_Max
const int VoltageIndex = 4;  //SoC
//Read SoC byte or V_Cell_Min/Max bytes [0,1,2,3]
const int TempMin = 0;  //adjust constants
const int TempMax = 255;
const int VoltageMin = 0;
const int VoltageMax = 255;

char TempByte = 0;
unsigned char VoltageByte = 0;

void setup() {
  pinMode(OutputPin, OUTPUT);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps

  if (!CAN.begin(500E3)) {        //Where does CAN Begin?
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }


    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);
    
    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets

      unsigned char CurrentIndex = 0;
      while (CAN.available()) {
        char CanByte = (char)CAN.read();
        Serial.print(CanByte); //may need to adjust 
        //Serial.print(" ");
        if (CurrentIndex == TempIndex){
          TempByte = CanByte;
        }
        if (CurrentIndex == VoltageIndex){
          VoltageByte = CanByte;          
        }
      }
      
      if (TempVoltCheck()){
        digitalWrite(OutputPin,1);
      }
      
      Serial.println();
    }
  
    Serial.println();
  }
}

char OutOfBounds(char a, char Upper, char Lower){
  if (a > Upper || a < Lower){
    return 0;
  }
  return 1;
}
char TempVoltCheck(){//returns 1 if Voltage or Temp is out of range
  if (OutOfBounds(VoltageByte,VoltageMax,VoltageMin)){
    return 1;
    }
  else if (OutOfBounds(TempByte,TempMax,TempMin)){
    return 1;
    }
  return 0;
}