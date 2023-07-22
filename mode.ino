void mode() {
  // if just activated play intro animation
  if (activated != active) {

    // intro animation
    if (activated) {
      if (effect < 1) {
        bouncingIntro();
      } else {
        // fadeToAnimation(uint32_t targetColor, unsigned long fadeDuration)
        active = activated;
      }
    }

    // outro animation
    if (!activated) {
      if (effect < 1) {
        int brightness = fadeFromTo(smoothPotiValue, 0, 4);
        gradialFill(brightness);
        powerLed(brightness * maxBrightness);

        if (brightness <= 0) {
          active = activated;
        }
      } else {
        // fadeout
      }
    }
  } 
  
  if (activated && activated == active) {

    switch (effect) {
      case 0:
        gradialFill(smoothPotiValue);
        powerLed(smoothPotiValue * maxBrightness);
        break;
      case 1:
        fillStripWithColor(smoothPotiValue);
        break;
      case 2:
        rainbow(100);
        break;
      default:
        effect = 0;
        break;
    }
  }
}
