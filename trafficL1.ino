#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Traffic Light LED Pins
#define RED 14    // D5 
#define YELLOW 4  // D2
#define GREEN 5   // D1

// WiFi Credentials
const char* apSSID = "TrafficLight_2";  // Unique SSID for each traffic light
const char* staSSID = "phone";          // Local WiFi SSID
const char* staPassword = "0987654321"; // Local WiFi Password

// Server Details
const char* serverIP = "192.168.223.247"; // Python server IP
const int serverPort = 5000;
const char* trafficLightID = "TrafficLight_2";  // Unique ID for this light

// Global Variable for Response Status
String status = "";  // Stores last received server response

void setup() {
  Serial.begin(115200);

  // Initialize LED pins
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);

  // Start with all lights OFF
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);

  // Set ESP to AP + STA Mode
  WiFi.mode(WIFI_AP_STA);

  // **Limit Hotspot Range by Using a Lower Rate**
  wifi_set_user_fixed_rate(0, 0);  // Uses lower transmission speed to reduce range

  // Start SoftAP (Hotspot for Vehicles)
  WiFi.softAP(apSSID, "", 1, false, 4); // Max 4 connections, lowest channel 1

  // Connect to Local WiFi (for sending data & receiving commands)
  WiFi.begin(staSSID, staPassword);
  Serial.print("Connecting to Local WiFi...");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Try 20 times
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to Local WiFi.");
  } else {
    Serial.println("\nFailed to connect to WiFi!");
  }
}

void sendMACsToServer() {
  int numDevices = WiFi.softAPgetStationNum();
  struct station_info* station = wifi_softap_get_station_info();
  
  String macList = "";
  while (station) {
    macList += String(station->bssid[0], HEX) + ":" +
               String(station->bssid[1], HEX) + ":" +
               String(station->bssid[2], HEX) + ":" +
               String(station->bssid[3], HEX) + ":" +
               String(station->bssid[4], HEX) + ":" +
               String(station->bssid[5], HEX) + ",";
    station = STAILQ_NEXT(station, next);
  }

  // Send MAC addresses to Python server
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    String url = "http://" + String(serverIP) + ":" + String(serverPort) + "/update";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"light_id\":\"" + String(apSSID) + "\", \"macs\":\"" + macList + "\"}";
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
      status = http.getString(); // Store response in global variable
      Serial.println("Data Sent. Server Response: " + status);
    if (status.indexOf("green") != -1) {  // Check if "green" is in status
      Serial.println("Go");
      digitalWrite(GREEN, HIGH);
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, LOW);
    } else if (status.indexOf("red") != -1) {  // Check if "red" is in status
      Serial.println("Stop");
      digitalWrite(GREEN, LOW);
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
    } else {
      Serial.println("Something else: " + status);
    }

    } else {
      Serial.println("Error Sending Data");
    }
    http.end();
  }
}

void getTrafficSignal() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://" + String(serverIP) + ":" + String(serverPort) + "/signal";
    http.begin(client, url);

    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      status = http.getString(); // Store response globally
      //Serial.println("Received Signal: " + status);

      DynamicJsonDocument doc(256);
      deserializeJson(doc, status);
      
      if (doc.containsKey(trafficLightID)) {
        String signal = doc[trafficLightID];  // Get the signal for this traffic light
        controlTrafficLight(signal);
      }
    } else {
      Serial.print("Error Receiving Data. HTTP Response: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("ESP not connected to WiFi!");
  }
}

void controlTrafficLight(String signal) {
  Serial.println("Received Signal: " + status);
  if (signal == "green") {
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, LOW);
  } else { // Default to RED if command is unknown or "red"
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(YELLOW, LOW);
  }
}

void loop() {
  sendMACsToServer();  // Send vehicle count
  getTrafficSignal();   // Check for new traffic signal from the server
  delay(5000);         // Update every 5 seconds
}
