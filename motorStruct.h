#include <Servo.h>

//Structure for storing motor data
struct Motor{
  //Stores the pin the motor is wired to
  int pin;
  //Stores the GRCS ID of the motor
  int id;
  //Stores if the motor needs to be reversed, 1 for yes 0 for no
  bool inverse;
  //To be implemented later for different kinds of motor control
  //int motorType
  //Servo object for manipulating the motor
  Servo servo;
};
