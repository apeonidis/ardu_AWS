// v1.1.3
#include <SPI.h>
#include "RF24.h"

RF24 myRadio (7, 8);
struct package
{
  unsigned long timeFromReset;
  float temperature;
  float hum;
  float pressure;
  int16_t adc0;
  int16_t adc1;
};
struct package1
{
  float dewP;
  float cloudBase;
  float adc0Volts;
  float adc1Volts;
};


byte addresses[][6] = {"0"};



typedef struct package Package;
typedef struct package1 Package1;
Package data;
Package1 data1;

void setup()
{
  Serial.begin(9600);
  delay(1000);

  myRadio.begin();
  myRadio.setChannel(115);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
}


void loop()
{

  if ( myRadio.available()) {
    readData();
    calcData();
    Serial.print(data.timeFromReset);Serial.print(",");
    Serial.print(data.temperature);Serial.print(",");
    Serial.print(data.hum);Serial.print(",");
    Serial.print(data.pressure);Serial.print(",");
    Serial.print(data1.dewP);Serial.print(",");
    Serial.print(data1.cloudBase);Serial.print(",");
    Serial.print(data1.adc0Volts);Serial.print(",");
    Serial.println(data1.adc1Volts);
  }
}
void readData() {
  while (myRadio.available())
  {
    myRadio.read( &data, sizeof(data) );
  }
}
void calcData() {
data1.dewP = dewPointFast(data.temperature, data.hum);
data1.cloudBase = ((data.temperature - data1.dewP) * 400.0) * 0.3048;
data1.adc0Volts = data.adc0 * (6.144 / 32767.0);
data1.adc1Volts = data.adc1 * (6.144 / 32767.0);
}
//https://en.wikipedia.org/wiki/Dew_point#Calculating_the_dew_point
double dewPointFast(double celsius, double humidity) {
  double a = 17.67;
  double b = 243.5;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}
