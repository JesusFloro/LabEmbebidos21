
from grovepi import *
import time
import grovepi
import math
from grove_rgb_lcd import *
import random

# Connect the Grove Temperature Sensor to analog port A0
# SIG,NC,VCC,GND
sensor = 0
button = 4      #Port for Button

# Setting the RGB LCD Screen
setRGB(0,255,0)
setText("Grove - LCD RGB Backlight")
time.sleep(2)

setText("Random colors")
for i in range(0,51):
    setRGB(random.randint(0,255),random.randint(0,255),random.randint(0,255))
    time.sleep(.1)
time.sleep(1)

# ascii char 255 is the cursor character
setRGB(255,255,255)
setText(chr(255)*32)
time.sleep(2)
          
pinMode(button,"INPUT")     # Assign mode for Button as input
while True:
    try:
        button_status= digitalRead(button)  #Read the Button status
        if button_status:#If the Button is in HIGH position, show the temperature on the screen with a green bg
            temp = grovepi.temp(sensor,'1.1')
            toWrite = "Temp = " + str(int(temp)) + " C"
            setText(toWrite)
            setRGB(0,255,0)
            time.sleep(.5)
        else: #If Button is in Off position, show a message on the screen with a red bg
            setText("Presiona el boton") #Quiza anañadir un \n
            setRGB(255,0,0)
            time.sleep(.5)
    except KeyboardInterrupt:   # Stop the buzzer before stopping
        #digitalWrite(buzzer_pin,0)
        break
    except (IOError,TypeError) as e:
        print("Error")
