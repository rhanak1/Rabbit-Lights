# Welcome to Rabbit Lights

In the last five years, LED pacing lights technology has become increasingly popular in the sport of track and field. However, these systems are very expensive to purchase and install, with only the most prestigious meets and institutions being able to afford. In recent years more systems have appeared that are more affordable to rent, but at the end of the day smaller schools at the NCAA D2/3 and NAIA level do not have the budget to afford such a system.

## The Goal

My goal with Rabbit Lights is to explore the feasibility and cost effectiveness of building such a system. I want to be able to build something that is not only affordable, but also of quality and a product that is worth being purchased. I am first bulding a small demo (roughly 10m worth of LEDs), that can easily be scaled up if it works. I also will be closely documenting the process so that if it does work I can share my findings and system with others looking for a similar solution.

## Current Progress

Right now I am waiting on the LEDs to arrive, but while I wait I am building the web application that will be used to control the speed of the lights. The configuration settings are then sent via WIFI to the ESP32 controller where it runs the timing logic and lights up the correct LEDs. Right now it just displays which LED should be lit so it is then just a matter of sending that instruction to the future LEDs. I am currently working on adding some more features to the web app, such as being able to add multiple lights (runners) and giving multiple options for the way the race/interval should be run (even split or negative split).
