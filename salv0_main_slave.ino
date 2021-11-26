//Directions for the slave to output to the motor shield for SALV0 project
#include<Wire.h>

//Motor directional pins
#define motor_left_dir 7
#define motor_right_dir 8

//Motor pwm pins
#define motor_left_pwm 9
#define motor_right_pwm 10

//Motor sleep pins -- these aren't actually used
#define motor_left_slp 2
#define motor_right_slp 4


//Motor vehicle state
int vehicleState;

//Set vehicle heading state
bool HeadingPosY;

//Store the number of rotations to travel
float l_str_count, r_str_count;

//Store the number of rotations to turn
float l_turn_count, r_turn_count;


//Array to store encoder and vehicle state values
int commsArr[2];

//vehicleStraight -- Vehicle State 1
//vehicleStop -- Vehicle State 2
//vehicleLeftTurn -- Vehicle State 3
//vehicleRightTurn -- Vehicle State 4
//vehicleReverse -- Vehicle State 5
//vehicleStraightCount -- Vehicle State 6
//vehicleTurnCount -- Vehicle State 7

//<----------------Tuning the vehicle---------------------->//

float rot_str_count = 3.0;//Control the number of rotations forward
float rot_turn_count = 2.1;//Control the number of rotations to turn


//Motor straight speeds
#define motor_spd_left 100
#define motor_spd_right 100

//Motor reverse speeds
#define motor_rev_spd 100


void setup() {

    //Assign motor pins
  pinMode(motor_left_dir, OUTPUT);
  pinMode(motor_right_dir, OUTPUT);

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  
  Serial.begin(9600); 
  vehicleState = 6;
}

void loop() {

  
  //Debugging Statements
  //Serial.print("Vehicle State :");
  //Serial.println(vehicleState);
  //Serial.print("Left Encoder count:");
  //Serial.println(commsArr[0]/360.0);
  Serial.print("Right Encoder count:");
  Serial.println(commsArr[1]);
  
  
  
  
  switch(vehicleState){

    case 1:
      vehicleStraight();
      break;
    case 2:
      vehicleStop();
      break;
    case 3:
      vehicleLeftTurn();
      break;
    case 4:
      vehicleRightTurn();
      break;
    case 5:
      vehicleReverse();
      break;
    case 6:
      vehicleStraightCount();
      break;
    case 7:
      vehicleTurnCount();
      break;
    
  }

}

//<---------------------Vehicle State 1 ------------------------------------>//
void vehicleStraight(){


  analogWrite(motor_right_pwm, motor_spd_left);
  analogWrite(motor_left_pwm, motor_spd_right);
  

  digitalWrite(motor_left_dir, HIGH);
  digitalWrite(motor_right_dir, HIGH);

  if((commsArr[0]/360.0) >= l_str_count && (commsArr[1]/360.0) >= r_str_count)
    vehicleState = 7;

  
}

//<--------------------- Vehicle State 3 ------------------------------------>//
void vehicleLeftTurn(){

  analogWrite(motor_left_pwm, 0);
  analogWrite(motor_right_pwm, 100);

  digitalWrite(motor_left_dir, LOW); 

  if(commsArr[1]/360.0 >= r_turn_count){
    
    HeadingPosY = true;
    vehicleState = 6;
    
  }

}

//<--------------------- Vehicle State 4 ------------------------------------>//

void vehicleRightTurn(){
  
  analogWrite(motor_right_pwm, 0);
  analogWrite(motor_left_pwm, 100);

  if(commsArr[0]/360.0 >= l_turn_count)
  {
    HeadingPosY = false;
    vehicleState = 6;
  }

  digitalWrite(motor_right_dir, LOW);

  
}
//<--------------------- Vehicle State 2 ------------------------------------>//
void vehicleStop(){
  
  analogWrite(motor_right_pwm, 0);
  analogWrite(motor_left_pwm, 0);  

  delay(300);
}

//<--------------------- Vehicle State 5 ------------------------------------>//
void vehicleReverse(){
  

  analogWrite(motor_right_pwm, motor_rev_spd);
  analogWrite(motor_left_pwm, motor_rev_spd);
  
  
  digitalWrite(motor_left_dir, HIGH);
  digitalWrite(motor_right_dir, HIGH);

  
}

//<--------------------- Vehicle State 6 ------------------------------------>//
void vehicleStraightCount(){


  l_str_count = (commsArr[0]/360.0) + rot_str_count;
  r_str_count = (commsArr[1]/360.0) + rot_str_count;

  vehicleState = 1;

  
}
//<--------------------- Vehicle State 7 ------------------------------------>//
void vehicleTurnCount(){

  //TODO: Needs to do the turn count thing
  if(HeadingPosY == true){
      l_turn_count = (commsArr[0]/360.0) + rot_turn_count;
      vehicleState = 4;
  }
  else if(HeadingPosY == false){
    
      r_turn_count = (commsArr[1]/360.0) + rot_turn_count;
      vehicleState = 3;
  }

  
}



//Function to recieve data from master arduino
void receiveEvent(int howMany)
{
  for(int i=0; i<howMany; i++){

     commsArr[i] = Wire.read();
     //Serial.println(commsArr[i]);
  }
}
