#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>
#include "uptime.h"

#define DHTPIN 7
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
float temp = 0.0f, humidity = 0.0f;

const int tempProbePin = 7, fanSet1 = 9, fanSet2 = 10;
const int rs = 12, en = 13, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//configure Timer 1 (pins 9,10) to output 25kHz PWM
void setupPWMTimer() {
  //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << CS10) | (1 << WGM13);
  ICR1 = 320;
  OCR1A = 0;
  OCR1B = 0;
}

//equivalent of analogWrite on pin 9 - 80mm fans
void setPWMA(float f) {
  f = f < 0 ? 0 : f > 1 ? 1 : f;
  OCR1A = (uint16_t)(320 * f);
}

//equivalent of analogWrite on pin 10 - 120mm fans
void setPWMB(float f) {
  f = f < 0 ? 0 : f > 1 ? 1 : f;
  OCR1B = (uint16_t)(320 * f);
}

void setUptime() {
  uptime::calculateUptime();
  char buf [40];
  int n = sprintf(buf, "Up:%04i %02i:%02i:%02i", (int)uptime::getDays(), (int)uptime::getHours(), (int)uptime::getMinutes(), (int)uptime::getSeconds());
  lcd.setCursor(0, 0);
  lcd.print(buf);
  Serial.println(buf);
}

void setTemp() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  temp = event.temperature;

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  humidity = event.relative_humidity;
 
  lcd.setCursor(0, 1);
  lcd.print("T:"); lcd.print(temp, 1); lcd.print((char)223); lcd.print("C");
  lcd.setCursor(9, 1);
  lcd.print("H:"), lcd.print(humidity, 1);
  lcd.setCursor(15, 1);
  lcd.print("%");
}

void setFanSpeed() {
  if (temp < 24) {
    setPWMA(0.5f);
    setPWMB(0.25f);
  } else if (temp < 27) {
    setPWMA(0.60f);
    setPWMB(0.4f);
  } else if (temp < 30) {
    setPWMA(0.75f);
    setPWMB(0.5f);
  } else if (temp < 33) {
    setPWMA(1.0f);
    setPWMB(0.75f);
  } else {
    setPWMA(1.0f);
    setPWMB(1.0f);
  }
}

void setup()
{
  //enable outputs for Timer 1
  pinMode(fanSet1, OUTPUT); //80 mm fans
  pinMode(fanSet2, OUTPUT); //120 mm fan
  setupPWMTimer();
  setPWMA(0.5f); //set duty to 50% on pin 9
  setPWMB(0.5f); //set duty to 20% on pin 10

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("Starting up...");
  dht.begin();

  Serial.println(F("DHT22 Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void loop()
{
  delay(delayMS);
  setUptime();
  setTemp();
  setFanSpeed();
}
