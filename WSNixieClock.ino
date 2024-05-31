#include <M5StickCPlus.h>
#include <Preferences.h>
#include <WiFi.h>
#include "images/header_pixel.h"
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
long gmtOffset_sec     = 3600 * -8;
const int daylightOffset_sec = 0;

//OBFUSCATE!
const char* ssid     = "YOUR SSID";
const char* password = "YOUR PASSWORD";
struct tm RTCtime;
int bright =  0;
int TZoffset = 0;
long starttime; 
Preferences preferences;

void setup() { 
    // the order of the next two statements does not affect the startup flash
    M5.begin();
    M5.Lcd.fillScreen(BLACK);
    // Get preferences from EEPROM - this is preferred interface than EEPROM.h
    // M5StickC-Plus/examples/Advanced/Storage/Counter for example
    // second param is "readonly"
    preferences.begin("NixieClock", false);
    bright = preferences.getInt( "bright", bright);
    // int +-1 hours
    TZoffset = preferences.getInt( "TZoffset", TZoffset);
    M5.Lcd.setRotation(3);
    M5.Axp.ScreenBreath(bright);
    // Seems to be included with base device .h, set to 115200
    Serial.println("works");

    // out of sheer laziness copy header images to bitmaps
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

    // timeout
    starttime = millis();
}

 
void loop() {
        int d1,d2,d3,d4;
        getRTCTime(); 
        RTCtime.tm_hour = (RTCtime.tm_hour + TZoffset) % 24;
        d1 = RTCtime.tm_hour / 10;        
        d2 = RTCtime.tm_hour % 10;
        d3 = RTCtime.tm_min / 10;        
        d4 = RTCtime.tm_min % 10;        
        M5.Lcd.drawBitmap(0 ,135/2-30,60,60,bitmap[d1]);
        M5.Lcd.drawBitmap(60 ,135/2-30,60,60,bitmap[d2]);
        M5.Lcd.drawBitmap(120 ,135/2-30,60,60,bitmap[d3]);
        M5.Lcd.drawBitmap(180 ,135/2-30,60,60,bitmap[d4]);
        M5.update();
        if (M5.BtnA.wasPressed() && M5.BtnB.wasPressed()) { 
            setRTCTime();
        } else if (M5.BtnA.wasPressed()) {
            bright++;
            if (bright > 13) { 
                bright = 7;
            }
            M5.Axp.ScreenBreath(bright);
            M5.Lcd.setCursor(2,120);
            M5.Lcd.print("SET BRIGHT: ");
            M5.Lcd.println(bright);
        } else if (M5.BtnB.wasPressed()) {
            // cycle through some TZoffsets
            TZoffset--;
            if (TZoffset < -10) { TZoffset = 0; }
            M5.Lcd.setCursor(0,120);
            M5.Lcd.print("TZ offset: ");
            M5.Lcd.print(TZoffset);
        } else {
            //M5.Lcd.setCursor(0,120);
            ////M5.Lcd.println("                ");
        }
        battStat();
        delay(200);
        if (millis() > (starttime + 20000) && M5.Axp.GetIdischargeData() > 0.0)
        { 
            // save brightness, tzoffset in prefs
            preferences.putInt("bright", bright);
            preferences.putInt("TZoffset", TZoffset);
            preferences.end();
            M5.Axp.PowerOff();
        }
}

uint16_t rgb888to565 (int r, int g, int b) {
    return (uint16_t) (
        ((r >> 3) & 0x1f) << 11 | 
        ((g >> 2) & 0x3f) << 5 |
         (b >> 3) & 0x1f );  
}


void setRTCTime() {
    // REWRITE: Prompt for SSID, password, then set RTC to UTC
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    struct tm timeinfo;

    M5.Lcd.setCursor(0,0);
    M5.Lcd.println("Connect USB to serial at 115200...");
    M5.Lcd.println("Type CR in the console when ready...");
    WiFi.mode(WIFI_STA);
    M5.Lcd.printf("\nConnecting to %s", ssid);
    WiFi.begin(ssid, password);  // Connect wifi and return connection status.
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);         // delay 0.5s.  延迟0.5s
        M5.Lcd.print(".");
    }
    M5.Lcd.println("\nCONNECTED!");
    gmtOffset_sec = 0;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //  get the time
    // tm_sec,tm_min,tm_hr,tm_mday,tm_wday,tm_mon,tm_year,tm_yday,tm_isdst
    // Unfortunate name, we are really getting NTP time
    // storing local time for now out of sheer laziness
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

void battStat() {
    int xorigin = 200;
    int yorigin = 1;
    int c = GREEN;
    float batVoltage = M5.Axp.GetBatVoltage();
    // from Core2_factory_test.ino
    float batPct = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100.;
    if (batPct < 5.0) { 
        c = RED;
    }
    M5.Lcd.drawRect(xorigin, yorigin,30,15, c);
    M5.Lcd.drawRect(xorigin+29, yorigin+4,4,7,c);
    if (batPct > 25.0) {
        M5.Lcd.fillRect(xorigin+2, yorigin+2,8,11,GREEN);
    }
    if (batPct > 50.0) {
        M5.Lcd.fillRect(xorigin+11, yorigin+2,8,11,GREEN);
    }
    if (batPct > 75.0) {
        M5.Lcd.fillRect(xorigin+20, yorigin+2,8,11,GREEN);
    }
    
    
}
