/*
 *  GRCSv1
 *  By: Joey Hines
 *  
 *  For use on an Arduino or compatiable device. Commands
 *  are recvived via Serial at a buad rate of 9600
 *  
 *  Commands have the format of first a symbol identifer
 *  and then a value. For example, if you want to turn 
 *  on the right motor at full power, you would send'
 *  the command "R100"
 *  
 */

#include <Servo.h>

//Drive Motor Pins
const int rightMotor = 10;
const int leftMotor = 9;

//Servo Objects
Servo right;
Servo left;

//Setup
void setup() {
  //Begin Serial
  Serial.begin(9600);
  //Set timeout to 10 for fast int parsing
  Serial.setTimeout(10);

}

void loop() {
  //Int for storing the serial input
  int input = 0;
  
  //If data is received
  if (Serial.available() > 0) {
    
      //Store first byte to input
      input = Serial.read();
      //Echo it to serial for debugging
      //Serial.write(input);

      //Set the motor to write based on control byte 'L' for left, 'R' for right
      //The next byte is the speed (0 to 100, -100 for reverse full speed and 100 for foward full speed)
      if (input == 'R') {       
        int sp = Serial.parseInt();
        setMotor(rightMotor, sp);
        Serial.write(sp);
      }
      else if (input == 'L') {
        int sp = Serial.parseInt();
        setMotor(leftMotor, sp);
        Serial.print(sp);
      }
  }

  //Serial Sent Test
  //sendSerialData('T', 42);

  //If data needs to be sent
}

/* void setMotor(int pin, int sp)
 * Takes a value form -100 to 100  and sets the servo angle 0 to 180
 * The left motor is reversed 
 */
void setMotor(int pin, int sp) {
  int angle = sp * 0.9 + 90;

  if (pin == rightMotor) {
    //Add this line back in if the right motor needs to be inverted
     //invertAngle(angle);
    if (sp == 0) {
      right.detach();
    }
    else {
      right.attach(rightMotor);
      right.write(angle);
    }
  }
  else if (pin = leftMotor) {
    //Remove this line if the left motor is not reversed
    angle = invertAngle(angle);
    if (sp == 0) {
      left.detach();
    }
    else {
      left.attach(leftMotor);
      left.write(angle);
    }
  }
}
/* int invertAngle(int angle)
 * Used to invert the angle or the direction of motor spin
 */
int invertAngle(int angle) {
    angle = angle-90;
    angle = -angle;
    return angle + 90; 
}

/* void sendSerialData(char id, int data)
 * Writes to data to serial format is "CHAR_ID""INT_DATA"
 */
void sendSerialData(char id, int data) {
  Serial.write(id);
  Serial.write(data);
}

