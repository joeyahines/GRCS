/*
 *  GRCSv4
 *  By: Joey Hines (joey.ahines@gmail.com)
 *
 *  Change Log:
 *  +New devices added
 *   -Adafruit Motor Controller Shield
 *   -Relays
 *   -PWM Motor Controller
 *   -Any PWM controlled device
 *  +motor structure renamed to device and new elements added
 *    -Since more than motors are not supported, the rename was done to avoid 
 *	    confusion
 *    -Value pointer added for dynamic array for storing device specific values
 *	  -numberOfDeviceValues also added to keep track of this array's size
 *  +Switch cases added for device specific code
 * 
 */

#include <Servo.h>
#include "deviceStruct.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//Pointer for creating dynamic deviceArray
Device * deviceArray;

//Motor Sheild Object
Adafruit_MotorShield AFMS;

//Tracking if a motorSheild motor is added
bool firstMotorSheild = true;

//Number of devices
int numberOfDevices;
//bool for tracking first time setup
bool firstTimeSetup = true;

//MAIN PROGRAM BODY
void setup() {
  //Begin Serial
  Serial.begin(9600);
  AFMS.begin();
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

  //Call setupdeviceArray for first time setup
  setupDeviceArray();
  

}

void loop() {
  //Int for storing the serial input
  int id = 0;

  //If data is received
  if (Serial.available() > 0) {

    //Store the ID byte to input
    id = getSerialData();
    //Echo it to serial for debugging
    //Serial.print(id);
    
    //if the reassign id was not sent
    if (id != '$') {
      //Iterate through the motor array
      for (int i = 0; i < numberOfDevices; i++) {
        //If the id matches one of the motors
        if (deviceArray[i].id == id) {
          //Get the value to write to the motor
          int value = getSerialData();
          //Echo it to serial for debugging
          //Serial.println(value);
          writeToDevice(deviceArray[i], value);
        }
      }
    }
    //If it was
    else {
      //Call the setup deviceArray again
      setupDeviceArray();
    }


  }
}

/* void writeToDevice(Motor motor, int value)
 *  Takes value from -100 to 100 and writes and converts
 *  it to an angle from 0 to 180. Then it writes the
 *  angle to the servo
 */


void writeToDevice( device, int value) {
  //Inverse value if the motor is inversed
  if (device.inverse) {
    value = -(value-100)+100;
  }
  
  int deviceValue;
  
  //Converts the GRCS value to the device specific value
  switch(device.deviceType) {
      case 0: //Case for Servo
      //Convert to servo angle depending on device limits 
        deviceValue = (value * ((device.values[0] - device.values[0])/200)) + device.values[0];
          if (value != 100) {
            //Attach the motor
            device.servo.attach(device.pin);
            //Write the angle to he motor/servo
            device.servo.write(deviceValue);
          }
          else {
            //Detach motor to prevent unwanted movement
            device.servo.detach();
          }
        break;
      case 1: //Case for device controller shield
        //Convert to a 0 to 255 scale
        deviceValue = (value - 100) * 2.55;

        //If device value is less than 0
        if (deviceValue  < 0) {
          device.motor->run(BACKWARD);
          device.motor->setSpeed(-deviceValue);
        }
        //If it is greater than 0
        else if (deviceValue > 0) {
          device.motor->run(FORWARD);
          device.motor->setSpeed(deviceValue);
          
        }
        //If device value is 0
        else {
        device.motor->run(FORWARD);
        device.motor->setSpeed(0);
        }         
        break;
      case 2: //Relay case
        //If value is 0
        if (value != 100) {
          digitalWrite(device.pin, HIGH);
        }
        //If it is not 0
        else {
          digitalWrite(device.pin, LOW);
        }
        break;
      case 3: //Case for Motor Controller
        //Convert to a 0 to 255 scale
        deviceValue = (value - 100) * 2.55;

        //If device values is less than 0
        if (deviceValue  < 0) {
          //Set motor to reverse
          digitalWrite(device.value[0], HIGH);
          digitalWrite(device.value[1], LOW);
          analogWrite(device.pin, -deviceValue);
          
        }
        //If device value is greater than 0
        else if (deviceValue > 0) {
          //Set motor to forward
          digitalWrite(device.value[0], LOW);
          digitalWrite(device.value[1], HIGH);
          analogWrite(device.pin, deviceValue);
        } 
        else {
          //Stop Motor
          analogWrite(device.pin, 0);
        }     
   }
}

/* bool setupDeviceArray()
 * Receives all device info from the controller and the
 * dynamically creates an array for storing all of it
 */
bool setupDeviceArray() {
  if (!firstTimeSetup) {
    delete deviceArray;
  }
  //Wait to recv data
  Serial.println("Ready to Comply");

  //First value is the number of devices
  //Store the number of devices
  numberOfDevices = getSerialData();

  //Creat an array large enough to store all the devices
  deviceArray = new Device[numberOfDevices];

  //Print the number recv for debugging
  Serial.print("# of devices: ");
  Serial.println(numberOfDevices);

  //For each device, get the device data and
  //store it in the deviceArray
  for (int i = 0; i < numberOfDevices; i++) {
    deviceArray[i].id = getSerialData();
    Serial.print("ID: ");
    Serial.println(deviceArray[i].id);
    
    deviceArray[i].pin = getSerialData();
    Serial.print("PIN: ");
    Serial.println(deviceArray[i].pin);
    
    deviceArray[i].inverse = getSerialData();
    Serial.print("Inverse: ");
    Serial.println(deviceArray[i].inverse);
    
    deviceArray[i].deviceType = getSerialData();
    Serial.print("Device Type: ");
    Serial.println(deviceArray[i].deviceType);

    deviceArray[i].numberOfDeviceValues = getSerialData();
    Serial.print("Number Of Device Values:");
    Serial.println(deviceArray[i].numberOfDeviceValues);   

    //If the device needs more values
    if (deviceArray[i].numberOfDeviceValues != 0) {
      //Create a array to store them in
      deviceArray[i].value = new int[deviceArray[i].numberOfDeviceValues];

      //Fill the array
      for (int x = 0; x < deviceArray[i].numberOfDeviceValues; x++) {
        deviceArray[i].value[x] = getSerialData();
        Serial.print("Device Value: ");
        Serial.println(deviceArray[i].value[x]);
      }
    }
    
    //Switch for individual device setup, more to be added
    switch(deviceArray[i].deviceType) {
      case 0:
        break;
      case 1: //Case for device controller shield
       
        if (firstMotorSheild == true) {
          AFMS = Adafruit_MotorShield(); 
          firstMotorSheild = false;
        }
        deviceArray[i].motor = AFMS.getMotor(deviceArray[i].pin);
        deviceArray[i].motor->setSpeed(0);
        deviceArray[i].motor->run(FORWARD);
        deviceArray[i].motor->run(RELEASE);
        break;
      case 2: 
        break;
      case 3: 
        pinMode(deviceArray[i].pin, OUTPUT);  
        pinMode(deviceArray[i].value[0], OUTPUT);
        pinMode(deviceArray[i].value[1], OUTPUT);       
        break;
      default:
        Serial.println("Unknown type, assuming PWM servo."); 
        deviceArray[i].deviceType = 0;
    }
  
  }
  Serial.println("Done.");
  firstTimeSetup = false;

  //Will be used for checking impartial setups in future 
  //versions

  return true;
}

/* int getSerialData()
 *  Gets a packet from serial and returns its
 *  ASCII code value contained within
 */
int getSerialData() {
  //String for storing input
  String inString;

  //Run until full packet is received 
  while (1) {
    //Wait for serial connection
    while (!Serial.available());
    //Read in the next char
    int inChar = Serial.read();

    //Check it the end of packet is received
    if (inChar == 10) {
      //Ignore ASCII 10
      continue;
    }
    if (inChar == 13) {
      //Packet is done, break
      break;
    }

    //Add char to string
    if (!isDigit(inChar)) {
      inString += inChar;
    }
    inString += (char)inChar;
  }

  //Return char
  return inString.toInt();
}
