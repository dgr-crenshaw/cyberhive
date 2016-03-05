//Read temp and humidity from three DHT22 sensors
//Toggle between sensors using momentary switch
//display sensor-read with LED 1/2/3

#include "DHT.h" //sensor library

#define pinDHT01 3  // pin to read DHT22_01 -- pin 2 fried on my Uno R3
#define pinDHT02 4  // pin to read DHT22_02
#define pinDHT03 5  // pin to read DHT22_03

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (the the left) of the sensor to +5V
// Connect pin 2 (from the left) of the sensor to data input pin
// Connect pin 4 (from the left) of the sensor to ground

// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// 3.3V and 5V both work on Uno 3
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Doesn't seem necessary but included on board

// Initialize DHT sensors.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster processors.

DHT dht01(pinDHT01, DHTTYPE); //sensor 01
DHT dht02(pinDHT02, DHTTYPE); //sensor 02
DHT dht03(pinDHT03, DHTTYPE); //sensor 03

// Enable one of these two #includes and comment out the other.
// Conditional #include doesn't work due to Arduino IDE shenanigans.

#include "Button.h"

// One side of the momentary switch is attached to pin 9, the other to ground.
#define pinButton 9

Button switchSensorPress = Button(9, PULLUP);

// Use pin 6/7/8 to power LED sensor 1/sensor 2/sensor 3
#define pinSensor01 6
#define pinSensor02 7
#define pinSensor03 8

// Declare global variable to determine which led to light
int ledLight = 0; //start with nothing to simplify logic in loop()

// Function to turn all lights off
void lightsOff() {
  digitalWrite(pinSensor01, LOW);
  digitalWrite(pinSensor02, LOW);
  digitalWrite(pinSensor03, LOW);
}

//Function to select and turn on single light
void lightOn() {
  switch (ledLight) {
    case 0:
      ++ledLight; //increment value so next press turns on next light
      digitalWrite(pinSensor01, HIGH); //turn on sensor 1 light
      break;
    case 1:
      ++ledLight; //increment value so next press turns on next light
      digitalWrite(pinSensor02, HIGH); //turn on sensor 2 light
      break;
    case 2:
      ++ledLight; //increment value so next press turns on next light
      digitalWrite(pinSensor03, HIGH); //turn on sensor 3 light
      break;
    case 3:
      ledLight = 1; //reset value to 0 so next press turns on next light
      digitalWrite(pinSensor01, HIGH); //turn on sensor 1 light
      break;
  }
}

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
//#include <TinyWireM.h> // Enable this line if using Adafruit Trinket, Gemma, etc.

#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>

Adafruit_7segment matrixTemperature = Adafruit_7segment();
Adafruit_7segment matrixHumidity = Adafruit_7segment();

void setup() {

  pinMode(6, OUTPUT); //monitor 01
  pinMode(7, OUTPUT); //monitor 02
  pinMode(8, OUTPUT); //monitor 03

  //start the temperature and humidity sensors
  dht01.begin();
  dht02.begin();
  dht03.begin();

  matrixTemperature.begin(0x70); //start the 7 segment LED for temperature
  matrixHumidity.begin(0x71); //start the 7 segment LED for humidity
  matrixTemperature.setBrightness(0); //set the LED brightness low for indoor debug (0)/high for outdoor use (15)
  matrixHumidity.setBrightness(0); //set the LED brightness low for indoor debug (0)/high for outdoor use (15)
}

void writeValue(float temp, float humidity) {
  //show temperature on LED
  matrixTemperature.print(temp);
  
  matrixTemperature.writeDisplay();
  //show humidity on LED
  matrixHumidity.print(humidity);
  matrixHumidity.writeDisplay();
}

void failAllEights() {
  matrixTemperature.print(8888);
  matrixTemperature.writeDisplay();
  matrixHumidity.print(8888);
  matrixHumidity.writeDisplay();
}

void loop() {
  matrixTemperature.blinkRate(0); //For a reset if fault
  matrixHumidity.blinkRate(0); //For a reset if fault

  if (switchSensorPress.isPressed() && switchSensorPress.stateChanged()) {
    lightsOff(); //turn all lights off
    lightOn(); //turn the next light on
    delay(250); // .25 second delay to get finger off the switch

  }

  // Reading temperature or humidity takes about 250 milliseconds!t
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h01 = dht01.readHumidity();
  float h02 = dht02.readHumidity();
  float h03 = dht03.readHumidity();

  // Read temperature as Celsius (the default)
  //float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f01 = dht01.readTemperature(true);
  float f02 = dht02.readTemperature(true);
  float f03 = dht03.readTemperature(true);

  switch (ledLight) {

    case 1:
      //got a value?
      if (isnan(h01) || isnan(f01)) {
        failAllEights();
      }
      else {
        //show temperature/humidity on LED for sensor 1
        writeValue(f01, h01);
      }
      break;

    case 2:
      //got a value?
      if (isnan(h02) || isnan(f02)) {
        failAllEights();
      }
      else {
        //show temperature/humidity on LED for sensor 3
        writeValue(f02, h02);
      }
      break;

    case 3:
      //got a value?
      if (isnan(h03) || isnan(f03)) {
        failAllEights();
      }
      else {
        //show temperature/humidity on LED for sensor 3
        writeValue(f03, h03);
      }
      break;
  }
}
