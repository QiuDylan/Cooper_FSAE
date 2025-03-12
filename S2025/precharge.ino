//Pinouts for input and output signals
int IO1 = 12;                //Digital signal to represent opening(0)/closing(1) AIR+
int IO2 = 13;                //Digital signal to represent opening(0)/closing(1) AIR-
int IO3 = 14;                //Digital signal to represent opening(0)/closing(1) precharge relay 
int HVLV_In = 2;        //Digital signal to represent whether precharge has completed(0) or not(1)
int RTDS = 15;            //RTDS = 1 means ready to drive
int TX = 26;
int RX = 27;

enum STATES {
  STANDBY, 
  PRECHARGE, 
  ONLINE
};

int state = STANDBY;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IO1, OUTPUT);
  pinMode(IO2, OUTPUT);
  pinMode(IO3, OUTPUT);
  pinMode(HVLV_In, INPUT);
  pinMode(RTDS, INPUT);
  digitalWrite(IO2, HIGH);    //AIR- switch is always closed
}

void loop() {
  // put your in code here, to run repeatedly:
  switch (state) {
    case STANDBY: //open everything
      digitalWrite(IO1, LOW);
      digitalWrite(IO3, LOW);
      if(digitalRead(RTDS)==1) {
        state = PRECHARGE;
      }
    case PRECHARGE:
      digitalWrite(IO1, LOW); //Open AIR+ switch, pin D12 has to be initialized low                  
      digitalWrite(IO3, HIGH);  //Close PC relay switch 
      if(digitalRead(HVLV_In) == HIGH) {
        state = ONLINE;
      }
      break;
    case ONLINE: 
      digitalWrite(IO1, HIGH);  //Close AIR+ switch
      digitalWrite(IO3, LOW); //Open PC relay switch 
      if(digitalRead(HVLV_In) == LOW) {
        state = PRECHARGE;
      }
      break;
  }
}