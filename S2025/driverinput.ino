#include <Arduino.h>

//Core definitions
static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;

//Pin definitions
int APPS_1 = 34;            //Analog input: sensor 1, pin 34 is input only
int APPS_2 = 35;            //Analog input: sensor 2, pin 35 is input only
int THROTTLE = 32;          //Analog output: ratio between sensors or 0 if implausible
int TRACTIVE_SYSTEM = 18;   //Digital input: tractive system active(0) or inactive(1)
int BRAKE_PRESSURE_IN = 36;       //Analog input: brake pressure (0-100mV), pin 36 is input only
int BRAKE_PRESSURE_OUT = 33;    //Analog output: brake pressure (0-5V)
int BUTTON = 19;            //Digital input: button pressed(1) or not pressed(0)
int RTDS = 21;              //Digital output: ready to drive(1) or not ready(0)

//Variables
int sensor1value;
int sensor2value;
float sensor1angle;
float sensor2angle;

//Write functions for task
void apps_check(void* pvParameters) {
  
  unsigned long start;
  int implausible = 0;
  while (1) {
    
    sensor1value = analogRead(APPS_1);
    sensor2value = analogRead(APPS_2);
    
    sensor1angle = map(sensor1value, 0, 4095, 0, 40);
    sensor2angle = map(sensor2value, 0, 4095, 0, 40);
    
    float ratio = sensor1angle / sensor2angle;
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
  Serial.begin(9600);

  //Configure pins
  pinMode(APPS_1, INPUT);
  pinMode(APPS_2, INPUT);
  pinMode(THROTTLE, OUTPUT);
  pinMode(TRACTIVE_SYSTEM, INPUT);
  pinMode(BRAKE_PRESSURE_IN, INPUT);
  pinMode(BRAKE_PRESSURE_OUT, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(RTDS, OUTPUT);

  analogReadResolution(12);

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