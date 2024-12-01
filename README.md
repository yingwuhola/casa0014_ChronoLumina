# casa0014_ChronoLumina

An eye-friendly light controller which can automatically adjusts the brightness of the Chrono Lumina Blinker based on the ambient light level. 

Under normal conditions, the light is soft yellow. When the ambient light becomes stronger, the light will dim; when the ambient light is dark, the light will become brighter. 
You can also change the color of the light with magnets.

# Sketch of the prototype
![](image/package.jpg)
![](image/prototype_1.jpg)

# The hardware used: 
Arduino MKR1010; 

LDR Light Sensor; 

Hall Sensor SS49E

Attempts in Using the Hall Sensor
![](image/hallsensor_test.jpg)
![](image/hallsensor.jpg)


# Breadboard Circuit
![](image/circuit.jpg)

# Stripbpard Circuit Diagram
![](image/circuit_stripboard.jpg)

# How it works
1. A LDR Light Sensor is used to detect ambient light level to control the brightness of the Chrono Lumina Blinker. 
When strong ambient light(analog value > 800) is detected, the blinker is dimmed(brightness = 50) to act as an ambient light.
When moderate ambient light(400 < analog value < 800) is detected, the blinker’s brightness is soft(brightness = 90), simulating natural light.
When low ambient light(analog value < 400) is detected, the blinker’s  brightness becomes larger(brightness = 119), providing clear illumination.

2. A Hall Sensor is used to detect changes in the magnetic field to change the color of Chrono Lumina Blinker. Under a normal magnetic field, the colour of the blinker is yellow. When a strong magnetic field(analog value > 550) is detected, the blinker changes to green, and when a weak magnetic field(analog value < 350) is detected, it changes to red.

# Reference
Principle and Application of Hall sensor: https://www.electronics-tutorials.ws/electromagnetism/hall-effect.html

Characteristics of light sensor: https://www.electronics-tutorials.ws/io/io_4.html
