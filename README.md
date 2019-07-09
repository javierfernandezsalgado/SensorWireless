# Introduction 



SensorWireless is a firmware to manage a  remote temperature's sensor and rele actuator. The
communication is performed throught a BLE device. The temperature
sensor is DALLAS family thermistor, and the rele is a common Arduino´s Rele device.


The project is based on the arduino nano architecture. The board uses
a Atmega 320P. 

The firmware mainly functionalities are described bellow:

- Keeping in memory the last N minutes of temperature.
- Switch on/off the Rele.
- Getting the avg temperature of last N seconds.
- TC/TM interface




