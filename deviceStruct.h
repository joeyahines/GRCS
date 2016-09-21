#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//Structure for storing device data
struct Device{
  //Stores the GRCS ID of the motor
  int id;
  //Stores the pin the motor is wired to
  int pin;
  //Stores if the motor needs to be reversed, 1 for yes 0 for no
  bool inverse;
  //Stores type of device
  int type;
  //Adafruit motor controller sheild motor object
  Adafruit_DCMotor *motor;
  //Servo object
  Servo servo;
  //Pointer to a dynamic array if the device has extra data attached
  int numberOfDeviceValues;
  int * value;
};


