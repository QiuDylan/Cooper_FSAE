#include <Arduino.h>
#include <SPI.h>

//Core definitions
static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;

//Chip select pins
const uint8_t CS_1 = 10;
const uint8_t CS_2 = 11;

//Pin definitions
int APPS_1 = 34;            //Analog input: sensor 1, pin 34 is input only
int APPS_2 = 35;            //Analog input: sensor 2, pin 35 is input only
int THROTTLE = 32;          //Analog output: ratio between sensors or 0 if implausible
int TRACTIVE_SYSTEM = 18;   //Digital input: tractive system active(0) or inactive(1)
int BRAKE_PRESSURE_IN = 36;       //Analog input: brake pressure (0-100mV), pin 36 is input only
int BRAKE_PRESSURE_OUT = 33;    //Analog output: brake pressure (0-5V)
int BUTTON = 19;            //Digital input: button pressed(1) or not pressed(0)
int RTDS = 21;              //Digital output: ready to drive(1) or not ready(0)

//Write functions for task
void apps_check(void* pvParameters) {
  
  //Configure SPI
 
  unsigned long start;
  int implausible = 0;
  while (1) {
    
    //FOR SENSOR 1
    uint8_t data[10] = {0};
    int index = 0;

    SPI.beginTransaction(SPISettings(125000, MSBFIRST, SPI_MODE1));
    digitalWrite(CS_1, LOW);
    delayMicroseconds(20);
    
    //Send start byte
    data[index++] = SPI.transfer(0xAA);
    delayMicroseconds(50);
 
    //Send and receive data bytes
    for (uint8_t i = 1; i < 10; i++) {
      data[index++] = SPI.transfer(0xFF);
      delayMicroseconds(40);
    }

    digitalWrite(CS_1, HIGH);
    SPI.endTransaction();
 
    //Extract and calculate angle
    uint16_t data_bytes = (data[2] << 8) | data[3];  // Combine two bytes
    uint16_t first14 = data_bytes >> 2; // Extract first 14 bits
    float value = (float)(first14 / 16384.0);  // Divide by 2^14 and multiply by 360 to get degrees
    float angle1 = (value - 0.1) / (0.9 - 0.1) * 360;

    //FOR SENSOR 2
    
    data[10] = {0};
    index = 0;

    SPI.beginTransaction(SPISettings(125000, MSBFIRST, SPI_MODE1));
    digitalWrite(CS_2, LOW);
    delayMicroseconds(20);
    
    //Send start byte
    data[index++] = SPI.transfer(0xAA);
    delayMicroseconds(50);
 
    //Send and receive data bytes
    for (uint8_t j = 1; j < 10; j++) {
      data[index++] = SPI.transfer(0xFF);
      delayMicroseconds(40);
    }

    digitalWrite(CS_2, HIGH);
    SPI.endTransaction();
 
    //Extract and calculate angle
    data_bytes = (data[2] << 8) | data[3];  // Combine two bytes
    first14 = data_bytes >> 2; // Extract first 14 bits
    value = (float)(first14 / 16384.0);  // Divide by 2^14 and multiply by 360 to get degrees
    float angle2 = (value - 0.1) / (0.9 - 0.1) * 360;
    
    float ratio = angle1 / angle2;
    if (ratio > 1.1 || ratio < 0.9) {
      start = micros();
      implausible = 1;
      while (micros() - start <= 100) {
        if ((ratio > 1.1 || ratio < 0.9) == false) {
          implausible = 0;
          break;
        }
      }
    }
    if (implausible == 1) {
      digitalWrite(THROTTLE, 0);
    }
    else {
      dacWrite(THROTTLE, ratio*255);
    }
  }
}

void RTDS_check(void* pvParameters) {
  while (1) {
    float pressure_in = analogRead(BRAKE_PRESSURE_IN);
    float pressure_out = map(BRAKE_PRESSURE_IN, 0, 124, 0, 255);
    dacWrite(BRAKE_PRESSURE_OUT, pressure_out);
    if (digitalRead(TRACTIVE_SYSTEM) == LOW && digitalRead(BUTTON) == HIGH && analogRead(BRAKE_PRESSURE_IN) > 0) {
      digitalWrite(RTDS, HIGH);
    }
    else {
      digitalWrite(RTDS, LOW);
    }
  }
}

void setup() {

  //Configure serial
  Serial.begin(115200);

  //Configure pins
  pinMode(THROTTLE, OUTPUT);
  pinMode(TRACTIVE_SYSTEM, INPUT);
  pinMode(BRAKE_PRESSURE_IN, INPUT);
  pinMode(BRAKE_PRESSURE_OUT, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(RTDS, OUTPUT);
  pinMode(CS_1, OUTPUT);
  pinMode(CS_2, OUTPUT);

  //Deselect sensors initially
  digitalWrite(CS_1, HIGH);
  digitalWrite(CS_2, HIGH);
  SPI.begin();


  // Start APPS Check
  xTaskCreatePinnedToCore(
    apps_check,              //Function to be called
    "APPS Check",           //Name of task
    2048,                   //Stack size
    NULL,                   //Parameter to pass to function
    1,                      //Task priority
    NULL,                   //Task handle
    pro_cpu);               //Define CPU core to run the task

  // Start RTDS Check
  xTaskCreatePinnedToCore(
    RTDS_check,
    "RTDS Check",
    2048,
    NULL,
    2,
    NULL,
    app_cpu);
}

void loop() {

}