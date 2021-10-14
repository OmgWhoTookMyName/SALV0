
//Code Purpose: Develop a simple bot that takes Ultra-sonic sensor data to decide when to turn around
//Author: Randal Govan

//Libraries for Screen
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


  
  //Define Left Motor pins
  #define MOTOR_LEFT_IN1 6
  #define MOTOR_LEFT_IN2 7
  
  //Define Right Motor pins
  #define MOTOR_RIGHT_IN3 9
  #define MOTOR_RIGHT_IN4 10

  #define left_wheel_encoder 19
  #define right_wheel_encoder 18

  //Variable to control various vehicle states
  int vehicleState;

  //Define which way the vehicle is facing
  bool HeadingPosY;
  bool HeadingPosX;

  //Ultrasonic sensor pinouts
  const int trigPin = 2;
  const int echoPin = 3;
  
  //Ultrasonic sensor variables 
  long duration;
  int distance;
  
  //Declare variables for motor speeds
  int motor_speed_left = 200, motor_speed_right = 200;

  
  //Time variables for encoders
  unsigned long l_last_time, r_last_time;

  //Count variables for encoders
  int l_rot_count_raw, r_rot_count_raw;
  float l_rot_count, r_rot_count;
  

  //Function used to run something every t interval in ms
  #define runEvery(t) for (static typeof(t) _lasttime;(typeof(t))((typeof(t))millis() - _lasttime) > (t);_lasttime += (t))

  //Assign screen output
  LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // put your setup code here, to run once:

   //Motor Left
  //pinMode(MOTOR_LEFT_ENA, OUTPUT);
  pinMode(MOTOR_LEFT_IN1, OUTPUT);
  pinMode(MOTOR_LEFT_IN2, OUTPUT);

  //Motor Right
  //pinMode(MOTOR_RIGHT_ENB, OUTPUT);
  pinMode(MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(MOTOR_RIGHT_IN4, OUTPUT);

  //Serial Start-up for logging
  Serial.begin(9600);

  //Set the turn time of one motor to complete 180 degree turn
  //TurnTime = millis() + 1600;

  //Set initial Go forward state of vehicle
  vehicleState = 1;

  //Set Y Heading in the positive Y assume the vehicle to start at (0,0) and mow area is Quadarant 1
  HeadingPosY = true;

  //Set-up Ultrasonic pins & variables for 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //Create interrupt function for wheel encoders
  digitalWrite(left_wheel_encoder, 1);  //Left wheel encoder
  attachInterrupt(digitalPinToInterrupt(left_wheel_encoder), Lencoder, CHANGE);
  
  digitalWrite(right_wheel_encoder, 1);  //Right wheel encoder
  attachInterrupt(digitalPinToInterrupt(right_wheel_encoder), Rencoder, CHANGE);

  //Initialize LCD and backlight
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  
  
}

void loop() 
{
  
  l_rot_count = l_rot_count_raw / 6.0; 
  r_rot_count = r_rot_count_raw / 6.0;

    //Serial.print("l_rot_count: ");
    //Serial.println(l_rot_count);
 


 //<------- Diagnostics & Screen display ------->// 
  runEvery(100){
  Serial.print("Vehicle State: ");
  Serial.println(vehicleState);
  //Serial.print("Motor Speed Left: ");
  //Serial.println(motor_speed_left);
  //Serial.print("HeadingPosY: ");
  //Serial.println(HeadingPosY);

  //Print Vehicle State to LCD display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vehicle State:");
  lcd.setCursor(0,16);
  lcd.print(vehicleState);
  }

//<------- List of Vehicle Functions ------->//
/*          GoStraight();
            VehicleStop();
            VehicleRightTurn();
            VehicleLeftTurn();
            VehicleReverse();
*/

    switch(vehicleState){
      case 1:
        GoStraight();
        break;
      case 2:
        VehicleStop();
        break;
      case 3:
        VehicleRightTurn();
        break;
      case 4:
        VehicleLeftTurn();
        break;
      case 5:
        VehicleReverse();
        break;     
    }

//<------Update Vehicle Speed Via Encoders-------->//

//Measure the time between pulses every 5ms;
    runEvery(5){
      int l_last_pulse = millis() - l_last_time;
      int r_last_pulse = millis() - r_last_time;
      //Serial.print("Pulse Time: ");
      //Serial.println(last_pulse);
      
    
//Update the motor speed if the pulse signal starts drift from given value
if(l_last_pulse > 120 && motor_speed_left < 255)//TODO: Make the pulse width a variable
    {
      motor_speed_left = motor_speed_left + 1;
      //Serial.print("Increasing");   
    }
if(l_last_pulse < 120 && motor_speed_left > 0)
  {
    motor_speed_left = motor_speed_left - 1;
    //Serial.print("Decreasing");
    
  }
if(r_last_pulse > 120 && motor_speed_right < 255)
    {
      motor_speed_left = motor_speed_right + 1;
      //Serial.print("Increasing");
      
    }
if(r_last_pulse < 120 && motor_speed_right > 0)
  {
    motor_speed_left = motor_speed_right - 1;
    //Serial.print("Decreasing");
    
  }

if(l_last_pulse > 4000 && r_last_pulse > 4000)
{
  vehicleState = 5;
}



  
} 



//<------Continously run the Ultrasonic Sensor-------->//
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

// Prints the distance on the Serial Monitor
//Serial.print("Distance: ");
//Serial.println(distance);




//<------Methods of controlling Vehicle States-------->//

//If you hit a wall going in the positive Y direction make a right 180 turn
if(distance < 20 && HeadingPosY == true && distance > 10)
{
  vehicleState = 3;

}

//If you hit a wall going in the negative Y direction make a left 180 turn
if(distance < 20 && HeadingPosY == false && distance > 10)
{
  vehicleState = 4;

}

//If distance less than 10cm reverse and make a 180
if(distance <= 10)
{
  
  vehicleState = 5;
  
}
  
 
}


//Vehice state 1
void GoStraight(){
    
    //Set motor speeds
    analogWrite(5, motor_speed_left);       
    analogWrite(8, motor_speed_right);

    
    //Turn Left Motor
    digitalWrite(MOTOR_LEFT_IN1, HIGH);
    digitalWrite(MOTOR_LEFT_IN2, LOW);

    //Turn Right Motor
    digitalWrite(MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(MOTOR_RIGHT_IN4, LOW);
  

}

//Vehcile state 2
void VehicleStop(){
    
    //Turn Left Motor
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);


    //Turn Right Motor
    digitalWrite(MOTOR_RIGHT_IN3, LOW);
    digitalWrite(MOTOR_RIGHT_IN4, LOW);

    delay(1000);//TODO: Assign this in variable this is turn time

}

//Vehicle state 3
void VehicleRightTurn(){
    //Store the current number of rotations and spin one wheel until 180 turn.
    float l_turn_count = (l_rot_count_raw/6.0) + 2.0;  //TODO: 2.0 represents number of rotations to turn 180
    
    VehicleStop();

    while((l_rot_count_raw / 6.0) < l_turn_count){
    digitalWrite(MOTOR_LEFT_IN1, HIGH);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    Serial.println((l_rot_count_raw / 6.0));
    Serial.println(l_turn_count);
    
    }
    if((l_rot_count_raw / 6.0) >= l_turn_count)
    {
      HeadingPosY = false;
      VehicleStop();
      vehicleState = 1;
    }
 
}
//Vehicle state 4
void VehicleLeftTurn(){
    //Store the current number of rotations and spin one wheel until 180 turn.
    float r_turn_count = (r_rot_count_raw/6.0) + 2.0;  //TODO: 2.0 represents number of rotations to turn 180
    
    VehicleStop();

    while((r_rot_count_raw / 6.0) < r_turn_count){
    digitalWrite(MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(MOTOR_RIGHT_IN4, LOW);
    Serial.print("Rotation Count: ");
    Serial.println((r_rot_count_raw/6.0));
    Serial.print("Goal Turn Count: ");
    Serial.println(r_turn_count);
    }
    if((r_rot_count_raw / 6.0) >= r_turn_count)
    {
      HeadingPosY = true;
      VehicleStop();
      vehicleState = 1;
    }
 
}

void VehicleReverse()
{

    VehicleStop();
    //Set motor speeds
    analogWrite(5, motor_speed_left);       
    analogWrite(8, motor_speed_right);

    
    //Turn Left Motor
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, HIGH);

    //Turn Right Motor
    digitalWrite(MOTOR_RIGHT_IN3, LOW);
    digitalWrite(MOTOR_RIGHT_IN4, HIGH);
    
    delay(1000);
    if(HeadingPosY == true)
    {
      vehicleState = 3;
    }
    else if(HeadingPosY == false)
    {
      vehicleState = 4;
    }
  
  
}

void Lencoder()
{  
  l_last_time = millis();
  l_rot_count_raw++;
  
}

void Rencoder()
{
  
  r_last_time = millis();
  r_rot_count_raw++;
}
