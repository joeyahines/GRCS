Generic Robot Control System

For use on an arduino or compatible device. The goal of GRCS is to quickly 
program arduino powered robots and other remote controlled devices.

The robot receives an array from a controller controller (GRC is provided
for free on my GitHub page). The array includes device data such as device
type, pin, GRCS control id, etc. 

All packets end with a new line character 

To program the robot, send $ over serial. Then send the number of devices
followed by the individual device info.

Commands have the format of first a ASCII character  identifier and then a 
value. The values is from 0 to 200. Where 100 is 0, 200 is full speed forward 
and 0 is full speed reverse For example, if you want to turn on a device called
right motor at full forward, you would send the command "R200". 

Data stored in the deviceArray:
Modifiable
-GRCS id
-Pin Number
-Should motor be inversed
-number of device specific values
-Array of extra values 
Objects:
-Adafuit motor object
-Servo object

