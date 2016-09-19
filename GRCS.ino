/*
 *  GRCSv2
 *  By: Joey Hines
 *  
 *  For use on an Arduino or compatible device. Commands
 *  are received via Serial at a buad-rate of 9600
 *  
 *  Commands have the format of first a symbol identifier
 *  and then a value. For example, if you want to turn 
 *  on the right motor at full power, you would send'
 *  the command "R100"
 *  
 *  Currently only works with the Servo class, more
 *  to be added in v3.
 *  
 *  Sensor support also coming, planned for v4.
 *  
 *  Change Log:
 *  -Motor code rewrite
 *    Adds modularity to code
 *    Allows for adding new additional motors quickly
 *    Use GRCSv2_Generator.exe to add new motors quickly
 *  
 */

#include <Servo.h>

//Structure for storing motor data
struct Motor{
  int pin;
  int id;
  bool inverse;
  //To be implemented later for different kinds of motor control
  //int motorType
  Servo servo;
};

/*
 * Format for motor array
 * {pin, id, inverse}
 * ,{,,,}
*/
Motor motors[] = {{10,'R',0},{9, 'L', 1}}; //...
const int NUMBER_OF_MOTORS = 2;

//MAIN PROGRAM BODY
void setup() {
  //Begin Serial
  Serial.begin(9600);
  //Set timeout to 10 for fast int parsing
  Serial.setTimeout(10);

}

void loop() {
  //Int for storing the serial input
  int id = 0;
  
  //If data is received
  if (Serial.available() > 0) {
    
      //Store the ID byte to input
      id = Serial.read();
      //Echo it to serial for debugging
      //Serial.write(serialInput);

      //Iterate through the motor array
      for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
        //If the id matches one of the motors
        if (motors[i].id == id) {
          //Get the value to write to the motor
          int value = Serial.parseInt();
          //Check if the motor needs to be reversed
          if (motors[i].inverse) {
              value = -value;
          }
          //Convert from percentage to angle (0 to 180)
          int angle = value * 0.9 + 90;
          //If the value was not 0
          if (value != 0) {
            //Attach the motor
            motors[i].servo.attach(motors[i].pin);
            //Write the angle to he motor/servo
            motors[i].servo.write(angle);    
          }
          else {
            //Detach motor to remove unwanted movement
            motors[i].servo.detach();    
          }      
        }
      }
      
  }

  //Will be looked at for v4...
  //Serial Sent Test
  //sendSerialData('T', 42);
}



