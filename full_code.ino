#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define DHTTYPE DHT22
#define BUZZER_PIN 2
#define BUTTON_PIN 4

// Pin DHT
const int dhtPins[] = {18, 19};
DHT dht[] = {DHT(dhtPins[0], DHTTYPE), DHT(dhtPins[1], DHTTYPE)};

// Lokasi Sensor
const char* lokasiSensor[] = {"Ruang Server", "Ruang Pabx"};

// Variabel untuk tampilan dan pengiriman email
bool showLogo[2] = {false, false};
unsigned long lastEmailSent[2] = {0, 0};

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Koneksi WiFi
const char* ssid = "IT-SV-RM-PUB";
const char* password = "internet";

// Email
ESP_Mail_Session session;
SMTP_Message message;

const char* senderEmail = "suhuserverronobis@gmail.com";
const char* senderPassword = "oyoj hveu gvsa giyq";

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(BUZZER_PIN, LOW);

  for (DHT& sensor : dht) {
    sensor.begin();
  }

  connectWiFi();
  setupEmail();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    bunyikanBuzzer();
    Serial.println("Tombol ditekan - Sensor Air");
  }

  for (int i = 0; i < 2; i++) {
    prosesSensor(i);
  }

  delay(1000);
}

void connectWiFi() {
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

void setupEmail() {
  session.server.host_name = "smtp.gmail.com";
  session.server.port = 465;
  session.login.email = senderEmail;
  session.login.password = senderPassword;
  session.login.user_domain = "";

  message.sender.name = "Server Alert";
  message.sender.email = senderEmail;
  message.subject = "Peringatan Suhu Tinggi!";

  message.addRecipient("ITM", "mile@novotelsolo.com");
  message.addRecipient("Support", "rayhanwidisugiarto2002@gmail.com");
  message.addRecipient("Admin", "eosmilenetwork@gmail.com");
}

void prosesSensor(int index) {
  float suhu = dht[index].readTemperature();
  float kelembapan = dht[index].readHumidity();

  lcd.setCursor(0, index);

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.printf("Error membaca sensor DHT pada %s\n", lokasiSensor[index]);
    lcd.printf("S%d:------     ", index + 1); // strip jika error
    return;
  }

  lcd.printf("S%d:%.1fC %.1f%%", index + 1, suhu, kelembapan);
  Serial.printf("S%d (%s) - Suhu: %.1f C, Kelembapan: %.1f %%\n", index + 1, lokasiSensor[index], suhu, kelembapan);

  if (suhu > 30) {
    lcd.setCursor(15, index);
    showLogo[index] = !showLogo[index];
    lcd.print(showLogo[index] ? "*" : " ");
    bunyikanBuzzer();

    if (millis() - lastEmailSent[index] > 60000) {
      kirimEmail(String("Suhu dan kelembapan di ") + lokasiSensor[index] +
                 " melebihi batas:\n" +
                 "Suhu: " + suhu + " °C\n" +
                 "Kelembapan: " + kelembapan + " %");
      lastEmailSent[index] = millis();
    }
  } else {
    lcd.setCursor(15, index);
    lcd.print(" ");
  }
}

void bunyikanBuzzer() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void kirimEmail(String isiPesan) {
  SMTPSession smtp;

  String htmlContent =
    "<html>"
    "<head>"
    "<style>"
    "  body { font-family: Arial, sans-serif; margin: 20px; }"
    "  h1 { color: #FF5722; }"
    "  p { font-size: 14px; color: #333; }"
    "  .highlight { color: #FF5722; font-weight: bold; }"
    "  .footer { font-size: 12px; color: #777; margin-top: 20px; }"
    "</style>"
    "</head>"
    "<body>"
    "  <h1>⚠️ Peringatan Suhu Tinggi!</h1>"
    "  <p>Sistem mendeteksi <span class='highlight'>Suhu Tinggi</span> pada salah satu lokasi <span class='highlight'>Server Novotel</span>:</p>"
    "  <p><b>Detail:</b></p>"
    "  <h2><i>" + isiPesan + "</i></h2>"
    "  <p>Mohon segera melakukan tindakan untuk mengatasi masalah ini.</p>"
    "  <hr>"
    "  <div class='footer'>"
    "    Email ini dikirim secara otomatis oleh sistem IoT Server."
    "  </div>"
    "</body>"
    "</html>";

  message.html.content = htmlContent.c_str();
  message.html.charSet = "utf-8";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session)) {
    Serial.println("Koneksi ke server gagal!");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Gagal mengirim email: " + smtp.errorReason());
  } else {
    Serial.println("Email berhasil dikirim.");
  }

  smtp.closeSession();
}
