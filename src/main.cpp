// DS1302_Serial_Easy (C)2010 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// Adopted for DS1302RTC library by Timur Maksimov 2014
//
// A quick demo of how to use my DS1302-library to
// quickly send time and date information over a serial link
//
// I assume you know how to connect the DS1302.
// DS1302:  CE pin    -> Arduino Digital 27
//          I/O pin   -> Arduino Digital 29
//          SCLK pin  -> Arduino Digital 31
//          VCC pin   -> Arduino Digital 33
//          GND pin   -> Arduino Digital 35

#include <Arduino.h>
#include <Time.h>
#include <DS1302RTC.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Set pins:  CE(RST),IO(DAT),CLK
DS1302RTC RTC(8, 7, 6);

// sets relay to pin
#define RELAY1 2
#define RELAY2 3

#define BUTT_LIGHT 5

// update time on RTC?
#define TIMESET false

// defines display
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/* SETS VARIABLES WHEN TO WATER PLANTS */
// Pump 1
int watering1[3] = {19, 0, 0}; // when to foce water (hh:mm:ss)
int watering_pumptime1[3] = {0, 0, 58}; // for how long water (hh:mm:ss)

// Pump 2
int watering2[3] = {19, 0, 0}; // when to foce water (hh:mm:ss)
int watering_pumptime2[3] = {0, 0, 58}; // for how long water (hh:mm:ss)

time_t displayOnTime = 15; // display informations for x seconds
time_t displayTimestamp = 0;

// ------------ ICONS DEFINITIONS -------------

// dimensions: 12x16
const unsigned char icon_drop [] PROGMEM = {
  0x04, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x0e, 0x00, 0x1f, 0x00, 0x17, 0x00, 0x37, 0x80, 0x67, 0xc0,
0x4f, 0xc0, 0xdf, 0xe0, 0x9f, 0xe0, 0x8f, 0xe0, 0xc7, 0xe0, 0x67, 0xc0, 0x3f, 0x80, 0x0e, 0x00
};
// dimensions 128x64
const unsigned char bg_image [] PROGMEM = {
  // 'waterpump_bg'
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
0x3a, 0xab, 0x85, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1d, 0x55, 0xc2, 0x9c,
0x2a, 0xba, 0x8f, 0xb0, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x15, 0x5d, 0x47, 0xc4,
0x3a, 0xab, 0x85, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1d, 0x55, 0xc2, 0x9c,
0x22, 0xaa, 0x0f, 0x90, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x11, 0x55, 0x07, 0xd0,
0x23, 0xaa, 0x05, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x11, 0xd5, 0x02, 0x9c,
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
0xff, 0xff, 0xff, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xff, 0xff, 0xff, 0xff,
0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xc0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x2b, 0xbb, 0xb9, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x15, 0xdd, 0xdc, 0xee, 0x00, 0x00,
0x00, 0x00, 0x2a, 0x92, 0x29, 0x48, 0x00, 0x00, 0x00, 0x00, 0x15, 0x49, 0x14, 0xa4, 0x00, 0x00,
0x00, 0x00, 0x2b, 0x93, 0x31, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x15, 0xc9, 0x98, 0xe4, 0x00, 0x00,
0x00, 0x00, 0x3a, 0x92, 0x29, 0x48, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x49, 0x14, 0xa4, 0x00, 0x00,
0x00, 0x00, 0x2a, 0x93, 0xa9, 0x48, 0x00, 0x00, 0x00, 0x00, 0x15, 0x49, 0xd4, 0xa4, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3b, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0xdc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x22, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x54, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x32, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x58, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x22, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x54, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x23, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xd4, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// --------------------------------------------------------------------------

class Pump {
  int pumpPin;
  unsigned long previousMillis;

  public:
  Pump(int pin){
    pumpPin = pin;
    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin,HIGH); // to mozna udelat az dole
  }

  void startWater(){
    digitalWrite(pumpPin, LOW); // turns ON the pump
  }

  void stopWater(){
    digitalWrite(pumpPin, HIGH); // turns OFF the pump
  }

};

String to2digits(int number) {
  String res = String(number);
  if (number >= 0 && number < 10)
    res = "0" + res;
  return res;
}

void showTime(int x, int y, tmElements_t timeElem){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x,y);
  display.print(to2digits(timeElem.Hour));
  display.print(":");
  display.print(to2digits(timeElem.Minute));
  display.print(":");
  display.print(to2digits(timeElem.Second));
  //Serial.print(timeElem.Hour);Serial.print(timeElem.Minute);Serial.println(timeElem.Second);
}

//time_t timeToTimestamp(tmElements_t actualTime, int hmsStartArray[], int hmsStopArray[] = {false})

time_t timeToTimestampStart(tmElements_t actualTime, int hmsStartArray[]){
  tmElements_t elTimeStart;

  elTimeStart.Year = actualTime.Year;
  elTimeStart.Month = actualTime.Month;
  elTimeStart.Day = actualTime.Day;
  elTimeStart.Hour = hmsStartArray[0];
  elTimeStart.Minute = hmsStartArray[1];
  elTimeStart.Second = hmsStartArray[2];

  return makeTime(elTimeStart);
}

time_t timeToTimestampStop(tmElements_t actualTime, int hmsStartArray[], int hmsStopArray[]){
  tmElements_t elTimeStop;

  elTimeStop.Year = actualTime.Year;
  elTimeStop.Month = actualTime.Month;
  elTimeStop.Day = actualTime.Day;
  elTimeStop.Hour = hmsStartArray[0] + hmsStopArray[0];
  elTimeStop.Minute = hmsStartArray[1] + hmsStopArray[1];
  elTimeStop.Second = hmsStartArray[2] + hmsStopArray[2];

  return makeTime(elTimeStop);
}


// *****************

Pump waterPump1(RELAY1);
Pump waterPump2(RELAY2);

void setup()
{
  time_t t;
  tmElements_t tm;

  // Setup Serial connection
  Serial.begin(9600);

  // setup buttons
  pinMode(BUTT_LIGHT,INPUT_PULLUP);

  // setup display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  //display.dim(true);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("PLANT");
  display.println("PUMPER");
  display.setTextSize(1);
  display.print("v0.4");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  if (RTC.haltRTC()) {
    Serial.println("The DS1302 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  }
  if (!RTC.writeEN()) {
    Serial.println("The DS1302 is write protected. This normal.");
    Serial.println();
  }

  // sets time manually (you must set it first)
  if (TIMESET) {
    tm.Year = CalendarYrToTm(2017);
    tm.Month = 6;
    tm.Day = 14;
    tm.Hour = 21;
    tm.Minute = 10;
    tm.Second = 0;
    t = makeTime(tm);
    if (RTC.set(t) == 0) {
      Serial.println("Time set!");
    }
  }

  if (! RTC.read(tm)) {
    displayTimestamp = makeTime(tm) + displayOnTime; // sets stopping point for display show time
    Serial.print("displayTimestamp ");
    Serial.println(displayTimestamp);
  }
}

void loop()
{
  tmElements_t tm;
  time_t timeNow;

  time_t timePump1_start;
  time_t timePump1_stop;
  time_t timePump2_start;
  time_t timePump2_stop;


  if (! RTC.read(tm)) {
    if (tmYearToCalendar(tm.Year) >= 2017) {

      display.clearDisplay();

      display.drawBitmap(0, 0, bg_image, 128, 64, 1); // draw background
      showTime(41, 0, tm); // shows time


      // draw watering times and pump times
      display.setCursor(8, 40);
      display.print(to2digits(watering1[0]));
      display.print(":");
      display.print(to2digits(watering1[1]));
      display.print(":");
      display.print(to2digits(watering1[2]));

      display.setCursor(8, 57);
      display.print(to2digits(watering_pumptime1[0]));
      display.print(":");
      display.print(to2digits(watering_pumptime1[1]));
      display.print(":");
      display.print(to2digits(watering_pumptime1[2]));

      display.setCursor(73, 40);
      display.print(to2digits(watering2[0]));
      display.print(":");
      display.print(to2digits(watering2[1]));
      display.print(":");
      display.print(to2digits(watering2[2]));

      display.setCursor(73, 57);
      display.print(to2digits(watering_pumptime2[0]));
      display.print(":");
      display.print(to2digits(watering_pumptime2[1]));
      display.print(":");
      display.print(to2digits(watering_pumptime2[2]));


      // calculating time into unix timestamp
      timeNow = makeTime(tm);

      timePump1_start = timeToTimestampStart(tm, watering1);
      timePump1_stop = timeToTimestampStop(tm, watering1, watering_pumptime1);
      timePump2_start = timeToTimestampStart(tm, watering2);
      timePump2_stop = timeToTimestampStop(tm, watering2, watering_pumptime2);


      // pump #1
      if ( timeNow >= timePump1_start && timeNow < timePump1_stop ) {
        display.drawBitmap(29, 15, icon_drop, 16, 16, 1);  // draw drop
        waterPump1.startWater();
      } else {
        waterPump1.stopWater();
      }

      // pump #2
      if ( timeNow >= timePump2_start && timeNow < timePump2_stop ) {
        display.drawBitmap(89, 15, icon_drop, 16, 16, 1);  // draw drop
        waterPump2.startWater();
      } else {
        waterPump2.stopWater();
      }

      // display show informations
      int buttLightVal = digitalRead(BUTT_LIGHT);
      if (buttLightVal == LOW) {
        displayTimestamp = makeTime(tm) + displayOnTime; // make display light longer
      }

      if (timeNow >= displayTimestamp) {
        display.clearDisplay();
      }

      display.display(); // write everything to display
    }

  } else {
    Serial.println("DS1302 read error! Please check the circuitry.");
    Serial.println();
    delay(5000);
  }

}
