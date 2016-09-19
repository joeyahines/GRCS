/*
 *  GRCSv3
 *  By: Joey Hines
 *
 *  For use on an Arduino or compatiable device. Commands
 *  are received via Serial at a buad rate of 9600
 *
 *  Commands have the format of first a ASCII character 
 *  identified and then a value. For example, 
 *  if you want to turn on the right motor at full power, 
 *  you would send the command "R100". 
 *
 *  Currently only works with the Servo class, more
 *  to be added in v4.
 *
 *  Sensor support also coming, planned for v4.
 *
 *  Change Log:
 *  -Motors can be assigned remotely at start up and
 *  any time $ is received
 *    *Can be disabled on line  47 if pre-defined
 *      array should be used
 *    *Packet format
 *      $ | # of Motors | First Motor Pin | First Motor Id | First Motor Inverse State | ...
 *  -GRCVS_Gen is no longer needed, but can be used
 *    *Motor assignment is hard-coded on GRC v2, v3
 *     allows for dynamic assignment
 *  -The structure "Motor" is now defined in its 
 *  own .h file called "motorStruct.h"
 *    *This allows for the code for writing to the
 *      the motors to be moved back into its own
 *      function like v1
 *
 */

#include <Servo.h>
#include "motorStruct.h"

//Pointer for creating dynamic motorArray
Motor * motorArray;
//Number of motors
int numberOfMotors;
//bool for tracking first time setup
bool firstTimeSetup = true;




//MAIN PROGRAM BODY
void setup() {
  //Begin Serial
  Serial.begin(9600);
  //Set timeout to 10 for faster int parsing
  Serial.setTimeout(10);

  //Boolean used to track if the motor reset flag has been sent
  bool recv = false;


  //While recv is false
  Serial.println("Waiting for connection...");
  while (recv == false) {
    //Check if data is coming over serial
    int value = getSerialData();
    //if it is the motor reset symbol $
    if (value == '$') {
      //Set recv to true
        recv = true;
    }
   }

  //Call setupMotorArray for first time setup
  setupMotorArray();
  

}

void loop() {
  //Int for storing the serial input
  int id = 0;

  //If data is received
  if (Serial.available() > 0) {

    //Store the ID byte to input
    id = Serial.read();
    //Echo it to serial for debugging
    //Serial.print(id);
    
    //if the reassign id was not sent
    if (id != '$') {
      //Iterate through the motor array
      for (int i = 0; i < numberOfMotors; i++) {
        //If the id matches one of the motors
        if (motorArray[i].id == id) {
          //Get the value to write to the motor
          int value = Serial.parseInt();
          //Echo it to serial for debugging
          //Serial.println(value);
          writeToMotor(motorArray[i], value);
        }
      }
    }
    //If it was
    else {
      //Call the setupMotorArray again
      setupMotorArray();
    }


  }

  //Will be looked at for v4...
  //Serial Sent Test
  //sendSerialData('T', 42);
}

/* void writeToMotor(Motor motor, int value)
 *  Takes value from -100 to 100 and writes and converts
 *  it to an angle from 0 to 180. Then it writes the
 *  angle to the servo
 */


void writeToMotor(Motor motor, int value) {
  if (motor.inverse) {
    value = -value;
  }
  //Convert from percentage to angle (0 to 180)
  int angle = value * 0.9 + 90;
  //If the value was not 0
  if (value != 0) {
    //Attach the motor
    motor.servo.attach(motor.pin);
    //Write the angle to he motor/servo
    motor.servo.write(angle);
  }
  else {
    //Detach motor to prevent unwated movement
    motor.servo.detach();
  }
}
/* bool setupMotorArray()
 * Receives all motor info from the controller and the
 * dynamically creates an array for storing all of it
 */
bool setupMotorArray() {
  if (!firstTimeSetup) {
    delete motorArray;
  }
  //Wait to recv data
  //Serial.println("Ready to Comply");

  //First value is the number of motors
  //Store the number of motors
  numberOfMotors = getSerialData();

  //Create an array large enough to store all the motors
  motorArray = new Motor[numberOfMotors];

  //Print the number recv for debugging
  //Serial.print("# of Motors: ");
  //Serial.println(numberOfMotors);

  //For each motor, get the motor data and
  //store it in the motorArray
  for (int i = 0; i < numberOfMotors; i++) {
    motorArray[i].id = getSerialData();
    Serial.print("ID: ");
    Serial.println(motorArray[i].id);
    motorArray[i].pin = getSerialData();
    Serial.print("PIN: ");
    Serial.println(motorArray[i].pin);
    motorArray[i].inverse = getSerialData();
    Serial.print("Inverse: ");
    Serial.println(motorArray[i].inverse);
  }
  Serial.println("Done.");
  firstTimeSetup = false;

  //Will be used for checking impartial setups in future 
  //versions

  /*for (int i = 0; i < numberOfMotors; i++) {
    Serial.println(motorArray[i].id);
    Serial.println(motorArray[i].pin);
    Serial.println(motorArray[i].inverse);
  }*/
  return true;
}
/* int getSerialData()
 *  Gets the ASCII charcter from serial and returns its
 *  ASCII code value
 */

int getSerialData() {
  String inString;
  while (1) {
    while (!Serial.available());
    int inChar = Serial.read();
    if (inChar == '\n') {
      break;
    }
    if (!isDigit(inChar)) {
      inString += inChar;
    }
    inString += (char)inChar;
  }
  return inString.toInt();
}



