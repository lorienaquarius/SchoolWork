# SYSC4001 Assignment 1 Instructions

## Background
This assignment is designed to emulate an IoT setup. It includes several client devices in the form of sensors, each of which have a threshold that they sense to. Devices communicate with a controller server that manages the devices and checks if thresholds have been reached. If they have, a cloud is notified, as well as an initialized actuator takes an action.

## Running the assignment

To run the assignment, start with opening a new terminal, and running ./compile to compile all files and open the appropriate terminals.

## Order
 - ./cloud
 - ./controller
 - ./actuator [name]
 - ./device [name] [threshold value]
 - ./device [name] [threshold value]

## What to expect
The cloud process will alert the user whenever a device reaches the threshold. The controller process will notify the user when an actuator is registered, and when a sensor sends information. The actuator will notify the user when it is activated. Devices all start at a value of 1, and will increment their value by 1 every second, until the threshold 