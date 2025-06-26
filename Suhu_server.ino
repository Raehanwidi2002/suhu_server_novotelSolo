#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <WiFi.h> 

#define DHTPIN1 18
#define DHTPIN2 19
#define DHTTYPE DHT22

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BUZZER_PIN 2
#define BUTTON_PIN 4 

const char* ssid = "IT-SV-RM-PUB";     
const char* password = "internet"; 

bool showLogoS1 = false;
bool showLogoS2 = false;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  Serial.begin(115200); 
  Serial.println("Memulai Sistem...");

  dht1.begin();
  dht2.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP); 

  // Menghubungkan ke WiFi
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  
  lcd.setCursor(0, 0);
  lcd.print("WiFi Terhubung");
  delay(2000);
  lcd.clear();
}

void loop() {
  
  if (digitalRead(BUTTON_PIN) == LOW) { 
    bunyikanBuzzer();
    Serial.println("Tombol ditekan: Buzzer Manual Aktif");
  }

  
  float suhu1 = dht1.readTemperature();
  float kelembapan1 = dht1.readHumidity();

  float suhu2 = dht2.readTemperature();
  float kelembapan2 = dht2.readHumidity();

  
  if (isnan(suhu1) || isnan(kelembapan1) || isnan(suhu2) || isnan(kelembapan2)) {
    Serial.println("Error membaca sensor DHT!");
    lcd.setCursor(0, 0);
    lcd.print("Error membaca");
    lcd.setCursor(0, 1);
    lcd.print("sensor DHT!");
    delay(2000);
    return;
  }

  
  lcd.setCursor(0, 0);
  lcd.print("S1:");
  lcd.print(suhu1, 1);
  lcd.print("C ");
  lcd.print(kelembapan1, 1);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("S2:");
  lcd.print(suhu2, 1);
  lcd.print("C ");
  lcd.print(kelembapan2, 1);
  lcd.print("%");

  
  Serial.println("Data Sensor:");
  Serial.print("S1 - Suhu: ");
  Serial.print(suhu1);
  Serial.print(" C, Kelembapan: ");
  Serial.print(kelembapan1);
  Serial.println(" %");
  Serial.print("S2 - Suhu: ");
  Serial.print(suhu2);
  Serial.print(" C, Kelembapan: ");
  Serial.print(kelembapan2);
  Serial.println(" %");

  
  if (suhu1 > 30) {
    lcd.setCursor(15, 0);
    showLogoS1 = !showLogoS1;
    if (showLogoS1) {
      lcd.print("*");
    } else {
      lcd.print(" ");
    }

    bunyikanBuzzer();
  } else {
    lcd.setCursor(15, 0);
    lcd.print(" ");
  }

  if (suhu2 > 30) {
    lcd.setCursor(15, 1);
    showLogoS2 = !showLogoS2;
    if (showLogoS2) {
      lcd.print("*");
    } else {
      lcd.print(" ");
    }

    bunyikanBuzzer();
  } else {
    lcd.setCursor(15, 1);
    lcd.print(" ");
  }

  delay(1000);
}

void bunyikanBuzzer() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}
