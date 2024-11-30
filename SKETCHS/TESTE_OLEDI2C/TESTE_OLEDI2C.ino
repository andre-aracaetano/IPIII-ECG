#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <SPI.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // Largura do display
#define SCREEN_HEIGHT 32 // Altura do display

#define SCREEN_ADDRESS 0x3C // Endereço do display (tente trocar por 0x3D se não funcionar)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

// Função para desenhar o coração e o texto "LENDO"
void displayHeartWithText(bool heartVisible) {
  display.clearDisplay();

  // Exibe o bitmap do coração se estiver visível
  if (heartVisible) {
    display.drawBitmap(0, 0, heart_bitmap, 32, 32, 1);
    
    // Desenha um raiozinho no meio do coração (linha diagonal)
    display.drawLine(10, 10, 20, 20, 1);  // Exemplo de raio
    display.drawLine(20, 10, 10, 20, 1);  // Outra parte do raio
  }

  // Exibe o texto "LENDO" ao lado direito
  display.setTextSize(2); // Tamanho maior do texto
  display.setTextColor(1);
  display.setCursor(40, 8); // Posição ajustada para centralizar o texto
  display.print("LENDO");

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
}

void loop() {
  // Função que controla o coração piscando
  static unsigned long previousMillis = 0;
  const long interval = 500; // Intervalo para o piscamento (500ms)
  unsigned long currentMillis = millis();
  static bool heartVisible = true;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Alterna o estado de visibilidade do coração
    heartVisible = !heartVisible;
    
    displayHeartWithText(heartVisible);
  }
}