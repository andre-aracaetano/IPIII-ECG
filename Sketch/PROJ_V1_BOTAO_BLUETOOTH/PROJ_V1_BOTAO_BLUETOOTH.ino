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
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

char valorRecebido;

// Definição dos botões
#define BUTTON_PIN 12
#define BUTTON_PIN_BLUETOOTH 14
#define SCREEN_ADDRESS 0x3C // Endereço do display (tente trocar por 0x3D se não funcionar)
#define SCREEN_WIDTH 128 // Largura do display
#define SCREEN_HEIGHT 32 // Altura do display

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Estados do sistema
enum Estado { LENDO, PARADO, INICIANDO, BLUETOOTH, CONECTADO };
Estado estadoAtual = LENDO;

bool heartVisible = true;  // Controle da visibilidade do coração
unsigned long previousMillis = 0;
const long interval = 500; // Intervalo para o piscamento (500ms)

// Bitmap do coração com um raio dentro (32x32 pixels)
const unsigned char heart_bitmap[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x07, 0xF8, 0x3F, 0xE0, 0x0F, 0xFE, 0x7F, 0xF0,
  0x1F, 0xFE, 0x7F, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x3F, 0xFF, 0xEF, 0xF8, 0x3F, 0xFF, 0xCF, 0xFC,
  0x3F, 0xFF, 0xC7, 0xFC, 0x3F, 0xFF, 0x87, 0xFC, 0x3F, 0xFF, 0x83, 0xFC, 0x3F, 0xFF, 0x93, 0xFC,
  0x1F, 0xE7, 0x31, 0xF8, 0x1F, 0xE7, 0x39, 0xF8, 0x10, 0x02, 0x38, 0x00, 0x08, 0x1A, 0x7C, 0x00,
  0x07, 0xF8, 0x7F, 0xE0, 0x03, 0xF8, 0xFF, 0xC0, 0x01, 0xFC, 0xFF, 0x80, 0x00, 0xFD, 0xFF, 0x00,
  0x00, 0x7F, 0xFC, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Função para desenhar o coração e o texto
void displayHeartWithText(const char* message, bool heartVisible) {
  display.clearDisplay();

  // Exibe o bitmap do coração se estiver visível
  if (heartVisible) {
    display.drawBitmap(0, 0, heart_bitmap, 32, 32, 1);
    
    // Desenha um raiozinho no meio do coração (linha diagonal)
    display.drawLine(10, 10, 20, 20, 1);  // Exemplo de raio
    display.drawLine(20, 10, 10, 20, 1);  // Outra parte do raio
  }

  // Exibe o texto ao lado direito
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(50, 16);
  display.print(message);

  // Atualiza o display
  display.display();
}

// Função para desenhar o símbolo de Bluetooth
void displayBluetoothWithText(const char* message, bool bluetoothVisible) {
  display.clearDisplay();

  // Exibe o círculo representando o símbolo do Bluetooth se estiver visível
  if (bluetoothVisible) {
    display.fillCircle(16, 16, 10, WHITE); // Desenha um círculo com raio de 10 no lugar do coração
  }

  // Exibe o texto ao lado direito
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(50, 16);
  display.print(message);

  // Atualiza o display
  display.display();
}

void setup() {
  Serial.begin(9600);

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
}

void loop() {
  static bool buttonPressed = false;
  static bool buttonBluetoothPressed = false;
  static unsigned long inicioMillis = 0;
  static bool bluetoothAtivo = false;

  // Lê o estado do botão do pino 12 (LOW significa pressionado)
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;  // Marca que o botão foi pressionado
      if (estadoAtual == LENDO) {
        estadoAtual = PARADO;
        displayHeartWithText("PARADO", true);
      } else if (estadoAtual == PARADO) {
        estadoAtual = INICIANDO;
        displayHeartWithText("INICIANDO", true);
        inicioMillis = millis();  // Marca o tempo de início
      }
    }
  } else {
    buttonPressed = false;  // Libera o botão quando não estiver pressionado
  }

  // Lê o estado do botão do pino 14 (LOW significa pressionado)
  if (digitalRead(BUTTON_PIN_BLUETOOTH) == LOW) {
    if (!buttonBluetoothPressed) {
      buttonBluetoothPressed = true;  // Marca que o botão foi pressionado
      if (!bluetoothAtivo) {
        estadoAtual = BLUETOOTH;
        bluetoothAtivo = true;
        SerialBT.begin("ECG Bluetooth"); // Inicia o Bluetooth apenas quando necessário
        displayBluetoothWithText("BLUETOOTH", true);
      } else {
        estadoAtual = INICIANDO;
        bluetoothAtivo = false;
        SerialBT.end(); // Desativa o Bluetooth
        displayHeartWithText("INICIANDO", true);
      }
    }
  } else {
    buttonBluetoothPressed = false;  // Libera o botão quando não estiver pressionado
  }

  // Verifica se está no estado INICIANDO e se passaram 3 segundos
  if (estadoAtual == INICIANDO && (millis() - inicioMillis >= 3000)) {
    estadoAtual = LENDO;
  }

  // Controle do Bluetooth piscando ou conectado
  if (estadoAtual == BLUETOOTH) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      heartVisible = !heartVisible;
      displayBluetoothWithText("BLUETOOTH", heartVisible);
    }

    // Verifica se há conexão Bluetooth
    if (SerialBT.hasClient()) {
      estadoAtual = CONECTADO;
      displayBluetoothWithText("CONECTADO", true);  // Mostra o estado conectado e para de piscar
    }
  }

  // Função que controla o coração piscando no estado "LENDO"
  if (estadoAtual == LENDO) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      heartVisible = !heartVisible;
      displayHeartWithText("LENDO", heartVisible);
    }
  }

  // Checa se o Bluetooth está recebendo dados
  if (estadoAtual == CONECTADO && SerialBT.available()) {
    valorRecebido = (char)SerialBT.read();
    
    // Se receber o comando '1', imprime "RECEBIDO" no monitor serial
    if (valorRecebido == '1') {
      Serial.println("RECEBIDO");
    }
  }
}

