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
#include <ThreeWire.h>
#include <RtcDS1302.h>

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
const int IO = 27;    // DAT
const int SCLK = 25;  // CLK
const int CE = 26;    // RST

ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);

String getDateTimeString(const RtcDateTime& dt) {
  char datestring[20];
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  return String(datestring);
}

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

  // Obtém data e hora como string
  RtcDateTime now = Rtc.GetDateTime();
  String dateTimeString = getDateTimeString(now);
  myFile.println("INICIO:");
  myFile.print(dateTimeString);
  myFile.println();
  myFile.println("Leitura(ECG)");  // Cabeçalho do arquivo
  startTime = millis();
}

// Função para parar de gravar e fechar o arquivo atual
void stopRecording() {
  if (myFile) {

    // Obtém data e hora como string
    RtcDateTime now = Rtc.GetDateTime();
    String dateTimeString = getDateTimeString(now);
    myFile.println();
    myFile.println("FIM:");
    myFile.print(dateTimeString);
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

// Função para enviar arquivos .txt do SD via Bluetooth
void enviarArquivosSD() {
  File root = SD.open("/");
  File file = root.openNextFile();

  while (file) {
    if (!file.isDirectory()) {
      Serial.print("Enviando arquivo: ");
      Serial.println(file.name());

      // Enviar nome do arquivo
      SerialBT.println(file.name());

      // Enviar conteúdo do arquivo em blocos
      while (file.available()) {
        String data = file.readStringUntil('\n');
        SerialBT.println(data);
        delay(10);  // Pequeno atraso para evitar buffer overflow
      }
      SerialBT.println("FIM_ARQUIVO");  // Marcação de fim do arquivo
    }
    file.close();
    file = root.openNextFile();
  }

  SerialBT.println("FIM_TRANSFERENCIA");  // Indicar que todos os arquivos foram enviados
}

void setup() {
  Serial.begin(9600);
  myADS.begin();
  myADS.setGain(GAIN_ONE);
  myADS.setDataRate(500);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid()) {
    Rtc.SetDateTime(compiled);  // Configura RTC com data/hora da compilação
  }

  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);  // Garante que o RTC está rodando
  }

  if (!SD.begin()) {
    Serial.println("Falha ao inicializar o cartão SD.");
    return;
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Erro na inicialização!"));
    while (1);
  }
  display.clearDisplay();
  display.display();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN_BLUETOOTH, INPUT_PULLUP);

  displayHeartWithText("PRONTO", true);
}

void loop() {
  static bool buttonPressed = false;
  static bool buttonBluetoothPressed = false;

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      if (recording) {
        stopRecording();
        displayHeartWithText("PARADO", true);
        recording = false;
        estadoAtual = PARADO;
        delay(3000);
      } else {
        startNewFile();
        displayHeartWithText("LENDO", true);
        recording = true;
        estadoAtual = LENDO;
      }
    }
  } else {
    buttonPressed = false;
  }

  if (digitalRead(BUTTON_PIN_BLUETOOTH) == LOW) {
    if (!buttonBluetoothPressed) {
      buttonBluetoothPressed = true;
      if (!bluetoothAtivo) {
        estadoAtual = BLUETOOTH;
        bluetoothAtivo = true;
        SerialBT.begin("ECG Bluetooth");
        displayBluetoothWithText("BLUETOOTH", true);
        recording = false;
        stopRecording();
      } else {
        SerialBT.end();
        displayHeartWithText("PRONTO", true);
        bluetoothAtivo = false;
        recording = false;
        estadoAtual = PARADO;
        delay(3000);
      }
    }
  } else {
    buttonBluetoothPressed = false;
  }

  if (recording) {
    currentTime = millis();
    if (currentTime - startTime >= interval) {
      stopRecording();
      startNewFile();
    }

    int ecgValue = myADS.readADC_SingleEnded(0);
    String dataLine = String(ecgValue);
    myFile.println(dataLine);
    Serial.println(dataLine);
  }

  if (estadoAtual == LENDO) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= heartInterval) {
      previousMillis = currentMillis;
      heartVisible = !heartVisible;
      displayHeartWithText("LENDO", heartVisible);
    }
  } else if (estadoAtual == PARADO || estadoAtual == INICIANDO) {
    displayHeartWithText("PRONTO", true);
  }

  if (estadoAtual == BLUETOOTH) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= heartInterval) {
      previousMillis = currentMillis;
      heartVisible = !heartVisible;
      displayBluetoothWithText("BLUETOOTH", heartVisible);
    }
  }

  if (estadoAtual == BLUETOOTH && SerialBT.hasClient()) {
    displayBluetoothWithText("CONECTADO", true);
    if (SerialBT.available()) {
      valorRecebido = (char)SerialBT.read();
      Serial.println(valorRecebido);
      if (valorRecebido == '1') {
        enviarArquivosSD();  // Envia os arquivos via Bluetooth
      }
    }
  }
}
