#include <WiFi.h>
#include <HTTPClient.h>

void SetUpWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void TestWiFiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi is connected");
  } else {
    Serial.println("WiFi is not connected");
  }
}

void TestInternetConnection() {
  HTTPClient http;
  http.begin("http://www.google.com");
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
bool PingServer(const char* serverIP) {
    HTTPClient http;
    http.setTimeout(2000);
    String url = String("http://") + serverIP + ":6000/ping";
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        Serial.print("Ping Response code: ");
        Serial.println(httpResponseCode);
    } else {
        Serial.print("Error on Ping request: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    return httpResponseCode > 0;
}
void UpdateDistanceValue(float distance, const char* serverIP) {
    if (WiFi.status() != WL_CONNECTED) return;
    HTTPClient http;
    http.setTimeout(2000);
    String url = String("http://") + serverIP + ":6000/distance";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    String jsonPayload = "{\"distance\": " + String(distance) + "}";
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.print("Update Distance Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error on Update Distance request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
}
char* GetSongPlaying(const char* serverIP) {
    if (WiFi.status() != WL_CONNECTED) return nullptr;
    HTTPClient http;
    http.setTimeout(2000);
    String url = String("http://") + serverIP + ":6000/get_current_song";
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        String response = http.getString();
        char* songName = new char[response.length() + 1];
        strcpy(songName, response.c_str());
        return songName;
    } else {
        Serial.print("Error on Get Song request: ");
        Serial.println(httpResponseCode);
        return nullptr;
    }
}

