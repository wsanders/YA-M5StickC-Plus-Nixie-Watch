#include <M5StickCPlus.h>
#include <WiFi.h>
#include "images/0.h"
#include "images/1.h"
#include "images/2.h"
#include "images/3.h"
#include "images/4.h"
#include "images/5.h"
#include "images/6.h"
#include "images/7.h"
#include "images/8.h"
#include "images/9.h"
int pixel[3];
// pointer fu to preserve the start of .h data
char *datastart[10];
uint16_t *bitmap[10];
int height = 60;
int width = 60;
char *data;
const char* ntpServer = "us.pool.ntp.org";
const long gmtOffset_sec     = 3600 * -8;
const int daylightOffset_sec = 0;

//OBFUSCATE!
const char* ssid     = "YOURSSID";
const char* password = "YOURPASS";
struct tm RTCtime;


void setup() { 
    M5.begin();
    M5.Lcd.setRotation(3);
    for (int i=0; i<10; i++) { 
        bitmap[i] = (uint16_t *)malloc(height * width * sizeof(uint16_t));
    }
    datastart[0] = data0;
    datastart[1] = data1;
    datastart[2] = data2;
    datastart[3] = data3;
    datastart[4] = data4;
    datastart[5] = data5;
    datastart[6] = data6;
    datastart[7] = data7;
    datastart[8] = data8;
    datastart[9] = data9;
    for (int i=0; i<10; i++) {
        data = datastart[i];
        for (int16_t y=0; y < height; y++) {
            for (int16_t x=0; x < width; x++) {
                HEADER_PIXEL(data, pixel);
                bitmap[i][60*y + x] = rgb888to565(pixel[0], pixel[1], pixel[2]);
            }
        }
    }
    setRTCTime();
}

 
void loop() {
        int d1,d2,d3,d4;

        getRTCTime(); 
        d1 = RTCtime.tm_hour / 10;        
        d2 = RTCtime.tm_hour % 10;
        d3 = RTCtime.tm_min / 10;        
        d4 = RTCtime.tm_min % 10;        
        M5.Lcd.drawBitmap(0 ,135/2-30,60,60,bitmap[d1]);
        M5.Lcd.drawBitmap(60 ,135/2-30,60,60,bitmap[d2]);
        M5.Lcd.drawBitmap(120 ,135/2-30,60,60,bitmap[d3]);
        M5.Lcd.drawBitmap(180 ,135/2-30,60,60,bitmap[d4]);
        delay(800);
}

uint16_t rgb888to565 (int r, int g, int b) {
    return (uint16_t) (
        ((r >> 3) & 0x1f) << 11 | 
        ((g >> 2) & 0x3f) << 5 |
         (b >> 3) & 0x1f );  
}


void setRTCTime() {
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    struct tm timeinfo;

    WiFi.mode(WIFI_STA);
    M5.Lcd.printf("\nConnecting to %s", ssid);
    WiFi.begin(ssid, password);  // Connect wifi and return connection status.
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);         // delay 0.5s.  延迟0.5s
        M5.Lcd.print(".");
    }
    M5.Lcd.println("\nCONNECTED!");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //  get the time
    // tm_sec,tm_min,tm_hr,tm_mday,tm_wday,tm_mon,tm_year,tm_yday,tm_isdst
    // Unfortunate name, we are really getting NTP time
    if (!getLocalTime(&timeinfo)) {
        M5.Lcd.println("Failed to obtain time");
        return;
    }
    RTC_TimeStruct.Hours = timeinfo.tm_hour;
    RTC_TimeStruct.Minutes = timeinfo.tm_min;
    RTC_TimeStruct.Seconds = timeinfo.tm_sec;
    RTC_DateStruct.WeekDay = timeinfo.tm_wday;
    RTC_DateStruct.Month = timeinfo.tm_mon;
    RTC_DateStruct.Date = timeinfo.tm_mday;
    RTC_DateStruct.Year = timeinfo.tm_year;
    M5.Rtc.SetTime(&RTC_TimeStruct);
    M5.Rtc.SetDate(&RTC_DateStruct);
    // I have no idea why this works:
    M5.Lcd.setCursor(0,0);
    M5.Lcd.println(&timeinfo, "RTC SET: %A, %B %d \n%Y %H:%M:%S");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void getRTCTime() {
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    // tm_sec,tm_min,tm_hr,tm_mday,tm_wday,tm_mon,tm_year,tm_yday,tm_isdst
    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetDate(&RTC_DateStruct);
    RTCtime.tm_hour = RTC_TimeStruct.Hours;
    RTCtime.tm_min = RTC_TimeStruct.Minutes;
    RTCtime.tm_sec = RTC_TimeStruct.Seconds;
    RTCtime.tm_wday = RTC_DateStruct.WeekDay;
    RTCtime.tm_mon = RTC_DateStruct.Month;
    RTCtime.tm_mday = RTC_DateStruct.Date;
    RTCtime.tm_year = RTC_DateStruct.Year;
    // I have no idea why this works:
    //M5.Lcd.setCursor(0,0);
    //M5.Lcd.println(&RTCtime, "RTC TIME IS : %A, %B %d \n%Y %H:%M:%S");
}

