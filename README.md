# IoT
This project was made during my Erasmus mobility period for the Internet of Things / Embedded Systems course at the University of Bologna in the year 2020/2021/2 semester. The project consists of a mandatory report written in LaTeX and a project presentation. For the project, I implemented a smart parking monitoring system for garages. The solution is able to measure the distance and the motion of each parking slot and transmit the data to a database from which it visualizes the data.

As an IoT project, it contains multiple parts to implement a full IoT stack, from sensor data acquisition and processing until data analysation and visualization. The project requires the following hardware:

 - ESP32 Wroom microcontroller
 - HC-SR501 PIR motion sensor
 - HC-SR04 ultrasonic sensor
 - jumpers, breadboard and USB 3.0 cable
 
For the development I used the following environments, softwares and services:

 - Arduino IDE: [link](https://www.arduino.cc/en/software)
 - ESP32 Board Installation in Arduino IDE: [link](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
 - Pubsubclient for Arduino: [link](https://pubsubclient.knolleary.net/api#subscribe)
 - Mosquitto Broker: [link](https://mosquitto.org/download/)
 - PyCharm Editor: [link](https://www.jetbrains.com/pycharm/)
 - paho-mqtt extension for mosquitto support with Python: [link](https://pypi.org/project/paho-mqtt/)
 - Influx DataBase: [link](https://www.influxdata.com/) [link2](https://docs.influxdata.com/influxdb/cloud/tools/client-libraries/python/)
 - Grafana Dashboard: [link](https://grafana.com/)

For the application to work you need to go through the following steps:

1. assemble the hardware in the following manner: ![mc layout](https://github.com/balintbujtor/IoT/blob/main/Project%20Images/mc_layout.png)
3. run and export the software to the microcontroller
4. start the mosquitto broker
5. start running the mosquitto client in the Python file
6. start the influxDB client
7. you can check the logs and the visualisation on the InfluxDB and Grafana dashboards (I used the local IP addresses to showcase my project. 8086 for InfluxDB and 3000 for Grafana). Of course, you can use them connected to the internet for an online database.

I used the following sites for help and troubleshooting:
 - [ESP32 Arduino Extension installation troubleshooting](https://randomnerdtutorials.com/esp32-troubleshooting-guide/)
 - [ESP32 Pin Layout](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
 - [Ultrasonic sensor usage](https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6)
 - [PIR sensor usage](https://create.arduino.cc/projecthub/electropeak/pir-motion-sensor-how-to-use-pirs-w-arduino-raspberry-pi-18d7fa)
 - [MQTT Tutorial](http://www.steves-internet-guide.com/)

However, the project is working and was accepted in the exam with 28/30 points, it can be further improved, for example with the following features:
 - Enable the changing of the threshold value remotely, from the dashboard or from the Python client.
 - Do not transmit the occupancy datastream from the microcontroller but calculate it in the dashboard based on the threshold value.
 - Execute data prediction, forecast based on the incoming data in Grafana.
 
Some images of the finished project:

- The assembled hw: ![hw](https://github.com/balintbujtor/IoT/blob/main/Project%20Images/IMG_6689.jpg)
- Visualization and queries in Influx Database: ![dashboard](https://github.com/balintbujtor/IoT/blob/main/Project%20Images/influx%20dashboard.png) ![query](https://github.com/balintbujtor/IoT/blob/main/Project%20Images/influx%20query.png)
- Dashboard in Grafana: ![grafana1](https://github.com/balintbujtor/IoT/blob/main/Project%20Images/grafana%20part%201.png) ![grafana2](https://github.com/balintbujtor/IoT/blob/main/Project%20Images/grafana%20part%202.png)
