#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< Address for 128x32 display

#define BUTTON_PIN 14 // Define o pino onde o botão está conectado

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* messages[] = {"LIGADO", "LENDO", "ENVIANDO"};
int currentMessage = 0; // Índice da mensagem atual

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Configura o pino do botão como entrada com pull-up interno

  // Initialize display with I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Set text color to white

  showMessage();
}

void loop() {
  // Verifica se o botão foi pressionado
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Debounce para evitar leituras erradas
    if (digitalRead(BUTTON_PIN) == LOW) {
      currentMessage = (currentMessage + 1) % 3; // Passa para a próxima mensagem
      showMessage();
      while (digitalRead(BUTTON_PIN) == LOW); // Espera até o botão ser solto
    }
  }
}

void showMessage() {
  display.clearDisplay();
  display.setTextSize(2); // Tamanho da fonte para caber no display
  int16_t x, y;
  uint16_t width, height;
  display.getTextBounds(messages[currentMessage], 0, 0, &x, &y, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2); // Centraliza o texto
  display.print(messages[currentMessage]);
  display.display();
}
