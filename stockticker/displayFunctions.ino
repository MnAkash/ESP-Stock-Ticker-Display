// was 68 with lower case chars, 42 without -- 40 without l/r arrow -- 42 WITH PLUS AND DOT
byte displayLetters[44][charWidth] = {  // [A] = chars/digits contained in array, [B] = charWidth (no need to add 1, as was 'one indexed' at #define)
  {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 // , 0x00
  {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
  {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
  {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
  {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9

  {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
  {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
  {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7f, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G
  {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
  {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
  {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M
  {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
  {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
  {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
  {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
  {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
  {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
  {0x3f, 0x40, 0x38, 0x40, 0x3f}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z

  {0x00, 0x00, 0x00, 0x00, 0x00}, // space
  {0x10, 0x18, 0x1c, 0x18, 0x10}, // new up arrow
  {0x04, 0x0c, 0x1c, 0x0c, 0x04}, // new down arrow
  {0x08, 0x08, 0x08, 0x08, 0x08}, // dash / maintain
  {0x08, 0x08, 0x3E, 0x08, 0x08}, // plus
  {0x00, 0x60, 0x60, 0x00, 0x00}, // full stop
  {0x26, 0x16, 0x08, 0x34, 0x32}, // %
  {0x24, 0x2a, 0xff, 0x2a, 0x12}// $
};

byte displayled() {
  String inputString;
  char Amount[sizeof(marketprice_float) + 8];
  char Increase[sizeof(increase_float) + 8];
  char Ratio[sizeof(ratio_float) + 8];

  if ((String)exchangeName == "PNK") { //for OTC market 4 digit after decimal
    dtostrf(marketprice_float, sizeof(marketprice_float), 4, Amount);
    dtostrf(increase_float, sizeof(increase_float), 4, Increase);
    dtostrf(ratio_float, sizeof(ratio_float), 4, Ratio);
  }
  else if ((String)exchangeName == "CCC") { //for Crypto market 4 digit after decimal
    dtostrf(marketprice_float, sizeof(marketprice_float), 4, Amount);
    dtostrf(increase_float, sizeof(increase_float), 4, Increase);
    dtostrf(ratio_float, sizeof(ratio_float), 4, Ratio);
  }
  else {
    dtostrf(marketprice_float, sizeof(marketprice_float), 2, Amount);
    dtostrf(increase_float, sizeof(increase_float), 2, Increase);
    dtostrf(ratio_float, sizeof(ratio_float), 2, Ratio);
  }


  if (increase_float < 0) {
    inputString = ((String)tickerSymbol + " " + "$" + Amount + " " + "|" + Increase + " " + "|" + Ratio + "% ");
  }
  else if (increase_float >= 0) {
    inputString = ((String)tickerSymbol + " " + "$" + Amount + " " + "^" + "+" + Increase + " " + "^" + "+" + Ratio + "% ");
  }
  Serial.println(inputString);

  for (int i = 0; i < inputString.length(); i++) {

    byte Return = hanldeBlueetoothData();
    if (Return == 1) {
      return 1;
    }

    char currentChar = inputString[i];
    int currentCol, colorMode;// = int(colString[i]) - 48; // minus 48 because int("0") will return the ascii value of char '0' (48)

    if ((currentChar >= 'A') && (currentChar <= 'Z')) {
      currentCol = 0;
    } /*else if ((currentChar == '+') || (currentChar == '-')) {
      currentCol = colorMode;
    } *//*else if((currentChar >= '0') && (currentChar <= '9') ||(currentChar == '%')){
      colorMode = 3;
      currentCol = colorMode;
    }*/else if ((currentChar == '^') || (currentChar >= '0') && (currentChar <= '9') && (currentChar == '%')) {
      colorMode = 1;
      currentCol = colorMode;
    } else if ((currentChar == '|') || (currentChar >= '0') && (currentChar <= '9') && (currentChar == '%')) {
      colorMode = 2;
      currentCol = colorMode;
    }
    else if ((currentChar == '$') || (currentChar >= '0') && (currentChar <= '9') && (currentChar == '%')) {
      colorMode = 4;
      currentCol = colorMode;
    }

    int indexValue;
    if ((currentChar >= '0') && (currentChar <= '9')) indexValue = currentChar - 48; // 0-9
    else if ((currentChar >= 'A') && (currentChar <= 'Z')) indexValue = currentChar - 55; // upper case
    //else if ((currentChar >= 'a') && (currentChar <= 'z')) indexValue = currentChar - 61; // lower case , check that these are in the right place / the offset is correct, as i may have put the array chunk back in the wrong place!!
    else if ((currentChar == ' ')) indexValue = 36; // space, was 62 when lower case chars
    else if ((currentChar == '^')) indexValue = 37; // up arrow, was 63 when lower case chars
    else if ((currentChar == '|')) indexValue = 38; // down arrow, was 64 when lower case chars
    else if ((currentChar == '%')) indexValue = 42;
    //    else if ((currentChar == '<')) indexValue = 39; // left arrow, was 65 when lower case chars
    //    else if ((currentChar == '>')) indexValue = 40; // right arrow, was 66 when lower case chars
    else if ((currentChar == '-')) indexValue = 39; // dash / maintain, was 67 when lower case chars, 41 with l/r arrows
    else if ((currentChar == '+')) indexValue = 40; // plus
    else if ((currentChar == '.')) indexValue = 41; // full stop
    else if ((currentChar == '$')) indexValue = 43; // full stop

    for (int x = 0; x < charWidth; x++) {
      nudgeColumns(displayLetters[indexValue][x], currentCol); // inputCols[i]); // panel.Color(0,0,255));
      displayUpdate(0);
      delay(DelayValue);
    }
    nudgeColumns(0, 0); // - 1 might break things here? or make things go white due to the lookup default being white
    displayUpdate(0);
    delay(DelayValue); // this will be 200 as per the rest once confirmed that the double jump issue is done!
  }

  return 0;
}

void scrollText(String text) {
  int color = 4;//yellow
  int indexValue;
  for (int i = 0; i < text.length(); i++) {
    char currentChar = text[i];
    if ((currentChar >= 'A') && (currentChar <= 'Z')) indexValue = currentChar - 55;
    else if ((currentChar == ' ')) indexValue = 36;
    else if ((currentChar == '$')) indexValue = 43; // full stop
    else indexValue = 36;//make a space

    for (int x = 0; x < charWidth; x++) {
      nudgeColumns(displayLetters[indexValue][x], color);
      displayUpdate(0);
      delay(DelayValue);
    }
    nudgeColumns(0, 0);
    displayUpdate(0);
    delay(DelayValue);
  }
}


//int getMappedPixel(int columnID, int rowID) {
//  int z;
//
//  z = (columnSize-1) - columnID;
//  if (columnID % 2) z++;
//
//  z = z * (rowSize);
//  if (columnID % 2) z = z - rowID - 1;
//  else z = z + rowID;
//
//  if (z >= ledTotal) z++;
//  return z;
//}
int getMappedPixel(int columnID, int rowID) {
  int panel_num = columnID / 8;
  // Calculate the LED number within the panel
  int led_num = rowID * 8 + (columnID % 8);
  // Calculate the overall LED number
  int overall_led_num = (panel_num * 64) + led_num;
  return overall_led_num;
}


void setMappedColor(int columnID, int rowID, long pixelColor) {
  int mappedPixel = getMappedPixel(columnID, rowID);

  if (mappedPixel < ledTotal) {
    panel.setPixelColor(mappedPixel, pixelColor);
  }
  else {
    mappedPixel = mappedPixel - ledTotal;
    panel.setPixelColor(mappedPixel, pixelColor);
  }
}

void colorAll(long color) {
  for (int i = 0; i < rowSize; i++) {
    for (int j = 0; j < columnSize; j++) {
      setMappedColor(j, i, color);
    }
  }
}

void convertToBits(byte inputValue, int outputArray[]) {
  if (inputValue & 0x80) outputArray[0] = 1; // not used?
  if (inputValue & 0x40) outputArray[0] = 1;
  if (inputValue & 0x20) outputArray[1] = 1;
  if (inputValue & 0x10) outputArray[2] = 1;
  if (inputValue & 0x08) outputArray[3] = 1;
  if (inputValue & 0x04) outputArray[4] = 1;
  if (inputValue & 0x02) outputArray[5] = 1;
  if (inputValue & 0x01) outputArray[6] = 1;
}

void displayUpdate(long backColor) {
  colorAll(backColor);
  for (int i = 0; i < columnSize; i++) {
    int columnData[rowSize] = {0};
    convertToBits(displayOutput[i], columnData);
    for (int j = 0; j < rowSize; j++) {
      if (columnData[rowSize-1 - j] == 1) setMappedColor(i, j, colorLookup(displayColors[i])); // was displayColors[i][j] before memory save
    }
  }

  panel.show();
}

void nudgeColumns(byte newData, byte newColor) {
  for (int i = 0; i < (columnSize - 1); i++) {
    displayOutput[i] = displayOutput[i + 1];
    displayColors[i] = displayColors[i + 1];
  }
  displayOutput[columnSize - 1] = newData;

  if (newColor >= 0) { // created for memory save
    displayColors[columnSize - 1] = newColor;
  }
}

long colorLookup(int inputColor) {
  long outputColor = 0;
  switch (inputColor) {
    case 0:
      outputColor = panel.Color(255, 255, 255); // white
      break;
    case 1:
      outputColor = panel.Color(255, 0, 0); // red
      break;
    case 2:
      outputColor = panel.Color(0, 255, 0); // green
      break;
    case 3:
      outputColor = panel.Color(0, 0, 255); // blue
      break;
    case 4:
      outputColor = panel.Color(255, 255, 0); // yellow/amber
      break;
    case 5:
      outputColor = panel.Color(0, 255, 255); // cyan
      break;
    case 6:
      outputColor = panel.Color(255, 0, 255); // magenta
      break;
    default:
      outputColor = panel.Color(255, 255, 255); // white
      break;
  }

  return outputColor;
}

void setBrightnessTo(int value) {
  //value can be in between 0 to 100
  int pwmVal = map(value, 0, 100, 0, 255);
  panel.setBrightness(value);
}
void setScrollSpeedTo(int value) {
  //value can be in between 0 to 100
  DelayValue = map(value, 0, 100, 500, 5);
}
