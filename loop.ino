
//// LOOP /////////////////////////////////////////////////////////////////////////////////
void loop() {
  currentTime = millis();
  potiTick();
  buttonTick();

  //  Run current frame
  mode();

  // update the strip
  strip.show();
}