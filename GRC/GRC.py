"""
Generic Robot Controller v1.2
For use with GRCS v4+

This program supports GRCSv4+ standards of devices

Change-log from v1:
-GRCS device array now configurable at program start up
-Device array can be loaded from file or input manually witch can be saved for later use
-Recode of the main program loop, dynamically gets joystick data based off the deviceArray
-Support for joystick buttons also added
"""

import pygame
import socket
import time
import sys


# Define some colors
BLACK    = (   0,   0,   0)
WHITE    = ( 255, 255, 255)

#Default robot ip
robot = ("192.168.4.1", 80);

#Define list fro storing the deviceArray
deviceArray = []

#Class for writing text to the screen
class TextPrint:
    def __init__(self):
        self.reset()
        self.font = pygame.font.Font(None, 20)

    def prin(self, screen, textString):
        textBitmap = self.font.render(textString, True, BLACK)
        screen.blit(textBitmap, [self.x, self.y])
        self.y += self.line_height
        
    def reset(self):
        self.x = 10
        self.y = 10
        self.line_height = 15
        
    def indent(self):
        self.x += 10
        
    def unindent(self):
        self.x -= 10
	
#class for storing GRCS devices and how to control them
class Device: 

	def __init__(self, id, pin, inverse, deviceType, controlType, controlNumber, numberOfDeviceValues):
		self.id = id
		self.pin = pin
		self.inverse = inverse
		self.deviceType = deviceType
		self.controlType = controlType
		self.controlNumber = controlNumber
		self.numberOfDeviceValues = numberOfDeviceValues;
		self.deviceValues = [];
		
	def toFile(self):
		string = self.id + "\n" + self.pin + "\n" + self.controlType + "\n" + self.inverse + "\n" + self.deviceType + "\n" + self.controlType + "\n" + self.controlNumber + "\n" + str(self.numberOfDeviceValues) + "\n"
		
		if (self.numberOfDeviceValues != 0) :
			for x in range(0, self.numberOfDeviceValues) :
				string = string + str(self.deviceValues[x]) + "\n"
			
		return string;
		
	
		
#Open up a UDP socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#Print menu to let the user choose to create a new robot or load one in
print "GRCS Initialization GRCv2"
print "a) Setup new robot"
print "b) Load existing robot"

#Get user choice
firstTimeSetup = raw_input()


#If the want to setup  a new robot
if (firstTimeSetup == 'a') :
	#Ask the user for the IP and the port of the robot
	ip = raw_input("What is the robot's IP address?(xxx.xxx.xxx.xxx) ")
	port = int(raw_input("What port is the robot listening on? (Default 80) " ))
	robot = (ip, port)

	#Ask user for the number of devices
	numOfDevices = int(raw_input('How many motors or other devices need to be controlled? '))

	#Setup device array
	for x in range(0, numOfDevices) :
		id = raw_input('What is the GRCS id of device %d? ' % (x+1))
		pin = raw_input('What is the pin # of device %d? ' % (x+1))
		inverse = raw_input('Does device %d need to be inversed? (0 or 1) : ' % (x+1))
		deviceType = raw_input('What is the device type of device %d? ' % (x+1))
		controlType = raw_input('How do you want to control device %d? 0 for joystick, 1 for button: ' % (x+1))
		controlNumber = raw_input('What is the number for the button or joystick depending on your selection? ')
		
		if (deviceType == "0" or deviceType == "3") :
			numberOfDeviceValues = 2;
		else :
			numberOfDeviceValues = 0;
			
		#Create device object 
		device = Device(id, pin, inverse, deviceType, controlType, controlNumber, numberOfDeviceValues)
		
		if (deviceType == "0") :
			device.deviceValues.append(raw_input('What is the lowest angle you want this servo to go to? (0 is the lowest): '))
			device.deviceValues.append(raw_input('What is the highest angle you want this servo to go to? (180 is the highest): '))
		elif (deviceType == "3") :
			device.deviceValues.append(raw_input('What is in Pin 1? '))
			device.deviceValues.append(raw_input('What is in Pin 2? '))

		#Append device to device array
		deviceArray.append(device)
		
	#Ask the user if they want to save the config
	save = raw_input('Do you want to save this robot? (Y or N) ')
	
	#Save the robot the given file
	if (save == 'Y' or save == 'y') :
		fileName = raw_input('What do you want the file to be named? ')
		f = open(fileName, 'w')
		f.write(robot[0] + '\n')
		f.write(str(robot[1]) + '\n')
		f.write(str(numOfDevices) + '\n')
		
		for x in range(0, numOfDevices):
			f.write(deviceArray[x].toFile())
			
		print "Done writing to file."
		
#If they want to load a robot in from file
elif (firstTimeSetup == 'b') :
	#Get file to load from
	fileName = raw_input('What file is the robot data is stored? ')
	
	#Open file
	f = open(fileName, 'r')
	
	#Get General robot info
	ip = (f.readline())[:-1]
	port = int((f.readline())[:-1])
	robot = (ip, port)
	numOfDevices = int((f.readline())[:-1])
	
	#setup device array
	for x in range(0, numOfDevices) :
	
		id = (f.readline())[:-1]
		pin = ((f.readline())[:-1])
		inverse = ((f.readline())[:-1])
		deviceType = ((f.readline())[:-1])
		controlType = ((f.readline())[:-1])
		controlNumber = ((f.readline())[:-1])
		numberOfDeviceValues = int((f.readline())[:-1])
		device = Device(id, pin, inverse, deviceType, controlType, controlNumber, numberOfDeviceValues )
		
		if (numberOfDeviceValues != 0) :
			for y in range(0,numberOfDeviceValues) :
				device.deviceValues.append((f.readline())[:-1])
		
		
		deviceArray.append(device)
else :
	print "Invalid option"
	sys.exit()
	
#Put the robot in programming mode
s.sendto('$',robot)

#Tell the robot how many devices need to be controlled
s.sendto("%d" %numOfDevices,robot)	

#Send deviceArray to robot
for device in deviceArray:
	s.sendto(device.id, robot)
	s.sendto(device.pin, robot)
	s.sendto(device.inverse, robot)
	s.sendto(device.deviceType, robot)
	s.sendto(str(device.numberOfDeviceValues), robot)
	
	for y in range(0,numberOfDeviceValues) :
		s.sendto(str(device.deviceValues[y]), robot)
	
	
#Begin pygame
pygame.init()

# Set the width and height of the screen [width,height]
size = [500, 250]
screen = pygame.display.set_mode(size)

pygame.display.set_caption("GRCv1.2")

#Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()

# Initialize the joysticks
pygame.joystick.init()
    
# Get ready to print
textPrint = TextPrint()

#Setup Joystick
joystick = pygame.joystick.Joystick(0)
joystick.init()

value = 0

# -------- Main Program Loop -----------
while done==False:
    # EVENT PROCESSING STEP	
    for event in pygame.event.get(): # User did something
        if event.type == pygame.QUIT: # If user clicked close
            done=True # Flag that we are done so we exit this loop
            
	# DRAWING STEP
	# First, clear the screen to white. Don't put other drawing commands
	# above this, or they will be erased with this command.
	screen.fill(WHITE)
	textPrint.reset()
	
	textPrint.prin(screen, "Generic Robot Control System")
	textPrint.prin(screen, "Robot IP and Port {}".format(robot) )
	
	#Get data from the joystick and send it to the robot then print it to screen
	for device in deviceArray:
		#Case for joystick control
		if (device.controlType == "0") :
			value = int(joystick.get_axis(int(device.controlNumber)) * 100 + 100)
			#Case for servos
			if (device.deviceType == "0") : 		
				if (value > 120 or value < 80) :
					s.sendto(device.id + '\n', robot)
					s.sendto(str(value) + '\n', robot)
					textPrint.prin(screen,device.id + "{}".format(value) )
				else :
					s.sendto(device.id + '\n', robot)
					s.sendto("100"+ '\n', robot)
					textPrint.prin(screen,device.id + "{}".format(100) )
			#Case for motor controllers
			
			elif (device.deviceType == "3" or device.deviceType == "1") :
				if (value > 120 or value < 80) :
					s.sendto(device.id + '\n', robot)
					s.sendto(str(value) + '\n', robot)
					textPrint.prin(screen,device.id + "{}".format(value) )
				else :
					s.sendto(device.id + '\n', robot)
					s.sendto("100" + '\n', robot)
					textPrint.prin(screen,device.id + "{}".format(100) )
	
	#Update the window
	pygame.display.flip()
	
	# Limit to 20 frames per second
	clock.tick(40)
    
#Send a stop to all devices
for device in deviceArray:
	s.sendto(device.id + '\n', robot)
	s.sendto('100' + '\n', robot)
	
#Close udp socket
s.close()

#Close window
pygame.quit ()
