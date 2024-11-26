#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <SPI.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

char valorRecebido;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ECG Bluetooth"); 
  Serial.println("O dispositivo já pode ser pareado!");

}

void loop() {
    valorRecebido =(char)SerialBT.read();

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  
  }
  if (SerialBT.available()) {
    
    if(valorRecebido == '1')
    {
     SerialBT.println("LED 1 ligado:");
     Serial.println("LED 1 ligado:");
    }
    if(valorRecebido == '2')
    {
     SerialBT.println("LED 1 desligado:");
     Serial.println("LED 1 desligado:");
    }    
  
  }
  delay(30);
}