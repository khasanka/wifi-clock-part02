#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8

#define CLK_PIN   14 //D5
#define DATA_PIN  13 //D7
#define CS_PIN    15 //D8

const char *ssid     = "TronicsLab";
const char *password = "Kasun Hasanka";

WiFiUDP ntpUDP;
const long offsetInSeconds = 5.5 * 60 * 60;
NTPClient timeClient(ntpUDP, offsetInSeconds);
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
String previousTime;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  P.begin();
  ledDisplay("Connecting...");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  ledDisplay(WiFi.localIP().toString());
  timeClient.begin();
  delay(2000);
}

void loop() {

  timeClient.update();


  if (previousTime != timeClient.getFormattedTime()) {
    previousTime = timeClient.getFormattedTime();
    String timeString = get12ClockTime(timeClient);
    Serial.println(timeString);
    ledDisplay(timeString);
  }

}

void ledDisplay(String text) {
  int len = text.length() + 1;
  char textCharArray[len];
  text.toCharArray(textCharArray, len);
  P.displayText(textCharArray, PA_CENTER, 0, 0, PA_NO_EFFECT);
  P.displayAnimate();
}

String get12ClockTime(NTPClient timeClient) {
  // 00.20.10  / 09.40.30  / 15.21.30 / 21.45.20
  int hr = timeClient.getHours();
  String clockTime;

  if (hr == 0) {
    clockTime = "12";
  } else if (hr <= 12) {
    clockTime = convertIntToTwoDigitString(hr);
  } else {
    clockTime = convertIntToTwoDigitString(hr - 12);
  }

  clockTime.concat(":");
  clockTime.concat( convertIntToTwoDigitString(timeClient.getMinutes()) );
  clockTime.concat(":");
  clockTime.concat( convertIntToTwoDigitString(timeClient.getSeconds()) );

  if (timeClient.getHours() < 12) {
    clockTime.concat(" AM");
  } else {
    clockTime.concat(" PM");
  }
  return clockTime;
} // ------------- end of get12ClockTime

String convertIntToTwoDigitString(int digit) {
  String out;
  if (digit < 10) {
    out = "0";
    out.concat(digit);
  } else {
    out = String(digit);
  }
  return out;
}
