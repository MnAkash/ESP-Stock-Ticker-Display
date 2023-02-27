void connectToWifi(char ssid[], char password[]) {
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  Serial.print(",");
  Serial.print(password);
  
  WiFi.disconnect();
  //sendToApp('D');//'D' for disconnected
  delay(500);
  WiFi.mode(WIFI_STA);
  delay(1000);

  WiFi.begin(ssid, password);
  //Serial.print("Connecting");

  sendToApp('Q');//'C' for connecting
  unsigned long tick = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - tick > 10000) {
      Serial.println("Could not connect!");
      break;
    }
  }

  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) {
    sendToApp('C');//'C' for connected

    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    //The ESP tries to reconnect automatically when the connection is lost
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  }
  else{
    sendToApp('E');//'E' for disconnected and request to send again
  }
}

void writeStringToEEPROM(int addr, const String& str) {
  if (!isAlreadySaved()) {//if not already saved
    EEPROM.put(isSaved_ADDR, 1);//put 1 to remember credentials are saved
  }

  // Write a string to EEPROM starting at the specified address
  int strLen = str.length() + 1; // include null terminator
  for (int i = 0; i < strLen; i++) {
    EEPROM.write(addr + i, str[i]);
  }
  EEPROM.commit(); // save changes to permanent memory
}

String readStringFromEEPROM(int addr) {
  // Read a string from EEPROM starting at the specified address
  String str;
  char c;
  while ((c = EEPROM.read(addr++)) != '\0') {
    str += c;
  }
  return str;
}


bool isAlreadySaved() {
  int isSaved = 0;
  EEPROM.get(isSaved_ADDR, isSaved);

  if (isSaved == 1)return true;
  else return false;
}
