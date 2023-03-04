void setLEDStripColorTo(int red, int green, int blue) {
  ledStrip.clear();
  for (int i = 0; i < stripLEDTotal; i++) { // For each pixel...

    // ledStrip.Color() takes RGB values, from 0,0,0 up to 255,255,255
    ledStrip.setPixelColor(i, ledStrip.Color(red, green, blue));

    ledStrip.show();   // Send the updated pixel colors to the hardware.
  }
}
