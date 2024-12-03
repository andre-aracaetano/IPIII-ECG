#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include <Wire.h>
#include <RtcDS1302.h>

// Configuração do ADC e RTC
Adafruit_ADS1015 myADS;
const int IO = 27;    // DAT do RTC
const int SCLK = 25;  // CLK do RTC
const int CE = 26;    // RST do RTC
ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);

// Variáveis para controle de gravação e arquivo
bool recording = false;
File myFile;
unsigned long startTime = 0;
const unsigned long interval = 60000; // Intervalo de 1 minuto para salvar no arquivo
int fileCount = 0;

// Função para obter data e hora formatadas
String getDateTimeString(const RtcDateTime& dt) {
  char datestring[20];
  snprintf_P(datestring, sizeof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Day(), dt.Month(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
  return String(datestring);
}

// Função para iniciar um novo arquivo
void startNewFile() {
  fileCount++;
  String fileName = "/ECG_" + String(fileCount) + ".txt";
  myFile = SD.open(fileName.c_str(), FILE_WRITE);
  if (!myFile) {
    Serial.println("Erro ao abrir o arquivo.");
    return;
  }

  RtcDateTime now = Rtc.GetDateTime();
  myFile.println("INICIO:");
  myFile.println(getDateTimeString(now));
  myFile.println("Leitura(ECG)");
  startTime = millis();
  Serial.println("Gravação iniciada.");
}

// Função para parar a gravação
void stopRecording() {
  if (myFile) {
    RtcDateTime now = Rtc.GetDateTime();
    myFile.println("FIM:");
    myFile.println(getDateTimeString(now));
    myFile.close();
    Serial.println("Gravação finalizada e arquivo salvo.");
  }
}

void setup() {
  Serial.begin(9600);
  myADS.begin();
  myADS.setGain(GAIN_ONE);
  myADS.setDataRate(250);

  // Configura RTC
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) Rtc.SetDateTime(compiled);
  if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);

  // Configura SD
  if (!SD.begin()) {
    Serial.println("Falha ao inicializar o cartão SD.");
    while (1);
  }

  Serial.println("Sistema pronto. Deseja iniciar? (sim/nao)");
}

void loop() {
  // Pergunta inicial para iniciar gravação
  static bool waitingForStart = true;
  if (waitingForStart) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      if (input.equalsIgnoreCase("sim")) {
        waitingForStart = false;
        recording = true;
        startNewFile();
      } else if (input.equalsIgnoreCase("nao")) {
        Serial.println("Encerrando sistema.");
        while (1); // Finaliza o programa
      }
    }
    return;
  }

  // Registro contínuo de leituras durante a gravação
  if (recording) {
    unsigned long currentTime = millis();
    if (currentTime - startTime >= interval) {
      stopRecording();
      startNewFile();
    }

    // Lê o valor do ECG
    int ecgValue = myADS.readADC_SingleEnded(0);
    myFile.println(ecgValue);
    Serial.println(ecgValue);
  }
}
