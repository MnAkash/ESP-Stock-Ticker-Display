/*
   Author : Moniruzzaman Akash
   Email : akashmoniruzzaman@gmail.com
   Whatsapp: +8801712878635
   Last Edited: Feb 26, 2023

  Circuit Diagram:
  Display   -  ESP8266
  ---------------------
  Din     -   D5
  5V      -   Vin
  GND     -   GND

  LED strip -  ESP8266
  ---------------------
  Din     -   D1
  5V      -   Vin(Not necessary if Display 5V in connected to Vin)
  GND     -   GND

  Bluetooth -   ESP8266
  ----------------------
   5V     - Vin or 3.3V
   GND    -   GND
   Tx     -    D6
   Rx     -    D7
*/

#include <ESP8266WiFi.h>        //Use ESP8266 functions
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ArduinoJson.h>


#define panelDinPin 14 //D5
#define rowSize 8
#define charWidth 5

#define LEDstripPin 5 //D1
#define stripLEDTotal 60

#define isSaved_ADDR 64 // Address of the EEPROM if credentials saved
#define SSID_ADDR 0 // Start address of the SSID string in EEPROM
#define PASS_ADDR 32 // Start address of the password string in EEPROM

bool isDisplayOn = true;//svae status whether display is on/off

char ssid[20] = "Wifi";      //Wifi name
char password[20] = "pass";  //WIFI Password

char serverName[] = "https://query2.finance.yahoo.com/v8/finance/chart/";
char token[] = "?interval=1d";

const int stockQty = 20; // this is the amount of different stock values to be contained
//const char *tickers[stockQty] = {"SPY", "GE", "IBM", "KD", "EMB", "BLV", "LAC", "GOOGL", "NIO", "CHPT", "O", "VICI", "JEPI", "MPW", "BXMX", "WWR", "BABA", "NOK", "PFE", "SHOP"};
char tickers[150] = ""; //Will be in "SPY,GE,IBM,KD,EMB)" format
char tempTicker[150] = "";//to temporarily store tickers while turned off display

char tickerSymbol[15] = "-";
char exchangeName[10] = "";

float marketprice_float, PreviousClose_float, increase_float, ratio_float;


int scroll_speed = 90;//Data scroll Speed of the display (can be in between 0-100)
int DelayValue = 54;//90% speed gived delayValue of 54. Will be used to store mapped value for scroll_speed
int brightness = 5; // Brightness of the display (can be in between 0-100)


int panel_no = 1;   // How many panels connected together to form the display
int singlePanleLEDs = 256;
int singlePanelColumns = 32;

byte displayOutput[2560] = {0};//ideally columnSize
byte displayColors[2560] = {0};//ideally columnSize
int ledTotal = singlePanleLEDs*panel_no;
int columnSize = singlePanelColumns*panel_no;

//RGB values for led strip of housing light
int red = 255;
int green = 255;
int blue = 255;

Adafruit_NeoPixel panel = Adafruit_NeoPixel(1024, panelDinPin, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(stripLEDTotal, LEDstripPin, NEO_GRB + NEO_KHZ800);
WiFiClientSecure client;//Create a WiFi client and http client
HTTPClient http;
SoftwareSerial Bluetooth(D6, D7);//rxPin, txPin >> HC05(txPin, rxPin)


void setup() {
  client.setInsecure();
  Serial.begin(115200);
  Bluetooth.begin(9600);
  EEPROM.begin(512);// Initialize EEPROM with size 512 bytes
  delay(10);

  panel.begin();
  panel.show();
  setBrightnessTo(brightness);

  ledStrip.begin();
  setLEDStripColorTo(red, green, blue);//Initially set to White

  scrollText("GOLDEN A$$ET$");

  if (isAlreadySaved()) {//if credentials are saved in memory connect to that address
    strcpy(ssid, readStringFromEEPROM(SSID_ADDR).c_str() );  //get ssid from EEPROM
    strcpy(password, readStringFromEEPROM(PASS_ADDR).c_str() ); //get password EEPROM
    connectToWifi(ssid, password);//Remove comment to connect to wifi first
  }
  else sendToApp('D');//'D' for disconnected


  Serial.println("Program Started...");
}

void loop() {
  int j = 0; //for saving ticker chareters from tickers variable
  char ticker[15] = "";//use black ticker variable before saving anything to ticker
  //each ticker has 5 letter each. At best for 20 ticker, 'tickers' length can be 5*20=100
  for (int i = 0; i < 150; i++) {

    if (tickers[i] == 0) { //Null
      break;
    }
    else if (tickers[i] == ',' || tickers[i] == ')') {
      //When "," or ")" appears, it means one ticker is ready to scrape.
      ticker[j] = '\0';
      //===========Scrape data
      bool success = stockScrape(ticker);
      if (success ) {//if data scrapped successfully display data
        sendToApp('C');//'C' for connected
        byte Return = displayled();
        if (Return == 1)break;
      }
      else {
        sendToApp('D');//'D' for disconnected
      }

      //delay(100);

      char ticker[15] = "";//Then clear ticker variable for next one
      j = 0;

      if (tickers[i] == ')')break; //if it is ")" break the for loop to start over
    }

    else if (tickers[i] != 0) {
      ticker[j++] = tickers[i];//get each character one by one from tickers
    }

    yield();
  }//end for
  hanldeBlueetoothData();

}
