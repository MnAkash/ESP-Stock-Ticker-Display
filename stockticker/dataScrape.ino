//Returns true if successfully scrapped data, fasle otherwise
bool stockScrape(char *tickerName) {
  char url[80] = "";
  strcat(url, serverName);
  strcat(url, tickerName);
  strcat(url, token);
  
  //Serial.println(url);
  //Definining Everythinig into Zero and Null before sraping data
//  const char* tickerSymbol = "-";
//  tickerString = tickerSymbol;
  memset(tickerSymbol, 0, strlen(tickerSymbol));
  strcpy(tickerSymbol, "-");
  
  marketprice_float = 0.0;
  increase_float = 0.0;
  ratio_float = 0.0;
  
  if (WiFi.status() == WL_CONNECTED) {
    if (http.begin(client, url)) {

      int httpCode = http.GET();                //Check feedback if there's a response
      //Serial.println(httpCode);
      if (httpCode > 0)
      {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = http.getString();
          //Serial.println(payload);
          DynamicJsonDocument doc(2048);
          DeserializationError error = deserializeJson(doc, payload);
          if (error)
          {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return 0;
          }
          JsonObject chart_result_0 = doc["chart"]["result"][0];
          JsonObject chart_result_0_meta = chart_result_0["meta"];

          strcpy(tickerSymbol , chart_result_0_meta["symbol"]);
          //tickerString = tickerSymbol;


          marketprice_float = chart_result_0_meta["regularMarketPrice"];
          PreviousClose_float = chart_result_0_meta["chartPreviousClose"];

          increase_float = marketprice_float - PreviousClose_float;
          if (increase_float == 0) {
            ratio_float = 0;
          }
          else {
            ratio_float = (increase_float / PreviousClose_float) * 100;
          }

          
          //I choosed to display it on the serial monitor to help you debug
//          Serial.print(tickerSymbol);
//          Serial.print(" ");
//          Serial.print(marketprice_float);
//          Serial.print(" ");
//          Serial.print(increase_float);
//          Serial.print(" ");
//          Serial.print(ratio_float);
//          Serial.println("%");

        }
      }
      else //If we can't get data
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return false;
      }

      http.end();

      return true;
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    return false;
  }
  return true;
}
