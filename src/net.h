

void SetUpWiFi(const char* ssid, const char* password);
void TestWiFiConnection();
void TestInternetConnection();
bool PingServer(const char* serverIP);
void UpdateDistanceValue(float distance, const char* serverIP);