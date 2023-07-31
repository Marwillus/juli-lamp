
//// LOOP /////////////////////////////////////////////////////////////////////////////////
void loop() {
  currentTime = millis();
  potiTick();
  buttonTick();

  // check values after effect change
  // if (effect != selectedEffect) {
  //   printStuff();
  //   selectedEffect = effect;
  // }

  // bounce time
  // if (currentTime - pixelPrevious >= pixelInterval) {
  //   pixelPrevious = currentTime;

  // prevent flickering & unnessesary calculation
  // if (smoothPotiValue <= oldPotiValue - jitterThreshold || smoothPotiValue >= oldPotiValue + jitterThreshold || selectedEffect != oldSelectedEffect) {

  //  Run current frame
  mode();

  // update the strip
  strip.show();

  // }
}