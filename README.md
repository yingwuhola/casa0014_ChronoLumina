# CASA0014 ChronoLumin--LightMag Harmony

LightMag Harmony means that ChronoLumina adjusts the brightness and colour under the joint action of light and magnetic fields to present a harmonious lighting effect.

Under normal conditions, the light is soft yellow. The brightness of the light will change with the change of ambient light, with default mode and reading mode. The colour tone of the light will be changed according to the change of the magnetic field to meet the needs of different colour, warm yellow or cold white.


# Inspiration
Because the lighting in my dormitory is very dim, my eyes often feel strained after looking at the electronic screen for a while. That’s why I really hope to have a light  that allows me to freely adjust the brightness. I would prefer higher brightness for studying and a softer, more natural light in daily life.

# What I have learned
I found a Hall sensor in the lab, and after searching online, I found it has many applications in our daily lives, such as wireless headphone cases and speed dial in cars. It can detect changes in the strength and direction of magnetic fields.


# Sketch of the prototype
<img src="image/package.jpg" alt="" width="600">
<img src="image/prototype_1.jpg" alt="" width="600">
<img src="image/prototype_2.jpg" alt="" width="600">

# The hardware used: 
Arduino MKR1010; 

LDR Light Sensor; 

Hall Sensor SS49E

## Attempts in using the hall sensor
<img src="image/hallsensor_test.jpg" alt="" width="600">
<img src="image/hallsensor.jpg" alt="" width="400">


# Breadboard Circuit
<img src="image/circuit_2.jpg" alt="" width="600">


# Stripbpard Circuit Diagram
<img src="image/circuit_stripboard_2.jpg" alt="" width="600">


# How it works
-A LDR Light Sensor is used to detect ambient light level to control the brightness of the Chrono Lumina Blinker.  
In the default mode, when strong ambient light(analog value > 800) is detected, the blinker will become brighter(brightness = 119), providing clear illumination.
When moderate ambient light(400 < analog value < 800) is detected, the blinker’s brightness is soft(brightness = 90), simulating natural light.
When low ambient light(analog value < 400) is detected, the blinker will dim(brightness = 50) to act as an ambient light.  
In reading mode, when strong ambient light(analog value > 800) is detected, the blinker is dimmed(brightness = 50) to act as an ambient light.
When moderate ambient light(400 < analog value < 800) is detected, the blinker’s brightness is soft(brightness = 90), simulating natural light.
When low ambient light(analog value < 400) is detected, the blinker’s  brightness becomes larger(brightness = 119), providing clear illumination.

-A Hall Sensor is used to detect changes in the magnetic field to change the color of Chrono Lumina Blinker. Under a normal magnetic field, the colour of the blinker is yellow. When a strong magnetic field(analog value > 550) is detected, the blinker changes to warm yellow, and when a weak magnetic field(analog value < 350) is detected, it changes to cold white.


## The brightness of the light will change with the change of ambient light.
### 1)Default Mode
When the ambient light becomes stronger, the light will be brighter  
<img src="video/default1.GIF" alt="" width="250">  
when the ambient light is darker, the light will dim.   
<img src="video/default2.GIF" alt="" width="250">

### 2)Reading Mode
when the ambient light becomes stronger, the light will dim
<img src="video/read1.GIF" alt="" width="250">
when the ambient light is dim, the light will be brighter
<img src="video/read2.GIF" alt="" width="250">

## The colour of the light will change with the magnet.
### 1)warm yellow
When the N pole of the magnet approaches, the light turns warm yellow. 
<img src="video/warm.GIF" alt="" width="250">
### 2)cold white
When the S pole of the magnet approaches, the light turns cold white.
<img src="video/cold.GIF" alt="" width="250">


## full video    
<img src="video/full-video.GIF" alt="" width="400">

# Reference
Principle and Application of Hall sensor: https://www.electronics-tutorials.ws/electromagnetism/hall-effect.html

Characteristics of light sensor: https://www.electronics-tutorials.ws/io/io_4.html
