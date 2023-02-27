/*
   Author : Moniruzzaman Akash
   Email : akashmoniruzzaman@gmail.com
   Whatsapp: +8801712878635
   Last Edited: Feb 16, 2023

Circuit Diagram:
Display   -  ESP8266
---------------------
  Din     -   D5

Bluetooth -   ESP8266
----------------------
   5V     - Vin or 3.3V
   GND    -   GND
   Tx     -    D6
   Rx     -    D7
*/

#include <ESP8266WiFi.h>        //Use ESP8266 functions  
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define stripPin 14 //D5
#define rowSize 7 // zero indexed...
#define charWidth 5 // ** one indexed **


char ssid[20] = "Wifi";
char password[20] = "password";        //WIFI Password

int scroll_speed = 10;//Data scroll Speed of the display (can be in between 0-100)
int brightness = 70; // Brightness of the display (can be in between 0-100)
int panel_no = 1;   // How many panels connected together to form the display

int DelayValue = 50;//will be used to store mapped value for scroll_speed


char serverName[] = "https://query2.finance.yahoo.com/v8/finance/chart/";
char token[] = "?interval=1d";

const int stockQty = 20; // this is the amount of different stock values to be contained
//const char *tickers[stockQty] = {"SPY", "GE", "IBM", "KD", "EMB", "BLV", "LAC", "GOOGL", "NIO", "CHPT", "O", "VICI", "JEPI", "MPW", "BXMX", "WWR", "BABA", "NOK", "PFE", "SHOP"};
char tickers[150] = ""; //Will be in "SPY,GE,IBM,KD,EMB)" format

String tickerString;//Will be used to store scraped ticker symbol
String bluetoothString; //to save Bluetooth serial data


float marketprice_float, PreviousClose_float, increase_float, ratio_float;


int ledTotal = 255;//zero indexed
int columnSize = 31;//zero indexed

byte displayOutput[960] = {0};//ideally columnSize+1
byte displayColors[960] = {0};//ideally columnSize
byte currentStatus = 0; // 0 = off, 1 = on
unsigned int maxLoad = 0;
int flag = 0;
int val = 0; // val will be used to store the state of the input pin
int old_val = 0; // this variable stores the previous value of "val"
int state = 0; // 0 = LED off and 1 = LED on


Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledTotal + 1, stripPin, NEO_RGB + NEO_KHZ800);
WiFiClientSecure client;//Create a WiFi client and http client
HTTPClient http;
SoftwareSerial Bluetooth(D6, D7);//rxPin, txPin >> HC05(txPin, rxPin)



void setup() {
  client.setFingerprint("38 41 3B 07 A2 9F F4 0D FA 23 C8 4B 77 D6 45 1C D9 BD 9C F1");
  Serial.begin(115200);
  Bluetooth.begin(9600);

  //connectToWifi(ssid, password);//Remove comment to connect to wifi first
  strip.begin();
  strip.show();
  setBrightnessTo(brightness);

  Serial.println("Program Started...");
}
char ticker[10] = "";

void loop() {
  int j = 0; //for saving ticker chareters from tickers variable

  char ticker[10] = "";//use black ticker variable before saving anything to ticker
  //each ticker has 5 letter each. At best for 20 ticker, 'tickers' length can be 5*20=100
  for (int i = 0; i < 119; i++) {

    if (tickers[i] == ',' || tickers[i] == ')') {
      //When "," or ")" appears, it means one ticker is ready to scrape.
      ticker[j] = '\0';
      //===========Scrape data
      //Serial.print("Scraping: ");
      //Serial.println(ticker);
      stockScrape(ticker);
      displayled();
      delay(500);

      char ticker[10] = "";//Then clear ticker variable for next one
      j = 0;

      if (tickers[i] == ')')break; //if it is ")" break the for loop to start over
    }

    else if (tickers[i] != 0) {
      ticker[j++] = tickers[i];//get each character one by one from tickers
    }

    if (Bluetooth.available() > 0) {
      char data = Bluetooth.read();
      //Serial.println(data);

      if (data == '(') {//handle new tickers
        strcpy(tickers, Bluetooth.readString().c_str() );
        //strcpy(destination, source) char variable is not re-assignable. strcpy is used to assign new char string[]

        Serial.println(tickers);

        //Setting up variables to default before breaking the loop
        currentStatus = 0;
        maxLoad = 0;
        flag = 0;
        val = 0;
        old_val = 0;
        state = 0;
        break;//break the for loop to start from i=0 and j=0;
      }

      else if (data == 's') {//scroll speed setup
        //receives s<scroll_speed>
        scroll_speed = Bluetooth.readString().toInt();
        //since scroll_speed is global variable, assigning new value will work.
        //It will be used in display function
        setScrollSpeedTo(scroll_speed);
        
        Serial.print("Scroll Speed: ");
        Serial.println(scroll_speed);
        
      }

      else if (data == 'b') {//brightness setup
        //receives b<brightness>
        brightness = Bluetooth.readString().toInt();
        setBrightnessTo(brightness);
        Serial.print("Brightness: ");
        Serial.println(brightness);

      }

      else if (data == 'p') {//panel setup
        //receives p<brightness>
        panel_no = Bluetooth.readString().toInt();

        columnSize = 32*panel_no -1;
        ledTotal = 256*panel_no - 1;


        //Setting up variables to default before breaking the loop
        currentStatus = 0;
        maxLoad = 0;
        flag = 0;
        val = 0;
        old_val = 0;
        state = 0;
        Serial.println(panel_no);
        break;//break the for loop to start from i=0 and j=0;
      }

      else if (data == 'w') { //wifi setup
        //app sends wifi credentials in w<ssid>,<password> format
        //read rest of the data and parse ssid and password to connect to new wifi.

        bluetoothString = Bluetooth.readString(); //we already read intial 'w', now reading last "ssid,password"

        int commaIndex = bluetoothString.indexOf(',');          //first get the middel comman index
        strcpy(ssid, bluetoothString.substring(0, commaIndex).c_str() );  //get value before comma(ie. ssid)
        strcpy(password, bluetoothString.substring(commaIndex + 1).c_str() ); //get value after comma(ie. password)

        //strcpy(destination, source) char variable is not re-assignable. strcpy is used to assign new char string[]

        Serial.println("New Wifi set to :");
        Serial.print(bluetoothString);
        connectToWifi(ssid, password);//connect to wifi
      }

      else if (data == 'd') {//complete
        //when 'd' is received send <scroll_speed>,<brightness>,<panel_no> to app to update app labels
        Bluetooth.println((String) scroll_speed + "," + (String) brightness + "," + (String) panel_no );
      }

    }//end bluetooth avaialble

  }//end for
      //delay(3000);

}
