//Directions for master to take in rotary encoder data and output to motorshield/slave
#include<Wire.h>

//Encoder variables
//Left encoders
static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3
//Right Encoder
static int pinC = 18;
static int pinD = 19;

volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte cFlag = 0;
volatile byte dFlag = 0;


volatile byte oldEnc1Pos = 0;
volatile byte oldEnc2Pos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading1 = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
volatile byte reading2 = 0;

//Store the encoder pulse data
int encoder1Pos = 0;
int encoder2Pos = 0;




//0 -- Vehicle state | 1 -- encoder1Pos | 2 -- encoder2Pos
int commsArr[2];

//vehicleStraight -- Vehicle State 1
//vehicleStop -- Vehicle State 2
//vehicleLeftTurn -- Vehicle State 3
//vehicleRightTurn -- Vehicle State 4
//vehicleReverse -- Vehicle State 5
//vehicleStraightCount -- Vehicle State 6
//vehicleTurnCount -- Vehicle State 7



void setup() {



  //assign encoder pins
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinC, INPUT_PULLUP);
  pinMode(pinD, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinA),PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(digitalPinToInterrupt(pinB),PinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  attachInterrupt(digitalPinToInterrupt(pinC),PinC,RISING);
  attachInterrupt(digitalPinToInterrupt(pinD),PinD,RISING);

  Wire.begin();

  Serial.begin(9600); // start the serial monitor link
  
  
}

void loop() {


    if(oldEnc1Pos != encoder1Pos) {
      //Serial.print("Encoder 1 Rotations: ");
      //Serial.println(encoder1Pos/360);
  
      oldEnc1Pos = encoder1Pos;
  }

  if(oldEnc2Pos != encoder2Pos) {
    //Serial.print("Encoder 2 Rotations: ");
    //Serial.println(encoder2Pos/360);

    oldEnc2Pos = encoder2Pos;

  }


  //Store vehicle state / encoder values in comms array to send to slave
    commsArr[0] = encoder1Pos;//Left encoder
    commsArr[1] = encoder2Pos;//Right encoder

  //Communications loop to send
  Wire.beginTransmission(4); // transmit to device #4


  for(int i=0; i<2;i++){
    Wire.write(commsArr[i]);
  }

  //Wire.write("# of Pulses ");        // sends five bytes
 //Wire.write(encoder1Pos/360);    // sends one byte

  Wire.endTransmission();    // stop transmitting
    
  

}



void PinA(){
  cli(); //stop interrupts happening before we read pin values
  reading1 = PINE & 0x30; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading1 == B00110000 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoder1Pos --; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading1 == B00010000) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinB(){
  cli(); //stop interrupts happening before we read pin values
  reading1 = PINE & 0x30; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading1 == B00110000 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoder1Pos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading1 == B00100000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}


void PinC(){
  cli(); //stop interrupts happening before we read pin values
  reading2 = PIND & 0x0C; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading2 == B00001100 && cFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoder2Pos --; //decrement the encoder's position count
    dFlag = 0; //reset flags for the next turn
    cFlag = 0; //reset flags for the next turn
  }
  else if (reading2 == B00001000) dFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinD(){
  cli(); //stop interrupts happening before we read pin values
  reading2 = PIND & 0x0C; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading2 == B00001100 && dFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoder2Pos ++; //increment the encoder's position count
    dFlag = 0; //reset flags for the next turn
    cFlag = 0; //reset flags for the next turn
  }
  else if (reading2 == B00000100) cFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}
