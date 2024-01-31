#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include "time.h"

const char* ssid       = "peek-a-bon"; // SSID
const char* password   = "mazda8ebk751"; // Password
const char* ntpServer =  "0.jp.pool.ntp.org";

const char* saddr = "192.168.1.102"; //taeget PC address

const int sport = 55998;
const int kport = 5556;

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

uint8_t mac3[6];

WiFiUDP Udp;
char packetBuffer[255];


void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);

  M5.IMU.Init();

  esp_read_mac(mac3, ESP_MAC_WIFI_STA);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(1, 0, 2);
  M5.Lcd.println("Welcome \n If the connection fails, turn off and on the power again.");

  // connect to WiFi
  M5.Lcd.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");

  }
  M5.Lcd.println("CONNECTED");
  delay(1000);

  // Set ntp time to local
  configTime(9 * 3600, 0, ntpServer);
  // Get local time
  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    // Set RTC time
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Hours   = timeInfo.tm_hour;
    TimeStruct.Minutes = timeInfo.tm_min;
    TimeStruct.Seconds = timeInfo.tm_sec;
    M5.Rtc.SetTime(&TimeStruct);

    RTC_DateTypeDef DateStruct;
    DateStruct.WeekDay = timeInfo.tm_wday;
    DateStruct.Month = timeInfo.tm_mon + 1;
    DateStruct.Date = timeInfo.tm_mday;
    DateStruct.Year = timeInfo.tm_year + 1900;
    M5.Rtc.SetDate(&DateStruct);
  }
  M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 1, 2);
    M5.Lcd.print("Connected \nto ");
    M5.Lcd.println(WiFi.localIP());
   uint8_t mac3[6];
   esp_read_mac(mac3, ESP_MAC_WIFI_STA);
  M5.Lcd.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac3[0], mac3[1], mac3[2], mac3[3], mac3[4], mac3[5]);
  Udp.begin(kport); 

}

void loop() {
  int mil = millis(); 
  // put your main code here, to run repeatedly:
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetDate(&RTC_DateStruct);
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.printf("%04d-%02d-%02d ", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
  M5.Lcd.printf("%02d:%02d:%02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);


  // 加速度データ取得
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.Imu.getGyroData(&gyroX,&gyroY,&gyroZ);

  //UDP
  Udp.beginPacket(saddr, sport);
  Udp.printf("%3.3f,%3.3f,%3.3f,%3.3f,%3.3f,%3.3f", accX, accY, accZ, gyroX, gyroY, gyroZ);
  // Udp.printf("%3.3f,%3.3f,%3.3f", gyroX, gyroY, gyroZ);

  Udp.endPacket();  
  delay(10);
}
