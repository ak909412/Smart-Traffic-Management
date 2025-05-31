#include <ESP8266WiFi.h>

const char* ambulanceSSID = "ESP8266_AP";  // Ambulance WiFi SSID
#define LED_PIN D5  // LED connected to GPIO D5

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Ensure LED starts OFF

  connectToNetwork();
}

void connectToNetwork() {
  WiFi.disconnect();
  delay(100);
  Serial.println("🔍 Scanning for available networks...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("❌ No networks found.");
    digitalWrite(LED_PIN, LOW); // Ensure LED stays OFF
    return;
  }

  // **Try connecting to the Ambulance First**
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    if (ssid == ambulanceSSID) {
      Serial.println("🚑 Connecting to Ambulance: " + ssid);
      WiFi.begin(ssid, "");  // Open network, no password

      if (waitForConnection()) {
        Serial.println("✅ Connected to Ambulance Network");
        blinkLED();  // Turn ON LED
        return;
      }
    }
  }

  // **If no Ambulance is found, try connecting to a Traffic Light**
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    if (ssid.startsWith("TrafficLight_")) {
      Serial.println("🚦 Connecting to Traffic Light: " + ssid);
      WiFi.begin(ssid.c_str(), "");  // Open network, no password

      if (waitForConnection()) {
        Serial.println("✅ Connected to Traffic Light: " + ssid);
        digitalWrite(LED_PIN, LOW);  // Ensure LED stays OFF
        return;
      }
    }
  }

  // **No Ambulance or Traffic Light found → Ensure LED stays OFF**
  Serial.println("❌ No Ambulance or Traffic Light found.");
  digitalWrite(LED_PIN, LOW);
}

bool waitForConnection() {
  int timeout = 10; // 10 seconds timeout
  while (WiFi.status() != WL_CONNECTED && timeout-- > 0) {
    delay(1000);
    Serial.print(".");
  }
  return WiFi.status() == WL_CONNECTED;
}

void blinkLED() {
  while (WiFi.status() == WL_CONNECTED && WiFi.SSID() == ambulanceSSID) {
    digitalWrite(LED_PIN, HIGH);
    delay(3000); // LED ON for 3 seconds
    digitalWrite(LED_PIN, LOW);
    delay(6000); // LED OFF for 6 seconds
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Lost connection. Reconnecting...");
    digitalWrite(LED_PIN, LOW);  // Ensure LED is OFF when disconnected
    connectToNetwork();
  }
  delay(5000);  // Check connection every 5 seconds
}
