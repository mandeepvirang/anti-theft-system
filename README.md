Objective\
The main objective of this project is to design and implement an IoT based anti-theft system. When user leaves his house, he have to turn the system. It will monitor all the floor and any motion near the sensor will activate the device which will then notify the user about security breach and will capture the image of intruder and send it to the user.

Implemented Attributes\
    PIR Sensor to detect motion.\
    LED light to give visible alert.\
    Real time data is being controlled by Esp32.\
    Collected data is being sent to Thingspeak IoT platform(cloud) via internet for storing and processing purposes.\
    A mobile application to be used to access real time data.\
    Web Apis and webhooks are used to send SMS to user's mobile Phone.\
    SMTP protocol is used to send image of intruder via Email to User.

Hardware used\
    ESP32-CAM microcontroller\
    OV2640 camera\
    FTDI Programmer\ 
    Micro-SD Card\
    LED bulb\
    PIR Sensor

Software Requirements\
    Arduino IDE\
    Thingspeak Cloud IoT Platform.\
    Email Accounts to send and receive mails.\
    Mobile application to access Thingspeak server.
