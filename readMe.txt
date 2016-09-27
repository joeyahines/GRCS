Generic Robot Control System Version 4

For use on an Arduino or compatible device. The goal of GRCS is to quickly 
program Arduino powered robots and other remote controlled devices.

The robot receives an array from a controller (GRC is provided with this
project). The array includes device data such as device type, pin, GRCS control
id, etc. 

All packets end with a new line character 

To program the robot, send $ over serial. Then send the number of devices
followed by the individual device info.

Commands have the format of first a ASCII character  identifier and then a 
value. The values is from 0 to 200. Where 100 is 0% power, 200 is full speed 
forward and 0 is full speed reverse For example, if you want to turn on a 
device called right motor at full forward, you would send the command "R200". 

GRCS_Comms can be used on a bridge device (such as a ESP8266) to send commands
wirelessly. 

Data stored in the deviceArray:
Modifiable
-char: GRCS id
-int: Pin Number
-int: Should motor be inversed
-int: number of device specific values
-int poniter: Array of extra values 
Objects:
-Adafuit motor object
-Servo object

Near Goals
-Add sensor support/back communication to controller
-Split devices into individual objects with a abstract class parent that
 defines all the functions. This is to replace the switch statements and
 modularity
-Allow for wireless reprogramming of the Arduino, so controller has full
 control and can define devices at runtime and at the controller level.
 This also means that not all code has to be included for certain robots

Far Off Goals
-Add cross platform support (RasPi, Arduino, Windows IOT, others)