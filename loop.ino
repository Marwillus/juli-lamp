
//// LOOP /////////////////////////////////////////////////////////////////////////////////
void loop() {
  currentTime = millis();
  potiTick();
  buttonTick();

  // if (button.isClick()) {
  //   Serial.println("click");
  // }
  // if (button.isReleased()) {
  //   Serial.println("up");
  // }
  if (button.isSingleClick()) {
    Serial.println("single click");
    activated = !activated;
    Serial.println(activated);
  }
  if (button.isDoubleClick()) {
    Serial.println("double click");
    effect++;
    Serial.println(effect);
  }
  if (button.isLongClick()) {
    Serial.println("long click");
    // alternative poti mode
  }

  // check values after effect change
  // if (effect != selectedEffect) {
  //   printStuff();
  //   selectedEffect = effect;
  // }

  // bounce time
  // if (currentTime - pixelPrevious >= pixelInterval) {

    // prevent flickering & unnessesary calculation
    // if (smoothPotiValue <= oldPotiValue - jitterThreshold || smoothPotiValue >= oldPotiValue + jitterThreshold || selectedEffect != oldSelectedEffect) {

    mode();

    // pixelPrevious = currentTime;
    // oldPotiValue = smoothPotiValue;
  // }
}