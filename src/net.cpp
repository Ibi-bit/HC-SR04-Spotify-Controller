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
