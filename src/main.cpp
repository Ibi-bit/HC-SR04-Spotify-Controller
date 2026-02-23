#include <Arduino.h>
#include "net.h"
#include <HCSR04.h>
#include <SPI.h>
#include <U8g2lib.h>

#define OLED_MOSI 23
#define OLED_CLK 18
#define OLED_DC 19
#define OLED_CS 5
#define OLED_RST 21

String ssid = "Cutters";
String password = "Chickens";

String debugmsg = "";



void printWrapped(const String &str, int maxWidth);


const int DIST_AVG_SIZE = 10;
float distanceBuffer[DIST_AVG_SIZE] = {0};
int distanceIndex = 0;

float getSmoothedDistance(float newValue);


HCSR04 hc(4, 15);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(
	U8G2_R3, OLED_CS, OLED_DC, OLED_RST);


void setup() {
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

void loop() {
	u8g2.clearBuffer();

	u8g2.setFont(u8g2_font_6x10_mr);
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

void printWrapped(const String &str, int maxWidth)
{
	
	int cursorX = 	u8g2.getCursorX();
;
	int cursorY = u8g2.getCursorY();
	for (size_t i = 0; i < str.length(); i++)
	{
		char c = str[i];
		u8g2.print(c);
		cursorX += u8g2.getStrWidth(String(c).c_str());
		if (cursorX >= maxWidth)
		{
			cursorX = 0;
			cursorY += 10;
			u8g2.setCursor(cursorX, cursorY);
		}
	}
}