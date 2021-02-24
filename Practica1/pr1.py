
from grovepi import *
import time
import grovepi
import math
from grove_rgb_lcd import *
import random

# Connect the Grove Temperature Sensor to analog port A0
# SIG,NC,VCC,GND
sensor = 0
BOTON = 3   #Port for Button

# Setting the RGB LCD Screen
setRGB(0,255,0)
setText("Grove - LCD RGB Backlight")
time.sleep(2)

setText("Random colors")
for i in range(0,51):
    setRGB(random.randint(0,255),random.randint(0,255),random.randint(0,255))
    time.sleep(.1)
  
#time.sleep(1)
setText("Presiona el BOTON")
temp = grovepi.temp(sensor,'1.1')
setRGB(0,0,255) #RED
print("temp =", temp)
#setText(temp)

print("ENTRAR AL WHILE")        
pinMode(BOTON,"INPUT")     # Assign mode for Button as input

while True:
    print("while")
    print("TRY")
    button_status= digitalRead(BOTON)  #Read the Button status
    if button_status:   #If the Button is in HIGH position, run the program
        print("IF")
        #digitalWrite(buzzer_pin,1)                     
        # print "\tBuzzing"
        temp = grovepi.temp(sensor,'1.1')
        #print("temp =", temp)
        time.sleep(.5)
        setText("TEMP = " + str(int(temp)))
        setRGB(0,255,0)#GREEN
        time.sleep(.5)
    else:
        print("ELSE")
        setText("Presiona el boton")
        setRGB(255,0,0) #RED
        time.sleep(.5)