#include <Arduino.h>
#include "net.h"
#include <HCSR04.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define OLED_MOSI 23
#define OLED_CLK 18
#define OLED_DC 19
#define OLED_CS 5
#define OLED_RST 21


const char NBSP = '\xA0'; 


String ssid = "Pixel";
String password = "password";
const char *serverIP = "192.168.68.110";

String debugmsg = "";

void printWrapped(const String &str, int maxWidth);

const int DIST_AVG_SIZE = 10;
float distanceBuffer[DIST_AVG_SIZE] = {0};
int distanceIndex = 0;

float getSmoothedDistance(float newValue);
void printSmoothedDistance(float distance);

HCSR04 hc(4, 15);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(
	U8G2_R3, OLED_CS, OLED_DC, OLED_RST);

void setup()
{
	Serial.begin(115200);
	pinMode(OLED_RST, OUTPUT);
	digitalWrite(OLED_RST, LOW);
	delay(100);
	digitalWrite(OLED_RST, HIGH);
	delay(100);
	SPI.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);
	u8g2.begin();
	SetUpWiFi(ssid.c_str(), password.c_str());
	TestWiFiConnection();
	TestInternetConnection();
}

void loop()
{
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_6x10_mr);
	float distance = hc.dist();
	u8g2.setCursor(0, 10);
	float smoothedDistance = getSmoothedDistance(distance);
	printSmoothedDistance(smoothedDistance);

	static float lastSentDistance = -1000;
	static unsigned long lastSendTime = 0;
	const unsigned long sendInterval = 200;
	float distanceThreshold = 1.0;

	if (WiFi.status() != WL_CONNECTED)
	{
		debugmsg = "WiFi not connected, reconnecting...";
		SetUpWiFi(ssid.c_str(), password.c_str());
	}
	else
	{
		bool serverOk = PingServer(serverIP);
		if (serverOk)
		{
			debugmsg = "Server is reachable";
			if (smoothedDistance > 0 && (abs(smoothedDistance - lastSentDistance) > distanceThreshold) && (millis() - lastSendTime > sendInterval))
			{
				UpdateDistanceValue(smoothedDistance, serverIP);
				lastSentDistance = smoothedDistance;
				lastSendTime = millis();
			}
		}
		else
		{
			debugmsg = "Server is not reachable";
		}
	}
	float temp = temperatureRead();
	debugmsg += "\nTempppp:\t" + String(temp, 1) + "C ";

	u8g2.setCursor(0, 30);
	printWrapped(debugmsg, 64);
	u8g2.sendBuffer();
	delay(20);
}
float getSmoothedDistance(float newValue)
{

	if (newValue <= 0.5)
		return -1;
	distanceBuffer[distanceIndex] = newValue;
	distanceIndex = (distanceIndex + 1) % DIST_AVG_SIZE;
	float sum = 0;
	int count = 0;
	for (int i = 0; i < DIST_AVG_SIZE; i++)
	{
		if (distanceBuffer[i] > 0.5)
		{
			sum += distanceBuffer[i];
			count++;
		}
	}
	if (count == 0)
		return -1;
	return sum / count;
}
void printSmoothedDistance(float distance)
{
	static float lastValidDistance = 0;
	static int invalidCount = 0;
	const int INVALID_THRESHOLD = 5;
	if (distance >= 0)
	{
		lastValidDistance = distance;
		invalidCount = 0;
		u8g2.print(distance, 1);
		u8g2.print(" cm");
	}
	else
	{
		invalidCount++;
		if (invalidCount >= INVALID_THRESHOLD)
		{
			u8g2.print("Out of range");
		}
		else
		{
			u8g2.print(lastValidDistance, 1);
			u8g2.print(" cm");
		}
	}
}

void printWrapped(const String &str, int maxWidth)
{
	// \t is treaded a new word and will cause a line break if it doesn't fit, but won't be printed as a space
    int cursorX = u8g2.getCursorX();
    int cursorY = u8g2.getCursorY();
    String words = str;
    int wordStart = 0;
    for (size_t i = 0; i < words.length(); i++)
    {
        char c = words[i];
        if (c == ' ' || c == '\n' || c == '\t' || i == words.length() - 1)
        {
            String word = words.substring(wordStart, i);
            if (word.length() > 0)
            {
                if (cursorX + u8g2.getStrWidth(word.c_str()) > maxWidth)
                {
                    cursorX = 0;
                    cursorY += 10;
                    u8g2.setCursor(cursorX, cursorY);
                }
                u8g2.print(word);
                cursorX += u8g2.getStrWidth(word.c_str());
            }
            if (c == ' ')
            {
                if (cursorX + u8g2.getStrWidth(" ") > maxWidth)
                {
                    cursorX = 0;
                    cursorY += 10;
                    u8g2.setCursor(cursorX, cursorY);
                }
                u8g2.print(" ");
                cursorX += u8g2.getStrWidth(" ");
            }
			if(c == '\t'){
				 if (cursorX + u8g2.getStrWidth(" ") > maxWidth)
                {
                    cursorX = 0;
                    cursorY += 10;
                    u8g2.setCursor(cursorX, cursorY);
                }
                
                
            }
			
            if (c == '\n')
            {
                cursorX = 0;
                cursorY += 10;
                u8g2.setCursor(cursorX, cursorY);
            }
            wordStart = i + 1;
        }
    }
    if (wordStart < words.length())
    {
        String word = words.substring(wordStart);
        if (cursorX + u8g2.getStrWidth(word.c_str()) > maxWidth)
        {
            cursorX = 0;
            cursorY += 10;
            u8g2.setCursor(cursorX, cursorY);
        }
        u8g2.print(word);
    }
}