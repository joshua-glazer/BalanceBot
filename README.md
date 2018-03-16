## Welcome to the BalanceBot homepage

This project is part of the class MEC8358: Mechatronics II at Polytechnique Montreal. It's part of the mechatronics orientation for mechanical engineering students. The goal of the project is to understand how to build and design a mechatronic system, notably in the choice of system parameters, dynamic system modelling, motor control, PID tuning, and microcontroller programming.

Two-wheel balancing robots have become quite popular in the past few years, so we will be borrowing our ideas from previous projects and research papers. You can find links to videos and research papers on balance robots [here](references).

### Project Components

The BalanceBot feedback loop will consist of actuators, sensors, a microcontroller and other control circuitry. It will also need a support structure and other components. Here's a preliminary list of required parts:

- [Geared DC motors](https://www.robotshop.com/ca/en/solutions-cubed-m12v200-motor-rear-shaft.html): The motors are an incredibly important choice because most of the robot's components depend on what motors are chosen. I actually wanted to buy two of [these](https://www.robotshop.com/ca/en/12v-485rpm-econ-metal-gearmotor.html) motors, but they were out of stock, so I went with M12V200 motors. I was looking out for a few different things. First of all, the combined stall torque of both motors needed to be high enough that the robot would be able to hold up its own weight. These motors are rated at 50oz-in each, and I think a total of 100oz-in should be good enough. Second, other guides tend to prefer motors that are at least 200 RPM, so this manages to scrape by. The Actobotics motor is rated for 485RPM though... :'( Next, the stall current. This motor's stall current is a low 1.5A, which is actually super great because it means I was able to buy a super cheap motor driver. More on that later. The shaft diameter also plays an impact on the design because I need to buy wheels to fit on the shaft. This one's got a 6mm diameter with a D-shape (pretty standard). Finally, this motor has a rear shaft for an encoder... That said, encoders are pretty pricey so I doubt we'll end up using any. Side note: motors are expensive! I managed to get the cheapest ones that matched the guesstimate torque I wanted and they cost me $16 each... I would have gotten something cheaper if I planned on making a mini robot because I could have used Pololu microgear motors.
- Wheel assembly: To fit the 6mm shaft, I went with basically the only option I had - Banebots wheels. The wheel and the hub are actually separate components. The [hubs](https://www.robotshop.com/ca/en/hex-hub-s40-6mm-1wide.html) were picked because they're for 6mm shafts, and the [wheels](https://www.robotshop.com/ca/en/rb-ban-94-orange-wheel.html) were chosen because they're the largest diameter (2 7/8") that will fit on the hubs. Finally, I found [mounting brackets](https://www.robotshop.com/ca/en/dc-geared-motor-bracket-spg30-spg50.html) that matched the motor's screw holes.
- [Motor drivers](https://www.robotshop.com/ca/en/motor-driver-dual-tb6612fng-v2-headers.html): The actobotics motors I wanted to buy have a 3.8A stall current. This means I'd need a driver that can handle over 4A per motor, or over 8A total, so I was looking at getting [this](https://www.robotshop.com/ca/en/cytron-10a-5-25v-dual-channel-dc-motor-driver.html) Cytron driver. It costs $30, though, so in a way I guess the cheaper motors that draw less current were a much better idea. The driver is much less heavy duty but it only costs $30 and is probably all I need.
- [Battery](https://hobbyking.com/en_us/zippy-flightmax-1800mah-3s1p-40c.html?___store=en_us): My friend lent me two different LiPo batteries. One is 3S and the other is 4S. I'm leaning towards the 4S so I can squeeze out a bit more RPMs by powering the motor over its 12V nominal rating.
- [Microcontrollers](https://www.amazon.ca/Arduino-Uno-R3-Microcontroller-A000066/dp/B008GRTSV6): You all know the deal already. I've got one of these on the robot itself controlling everything. If I finish everything in time, I'll be plugging in one of [these bad boys](https://www.robotshop.com/ca/en/wifi-serial-transceiver-module-esp8266.html) through [this](https://www.robotshop.com/ca/en/cytron-4-channels-logic-converter.html) logic shifter. I'll have a matching pair of these in my hands, connected to a [Wii Chuck](https://www.robotshop.com/ca/en/wiichuck-adapter-arduino.html) adapter so that i can control the robot using a Nintendo Wii Nunchuck. I already own the 2 arduino unos and the nunchuck so I had to buy the wifi modules and the wii chuck adapter.
- [Acceleromoeter/gyroscope combo](https://www.amazon.ca/Robojax-MPU-6050-Gyroscope-Accelerometer-Raspberry/dp/B079Z1W6NS/): I'm not sure exactly where I bought this from, but the picture's the same. This is an MPU-6050 accelerometer-gyroscope combo. It has 6 axes (3 for gyro, 3 for accel), and I'm currently using a Kalman filter to combine the data and give me the angle of the board.
- Support structure: We're building the structure out of what I think is high density fiberboard. My partner is laser cutting the boards and we'll be screwing it together with brackets and stuff. If we can we'll try to add an artistic touch on it, too.
- Wires etc: We'll have to finalize the circuitry eventually. For now we're planning on breadboards and jumper cables, but I need to be careful with how I connect the LiPo battery to the rest of our circuit. I'll need to figure that out soon.

Most of these parts, are/were purchased from [RobotShop](https://www.robotshop.com).

![Image](Pictures/robotshop-logo-345x100-en.gif)

### Under Construction

This website is still being set up, so lots of things are still tutorial text.

Markdown tips:

```markdown
Syntax highlighted code block

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text
```

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/vashmata/BalanceBot/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

###### Go back:

[Projects Homepage](https://vashmata.github.io)
