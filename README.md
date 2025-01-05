# <Speedometer>

## Description

This is the code for a digital speedometer I built for one of my motorcycles. The motivation is to have a customizable speedometer made with cheap and readily available components, with clear instructions on how to implement it for anyone else's own applications. Also I wanted to improve my C/C++ and get better at electronics.


## Table of Contents 

If your README is long, add a table of contents to make it easy for users to find what they need.

- [Installation](#installation)
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

## Installation

This project was developed mostly using the excellet Wokwi, a web based arduino emulator and IDE. [Click here](https://wokwi.com/projects/395529083108023297/) to see an emulation of it working, and mess around with it over there.
To get this working on actual hardware, you will need the Arduino IDE and a compatible micro controller with the necessary hardware assembled. Once you have that, clone this repo into your Arduino folder, open the project, and upload the code to your device. 
That is a simplified overview, but I am happy to advise if you are interested in building one of these. 

## Parts List

Main components:
- Arduino Nano
- KY-040 Rotary Encoder module
- KY-003 Hall Effect Sensor Module
- HD44780 with I2C control board
- Mini360 power module

Nice to have:
- Perf board
- Jumper wires
- Neutral cure silicone


## Usage

Ultimately usage is simple. It taps into your vehicle's 12v power and simply shows you how fast you are going, on a 4x20 hd44780 display. Units of measure can be selected using the jogwheel, and a 0-60 timer is also included just for fun.
To use the 0-60 timer, come to a stop, press the jogdial button and take off. The first reading of the hall effect sensor will start the timer, and when the timer sees 60 (km/h or mph) it will stop and show your time, until you push the jog dial button again.


## Credits

This project relies on a few external arduino libraries. Shoutout to everyone involved in those.
Arduino.h - the arduino core library.
LiquidCrystal_I2C - a library that handles communication with HD44780 displays via I2C.
LCDBigNumbers - an awesome library that lets you put big numerical digits on hd44780 displays.



## License

[GPL V3](https://choosealicense.com/licenses/gpl-3.0/).
