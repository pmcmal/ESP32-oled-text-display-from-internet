#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <driver/adc.h>

#define SDA_PIN 5
#define SCL_PIN 4
#define SSD1306_I2C_ADDRESS 0x3C // adres i2c wyświetlacza oled
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Ustawienia sieci WiFi
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Konfiguracja wyświetlacza OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Adres URL do pliku TXT na Google Drive
const char* fileUrl = "https://raw.githubusercontent.com/pmcmal/ESP32-oled-text-display-from-internet/main/esp32.txt";
// serial baundrate 115200!

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  // Inicjalizacja WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Połączono z WiFi...");
  }

  // Inicjalizacja wyświetlacza OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    Serial.println(F("Nie można znaleźć wyświetlacza OLED"));
    delay(10*1000);
    ESP.restart();
    while (true);
  }

  // Wyczyszczenie ekranu
  display.clearDisplay();
  display.display();
}

void loop() {
  // Pobranie tekstu z pliku TXT na Google Drive
  String text = fetchTextFromGoogleDrive(fileUrl);
  
  Serial.println("Zawartość pobranego tekstu:");
  Serial.println(text);

  // Wyświetlanie kolejnych linii tekstu
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int lineNumber = 1;
  int yPos = 0;

  while (yPos < SCREEN_HEIGHT) {
    int dotPos = text.indexOf(String(lineNumber) + ".");
    if (dotPos != -1) {
      int nextDotPos = text.indexOf(String(lineNumber + 1) + ".", dotPos);
      if (nextDotPos == -1) {
        nextDotPos = text.length();
      }
      String line = text.substring(dotPos, nextDotPos);
      display.setCursor(0, yPos);
      display.println(line);
      yPos += 10;  // Przesunięcie o 10 pikseli na następną linię
      lineNumber++;
    } else {
      break;
    }
  }

  display.display();
  delay(5000);  // Opóźnienie przed wyświetleniem następnego zestawu linii
}

String fetchTextFromGoogleDrive(const char* url) {
  HTTPClient http;

  Serial.print("Pobieranie danych...");
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Gotowe.");
    http.end();
    return payload;
  } else {
    Serial.println("Błąd pobierania danych.");
    http.end();
    return "";
  }
}