# arduino-pwm-fan-controller

The purpose of this project is to build a fan controller where fan speed is determined by a temperature sensor.

## Bill of Materials
- Arduino Nano
- PWM Fans
- DHT22
- 16x2 LCD

## Wiring
Wire as many PWM fans in parallel on the PWM outputs
![alt text](/img/breadboard.png)


## Code
### Required Libraries
- [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor)
- [DHT Library](https://github.com/adafruit/DHT-sensor-library)
