/*
 v.1.1.1
   Transmitter for the meteorological station.
   Uses the RF24 module transmitter to send data from the sensors.
*/
// This code is more power efficient since it uses the low power library but is not tested
// UPDATE: CODE TESTED AND IT WORKS
#include "ClosedCube_HDC1080.h"
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include "RF24.h"
#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include "LowPower.h"


RF24 myRadio (7, 8);
byte addresses[][6] = {"0"};

struct package {
  unsigned long timeFromReset = 0;
  float   temperature ;
  float   hum;
  float   pressure;
  int16_t adc0;
  int16_t adc1;
};

typedef struct package Package;
Package data;

ClosedCube_HDC1080 hdc1080;
Adafruit_BMP085 bmp;
Adafruit_ADS1115 ads;

unsigned long previousMillis = 0;
int           RFchannel      = 115;
//long          LOOPdelay      = 5000;

void setup() {
  // Start serial port for info display
  Serial.begin(115200);
  delay(1000);

  // Initialize radio transmission module
  myRadio.begin();
  myRadio.setChannel(      RFchannel    );
  myRadio.setPALevel(      RF24_PA_MAX  );
  myRadio.setDataRate(     RF24_250KBPS );
  myRadio.openWritingPipe( addresses[0] );
  delay(1000);

  // Initialize sensors
  hdc1080.begin(0x40);
  bmp.begin();
  ads.begin();
}

void loop() {

  data.timeFromReset++;
  data.temperature   = hdc1080.readTemperature();
  data.hum           = hdc1080.readHumidity();
  data.pressure      = bmp.readPressure() / 100.0;
  data.adc0          = ads.readADC_SingleEnded(0);
  data.adc1          = ads.readADC_SingleEnded(1);

  // Send data to RF
  myRadio.write(&data, sizeof(data));

  printal(); // print info (non critical)
  delay(2000);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}


// Function to print data to serial
void printal() {
  Serial.println( data.timeFromReset );
  Serial.println( data.temperature   );
  Serial.println( data.hum           );
  Serial.println( data.pressure      );
  Serial.println( data.adc0          );
  Serial.println( data.adc1          );
  Serial.println( sizeof(data)       );
  Serial.println();
}

// END
