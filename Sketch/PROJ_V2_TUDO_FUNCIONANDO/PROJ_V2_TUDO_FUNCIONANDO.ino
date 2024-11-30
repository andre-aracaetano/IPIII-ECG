#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

BluetoothSerial SerialBT;
Adafruit_ADS1015 myADS;
char valorRecebido;

// Definição dos botões
#define BUTTON_PIN 12
#define BUTTON_PIN_BLUETOOTH 14
#define SCREEN_ADDRESS 0x3C // Endereço do display
#define SCREEN_WIDTH 128    // Largura do display
#define SCREEN_HEIGHT 32    // Altura do display

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variáveis para controle da gravação e contagem de arquivos
bool recording = false;
bool bluetoothAtivo = false;
int fileCount = 0;
File myFile;
unsigned long startTime = 0;
unsigned long currentTime = 0;
unsigned long interval = 60000; // Intervalo de 1 minuto para salvar no arquivo

// Estados do sistema
enum Estado { LENDO, PARADO, INICIANDO, BLUETOOTH, CONECTADO };
Estado estadoAtual = INICIANDO;

bool heartVisible = true;  // Controle da visibilidade do coração para animação
unsigned long previousMillis = 0;
const long heartInterval = 500; // Intervalo de piscamento do coração (500ms)

// Bitmaps para o coração e outros símbolos
const unsigned char heartBitmap[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x07, 0xF8, 0x3F, 0xE0, 0x0F, 0xFE, 0x7F, 0xF0,
  0x1F, 0xFE, 0x7F, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x3F, 0xFF, 0xEF, 0xF8, 0x3F, 0xFF, 0xCF, 0xFC,
  0x3F, 0xFF, 0xC7, 0xFC, 0x3F, 0xFF, 0x87, 0xFC, 0x3F, 0xFF, 0x83, 0xFC, 0x3F, 0xFF, 0x93, 0xFC,
  0x1F, 0xE7, 0x31, 0xF8, 0x1F, 0xE7, 0x39, 0xF8, 0x10, 0x02, 0x38, 0x00, 0x08, 0x1A, 0x7C, 0x00,
  0x07, 0xF8, 0x7F, 0xE0, 0x03, 0xF8, 0xFF, 0xC0, 0x01, 0xFC, 0xFF, 0x80, 0x00, 0xFD, 0xFF, 0x00,
  0x00, 0x7F, 0xFC, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Bitmap do círculo representando o Bluetooth
const unsigned char bluetoothBitmap[] PROGMEM = {
  0xff, 0xf8, 0x1f, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0x00, 0x00, 0xff, 
	0xfe, 0x00, 0x00, 0x7f, 0xfe, 0x01, 0x00, 0x7f, 0xfc, 0x03, 0x80, 0x3f, 0xfc, 0x03, 0xc0, 0x3f, 
	0xfc, 0x03, 0xf0, 0x3f, 0xf8, 0x03, 0x38, 0x1f, 0xf8, 0x03, 0x1c, 0x1f, 0xf8, 0x33, 0x0c, 0x1f, 
	0xf8, 0x3b, 0x3c, 0x1f, 0xf8, 0x1f, 0x70, 0x1f, 0xf8, 0x0f, 0xe0, 0x1f, 0xf8, 0x07, 0xc0, 0x1f, 
	0xf8, 0x07, 0xc0, 0x1f, 0xf8, 0x0f, 0xe0, 0x1f, 0xf8, 0x1f, 0x70, 0x1f, 0xf8, 0x3b, 0x3c, 0x1f, 
	0xf8, 0x33, 0x0c, 0x1f, 0xf8, 0x03, 0x1c, 0x1f, 0xf8, 0x03, 0x38, 0x1f, 0xfc, 0x03, 0xf0, 0x3f, 
	0xfc, 0x03, 0xc0, 0x3f, 0xfc, 0x03, 0x80, 0x3f, 0xfe, 0x01, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x7f, 
	0xff, 0x00, 0x00, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xf8, 0x1f, 0xff
};

// Função para iniciar um novo arquivo
void startNewFile() {
  fileCount++;
  String fileName = "/ECG_" + String(fileCount) + ".txt";
  myFile = SD.open(fileName.c_str(), FILE_WRITE);
  if (!myFile) {
    Serial.println("Erro ao abrir o arquivo.");
    return;
  }
  Serial.print("Iniciando arquivo: ");
  Serial.println(fileName);
  myFile.println("Tempo(ms),Leitura(ECG)");  // Cabeçalho do arquivo
  startTime = millis();
}

// Função para parar de gravar e fechar o arquivo atual
void stopRecording() {
  if (myFile) {
    myFile.close();
    Serial.println("Arquivo salvo.");
  }
}

// Função para desenhar o coração e o estado no display
void displayHeartWithText(const char* message, bool heartVisible) {
  display.clearDisplay();
  if (heartVisible) {
    display.drawBitmap(0, 0, heartBitmap, 32, 32, WHITE); // Exibe o coração
  }
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(50, 16); // Ajuste a posição do texto
  display.print(message);
  display.display();
}

// Função para desenhar o símbolo de Bluetooth no display
void displayBluetoothWithText(const char* message, bool bluetoothVisible) {
  display.clearDisplay();
  if (bluetoothVisible) {
    display.drawBitmap(0, 0, bluetoothBitmap, 32, 32, WHITE); // Exibe o símbolo do Bluetooth
  }
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(50, 16); // Ajuste a posição do texto
  display.print(message);
  display.display();
}

void setup() {
  Serial.begin(9600);
  myADS.begin();
  myADS.setGain(GAIN_ONE);
  myADS.setDataRate(250);

  if (!SD.begin()) {
    Serial.println("Falha ao inicializar o cartão SD.");
    return;
  }
  
  // Inicializa o display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Erro na inicialização!"));
    while (1);
  }
  display.clearDisplay();
  display.display();

  // Configura os botões como entrada com pull-up interno
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN_BLUETOOTH, INPUT_PULLUP);

  // Exibe a mensagem inicial
  displayHeartWithText("INICIANDO", true);
}

void loop() {
  static bool buttonPressed = false;
  static bool buttonBluetoothPressed = false;

  // Verifica se o botão de gravação foi pressionado (pino 12)
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      if (recording) {
        stopRecording();
        displayHeartWithText("PARADO", true);
        recording = false;
      } else {
        startNewFile();
        displayHeartWithText("LENDO", true);
        recording = true;
      }
    }
  } else {
    buttonPressed = false;
  }

  // Verifica se o botão do Bluetooth foi pressionado (pino 14)
  if (digitalRead(BUTTON_PIN_BLUETOOTH) == LOW) {
    if (!buttonBluetoothPressed) {
      buttonBluetoothPressed = true;
      if (!bluetoothAtivo) {
        estadoAtual = BLUETOOTH;
        bluetoothAtivo = true;
        SerialBT.begin("ECG Bluetooth");
        displayBluetoothWithText("BLUETOOTH", true);
        stopRecording();  // Parar gravação ao iniciar Bluetooth
      } else {
        SerialBT.end();
        displayHeartWithText("INICIANDO", true);
        bluetoothAtivo = false;
        estadoAtual = INICIANDO;
        startNewFile();  // Reiniciar gravação após desligar Bluetooth
      }
    }
  } else {
    buttonBluetoothPressed = false;
  }

  // Se estiver gravando, leia o ECG e salve no arquivo
  if (recording) {
    currentTime = millis();
    if (currentTime - startTime >= interval) {
      stopRecording();
      startNewFile();  // Criar novo arquivo após 1 minuto
    }

    int ecgValue = myADS.readADC_SingleEnded(0);
    unsigned long elapsedTime = millis() - startTime;
    String dataLine = String(elapsedTime) + "," + String(ecgValue);
    myFile.println(dataLine);  // Grava os dados no arquivo
    Serial.println(dataLine);  // Exibe no Serial Monitor
  }

  // Função que controla o coração piscando
  if (estadoAtual == LENDO) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= heartInterval) {
      previousMillis = currentMillis;
      heartVisible = !heartVisible;
      displayHeartWithText("LENDO", heartVisible);
    }
  }

  // Função que controla o símbolo de Bluetooth piscando
  if (estadoAtual == BLUETOOTH) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= heartInterval) {
      previousMillis = currentMillis;
      heartVisible = !heartVisible;
      displayBluetoothWithText("BLUETOOTH", heartVisible);
    }
  }

  // Verifica se há conexão Bluetooth
  if (estadoAtual == BLUETOOTH && SerialBT.hasClient()) {
    displayBluetoothWithText("CONECTADO", true);
    if (SerialBT.available()) {
      valorRecebido = (char)SerialBT.read();
      Serial.println(valorRecebido);
      // Envie dados ou execute ações baseadas no valorRecebido
    }
  }
}