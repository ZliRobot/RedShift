# RedShift

Telescope movement controll firmware for amateur telescope makers

RedShift is a program made for ESP8266 connected to two stepper motors that controll telescope azimuth and elevation. It hosts web server with a web app that allows to control your telescope using a web browser.

## Prerequisites

- Arduino IDE  https://www.arduino.cc/en/software (configured to work with ESP8266 board https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/ )   
- ESP8266WiFi library instaled via arduino IDE tools->Manage Libraries   
- ESPAsyncTCP https://github.com/me-no-dev/ESPAsyncTCP   
- ESPAsyncWebServer https://github.com/me-no-dev/ESPAsyncWebServer/   
- AsyncElegantOTA https://github.com/ayushsharma82/AsyncElegantOTA   
- LittleFS https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/   

- Telescope with AltAz or Dobson mount and stepper motors   

## Instructions
Example for wiring when using NodeMcu and ULN2003 stepper motor drivers is given [here](https://github.com/ZliRobot/RedShift/blob/main/NodeMcu-ULN2003%20wiring.JPG)

## Roadmap

Project is accepting contributions!

Since RedShift is a project in progress, following features are planned for implementation:
 - Manual movement: have a simple web interface that can be used as a joystick to controll movement **<- Currently here**   
 - Tracking: after telesope is moved to the desired position, it continues to move to compensate for Earth rotation   
 - Aiming: allows user to choose an object from the catalogue and automaticaly aim to it.   
 - Solar system objects: using JPL Horizons API https://ssd.jpl.nasa.gov/horizons/   
 
 
 ## Thanks
 
 Much of the work was derived from projects:   
 - https://randomnerdtutorials.com/stepper-motor-esp8266-websocket/   
 - https://github.com/google/web-starter-kit   
 - https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/   
 - http://eeshop.unl.edu/pdf/Stepper+Driver.pdf  
 
***
## [Buy me a coffee](https://buymeacoffee.com/ZliRobot)
