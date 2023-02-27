void connectToWifi(char ssid[], char password[]){
  if (WiFi.status() != WL_CONNECTED)    //In case the Wifi connection, disconnect first
    {
      WiFi.disconnect();
      delay(1000);
    }
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  
  //The ESP tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

}
