#define BLYNK_TEMPLATE_ID "TMPL67LOFE3Ce"
#define BLYNK_TEMPLATE_NAME "TUgas UAS"
#define BLYNK_AUTH_TOKEN "2ynQiU89xNnitl0KZFvN0VC_He8B6d8X"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// ---------- WiFi ----------
char ssid[] = ":v";
char pass[] = "sseloselooo";

// ---------- DHT ----------
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---------- KIPAS ----------
#define FAN_PIN 4
float suhuNyala = 29.0;
float suhuMati = 28.0;
bool kipasNyala = false;
bool modeManual = false;
bool kipasManualState = false;

// ---------- Virtual Pins ----------
#define VPIN_SUHU V0
#define VPIN_KELEMBABAN V1
#define VPIN_KIPAS_MANUAL V2

// ---------- Blynk Write (Kipas Manual) ----------
BLYNK_WRITE(VPIN_KIPAS_MANUAL) {
  int val = param.asInt(); // 0 atau 1 dari switch
  modeManual = true;
  kipasManualState = val;
  digitalWrite(FAN_PIN, val ? HIGH : LOW);
  kipasNyala = val;
  Serial.println("🔧 Mode manual: kipas dikontrol dari HP");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("📡 ESP32 Terhubung ke Blynk");
}

void loop() {
  Blynk.run();

  float suhu = dht.readTemperature();
  float kelembaban = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembaban)) {
    Serial.println("⚠️ Sensor error");
    delay(3000);
    return;
  }

  Serial.printf("🌡 Suhu: %.2f°C | 💧 Kelembaban: %.1f%% | Kipas: %s\n",
                suhu, kelembaban, kipasNyala ? "NYALA" : "MATI");

  Blynk.virtualWrite(VPIN_SUHU, suhu);
  Blynk.virtualWrite(VPIN_KELEMBABAN, kelembaban);

  // Otomatis kontrol kipas
  if (!modeManual) {
    if (suhu > suhuNyala && !kipasNyala) {
      digitalWrite(FAN_PIN, HIGH);
      kipasNyala = true;
      Serial.println("🚨 Suhu tinggi, kipas dinyalakan otomatis");
    } else if (suhu < suhuMati && kipasNyala) {
      digitalWrite(FAN_PIN, LOW);
      kipasNyala = false;
      Serial.println("✅ Suhu normal, kipas dimatikan otomatis");
    }
  }

  delay(3000);
}
