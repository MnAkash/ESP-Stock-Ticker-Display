//this fuction returns 0 normally, loop needs to be broken if returns 1
byte hanldeBlueetoothData() {
  if (Bluetooth.available() > 0) {
    char data = Bluetooth.read();
    Serial.println(data);

    if (data == '(') {//handle new tickers
      strcpy(tickers, Bluetooth.readString().c_str() );
      //strcpy(destination, source) char variable is not re-assignable. strcpy is used to assign new char string[]

      Serial.println(tickers);

      panel.clear();
      sendToApp('K');//'K' for okay
      return 1;//break;//break the for loop to start from i=0 and j=0;
    }

    else if (data == 'o') {//Turn on display
      if (!isDisplayOn) {
        Serial.println("ON");
        panel.show();
        setLEDStripColorTo(red, green, blue);

        strcpy(tickers, tempTicker);
      }
      isDisplayOn = true;
    }

    else if (data == 'f') {//Turn off display
      if (isDisplayOn) {
        Serial.println("OFF");
        panel.clear(); panel.show();
        ledStrip.clear(); ledStrip.show();

        strcpy(tempTicker, tickers);
        tickers[0] = 0;

        displayOutput[2560] = {0};
        displayColors[2560] = {0};

      }
      isDisplayOn = false;
      return 1;//break the main foor loop
    }

    else if (data == 's') {//scroll speed setup
      //receives s<scroll_speed>
      scroll_speed = Bluetooth.readString().toInt();
      setScrollSpeedTo(scroll_speed);
      //Serial.print("Scroll Speed: ");
      Serial.println(scroll_speed);
      sendToApp('K');//'K' for okay

    }

    else if (data == 'b') {//brightness setup
      //receives b<brightness>
      brightness = Bluetooth.readString().toInt();
      setBrightnessTo(brightness);
      //Serial.print("Brightness: ");
      Serial.println(brightness);
      sendToApp('K');//'K' for okay
    }

    else if (data == 'p') {//panel setup
      //receives p<no of panels>
      panel_no = Bluetooth.readString().toInt();

      columnSize = singlePanelColumns * panel_no;
      ledTotal = singlePanleLEDs * panel_no;

      panel.clear(); panel.show();
      Serial.println(panel_no);

      return 1;//break;//break the for loop to start from i=0 and j=0;
    }

    else if (data == 'w') { //wifi setup
      //app sends wifi credentials in w<ssid>,<password> format
      //read rest of the data and parse ssid and password to connect to new wifi.

      String bluetoothString = Bluetooth.readString(); //we already read intial 'w', now reading last "ssid,password"

      int commaIndex = bluetoothString.indexOf(',');          //first get the middel comman index
      strcpy(ssid, bluetoothString.substring(0, commaIndex).c_str() );  //get value before comma(ie. ssid)
      strcpy(password, bluetoothString.substring(commaIndex + 1).c_str() ); //get value after comma(ie. password)
      //strcpy(destination, source) char variable is not re-assignable. strcpy is used to assign new char string[]

      //Write to EEPROM
      writeStringToEEPROM(SSID_ADDR, bluetoothString.substring(0, commaIndex));
      writeStringToEEPROM(PASS_ADDR, bluetoothString.substring(commaIndex + 1));

      Serial.println(bluetoothString);

      connectToWifi(ssid, password);//connect to wifi
    }

    else if (data == 'd') {//complete
      //when 'd' is received send <scroll_speed>,<brightness>,<panel_no> to app to update app labels
      //(String) scroll_speed + "," + (String) brightness + "," + (String) panel_no
      Serial.printf("%d,%d,%d\n", scroll_speed, brightness, panel_no );
      Bluetooth.printf("%d,%d,%d", scroll_speed, brightness, panel_no );
    }

    else if (data == 'a') { //send wifi connection status
      if (WiFi.status() == WL_CONNECTED) {
        sendToApp('C');//'C' for connected
      }
      else {
        sendToApp('D');//'D' for disconnected
      }
    }

    else if (data == 'h') { //receives led strip color code in "h<Red>,<Green>,<Blue>" format
      String bluetoothString = Bluetooth.readString();
      Serial.println(bluetoothString);

      if (bluetoothString.length() >= 5) {
        int commaIndex1 = bluetoothString.indexOf(',');
        int commaIndex2 = bluetoothString.indexOf(',', commaIndex1 + 1);
        red = bluetoothString.substring(0, commaIndex1).toInt();
        green = bluetoothString.substring(commaIndex1 + 1, commaIndex2).toInt();
        blue = bluetoothString.substring(commaIndex2 + 1).toInt();

        setLEDStripColorTo(red, green, blue);
        sendToApp('K');//'K' for okay
      }
      else {
        Serial.println("Data Corrupted!");
        sendToApp('K');//'K' for okay
      }

    }

    else {
      Bluetooth.flush();
    }

  }

  return 0;
}


void sendToApp(char letter) {
  Serial.println(letter);
  Bluetooth.flush();
  Bluetooth.print(letter);
  //Bluetooth.println();
}
