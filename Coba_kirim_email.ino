

#include <ESP_Mail_Client.h>

// Objek SMTP Mail Client
ESP_Mail_Session session;
SMTP_Message message;

// Informasi WiFi
const char* ssid = "Redmi Note 11 Pro 5G";
const char* password = "qwertyui";

void setup() {
  // Inisialisasi Serial
  Serial.begin(115200);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected.");

  // Konfigurasi SMTP Session
  session.server.host_name = "smtp.gmail.com";
  session.server.port = 465;
  session.login.email = "rehanwidisugiarto2002@gmail.com";  // Ganti dengan email Anda
  session.login.password = "xkab qglz urzp irys";  // Ganti dengan password aplikasi Anda
  session.login.user_domain = "";

  // Konfigurasi Pesan
  message.sender.name = "Nama Pengirim";
  message.sender.email = "rehanwidisugiarto2002@gmail.com";
  message.subject = "Tes Email dari ESP32";
  message.addRecipient("EoS Mile Network", "eosmilenetwork@gmail.com");
  message.text.content = "Ini adalah email percobaan dari ESP32 menggunakan ESP Mail Client.";

  // Kirim Email
  SMTPSession smtp;
  if (!smtp.connect(&session)) {
    Serial.println("Koneksi ke server gagal!");
    return;
  }
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Gagal mengirim email: " + smtp.errorReason());
  } else {
    Serial.println("Email berhasil dikirim.");
  }

  // Bersihkan memori
  smtp.closeSession();
}

void loop() {
  // Tidak ada aksi di loop
}

