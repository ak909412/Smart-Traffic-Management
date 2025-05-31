#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// AP SSID (No Password for Open Network)
const char* apSSID = "ESP8266_AP";

// Web Server on ESP8266 (Port 80)
ESP8266WebServer server(80);

// Global Variable to Track Connection to Traffic Light
bool connectedToTrafficLight = false;

void setup() {
  Serial.begin(115200);
  
  // Start Open Access Point (No Password)
  WiFi.softAP(apSSID);
  Serial.println("📡 Open Access Point started");

  Serial.print("🌍 AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Reduce transmission power for short-range communication
  WiFi.setOutputPower(0); // Set to low power (2 dBm)

  // Set up Web Server Response for Connected Vehicles
  server.on("/", []() {
    server.send(200, "text/plain", "✅ You are connected to ESP8266_AP!");
  });

  server.begin();
  Serial.println("🌐 Web Server started");

  // Attempt to connect to Traffic Light
  connectToTrafficLight();
}

void connectToTrafficLight() {
  WiFi.disconnect();
  delay(100);
  Serial.println("🔍 Scanning for Traffic Light networks...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("❌ No networks found.");
    return;
  }

  // Try to connect to any "TrafficLight_X" SSID (X = 1,2,3,4)
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    if (ssid.startsWith("TrafficLight_")) {
      Serial.println("🚦 Connecting to: " + ssid);
      WiFi.begin(ssid.c_str(), "");  // Open network, no password

      int timeout = 10; // 10 seconds timeout
      while (WiFi.status() != WL_CONNECTED && timeout-- > 0) {
        delay(1000);
        Serial.print(".");
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Connected to " + ssid);
        connectedToTrafficLight = true;
        return;
      }
    }
  }

  Serial.println("❌ No TrafficLight network found.");
  connectedToTrafficLight = false;
}

void loop() {
  // Handle Web Server Requests
  server.handleClient();

  // Reconnect to Traffic Light if disconnected
  if (WiFi.status() != WL_CONNECTED && connectedToTrafficLight) {
    Serial.println("⚠️ Lost connection. Reconnecting...");
    connectToTrafficLight();
  }

  // Print Connected Vehicles (Clients)
  int numStations = WiFi.softAPgetStationNum();
  Serial.print("📡 Connected Vehicles: ");
  Serial.println(numStations);

  struct station_info* station = wifi_softap_get_station_info();
  while (station) {
    Serial.print("🚗 Vehicle MAC: ");
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", 
                  station->bssid[0], station->bssid[1], station->bssid[2],
                  station->bssid[3], station->bssid[4], station->bssid[5]);
    station = STAILQ_NEXT(station, next); // Move to the next station
  }

  delay(5000); // Check every 5 seconds
}
