#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <SPI.h>

Adafruit_ADS1015 myADS;
unsigned long t0;
int v;
String str;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  myADS.begin();
  myADS.setGain(GAIN_ONE);
  myADS.setDataRate(250);
  t0 = millis();
}
 
void loop() {

  // put your main code here, to run repeatedly:
  v = myADS.readADC_SingleEnded(0);
  str = millis() - t0;
  str += ' ';
  str += v;
  Serial.println(str);
}