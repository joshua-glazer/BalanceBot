## Welcome to the BalanceBot homepage

This project is part of the class MEC8358: Mechatronics II at Polytechnique Montreal. It's part of the mechatronics orientation for mechanical engineering students. The goal of the project is to understand how to build and design a mechatronic system, notably in the choice of system parameters, dynamic system modelling, motor control, PID tuning, and microcontroller programming.

Two-wheel balancing robots have become quite popular in the past few years, so we will be borrowing our ideas from previous projects and research papers. You can find links to videos and research papers on balance robots [here](references).

### Dynamic Analysis

The first step to building one of these robots is to understand the dynamics in play which guide its movement. The Balance Bot is modeled after an inverted pendulum sitting on either a wheel or a sliding box. Theoretically, if the robot was aligned just right, it would be able to stand up on its own, but any disturbances would cause it to fall over. We thus say that this a system on the limit of stability, as any disturbance causes it to become unstable. A detailed analysis of the math can be found [here](dynamics).

### Project Components

The BalanceBot feedback loop will consist of actuators, sensors, a microcontroller and other control circuitry. It will also need a support structure and other components. You can find links to the actual parts I bought as well as explanations for my choices [here](components). Here's a preliminary list of required parts:

- Motors
- Microcontroller(s)
- Sensor(s)
- Circuitry
- Battery
- Support structure
- etc

Most of these parts were purchased from [RobotShop](https://www.robotshop.com).

![Image](Pictures/robotshop-logo-345x100-en.gif)

### Structural Design

The structure has an influence on the dynamics of the system, as explained in the [section on dynamics](dynamics). Even more importantly, though, the structure must be able to hold all the components safely, as well as to allow easy access to the critical circuitry. An explanation of the design can be found [here](structure).

### Circuit Design

Now that all the components have been chosen and the structure has been designed, a circuit can be designed to fit into the structure. That being said, the structure did have to take the circuit into account - the two processes interact with each other to some extent. After having prototyped the circuit on a breadboard, the circuit was soldered onto a protoboard for semi-permanence. An explanation of the different design steps can be found [here](circuitry).

### Code

The second-to-last stage in the project is to program the robot. There are two main segments to the code (plus a third, bonus segment if time permits). The first segment deals with the data obtained from the MPU6050 module. The module sends accelerometer and gyroscope data to the Arduino Uno, and the data is processed with a Kalman filter in order to obtain good angle data. The second segment is the feedback loop which takes the angle data as an input and outputs a signal which is used to control the motors. The details are found [here](code).

### PID Tuning

Finally, we test the performance of the BalanceBot using the theoretical PID values and then tune the robot manually to make it actually work. The details are [here](tuning).

### Bonus: Remote Control

If I have the time for it, I'd like to be able to control the BalanceBot remotely. It's for this reason that I bought two WiFi modules to interface with the Arduino Unos I already own, as well as a Wii Chuck chip to interface my Wii Nunchuck with one of the Arduino Unos. If I ever get this far, the explanations will be [here](wifi).

###### Go back:

[Projects Homepage](https://vashmata.github.io)
