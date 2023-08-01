void mode() {
  // if just activated play intro animation
  if (activated != active) {

    // intro animation only once
    if (activated) {
      bouncingIntro();
      fadeAllIn();
    }

    // outro animation
    if (!activated) {
      fadeAllOut();
      bouncingOutro();
      // animationInProgress = true;
      // if (brightnessFactor <= 0) {
      //   delay(200);
      //   powerLed();
      //   while (animationInProgress) {
      //     currentTime = millis();
      //     bool done = bouncingOutro();
      //     if (done) {
      //       animationInProgress = false;  // Animation completed, prevent further execution
      //       active = false;
      //     }
      //   }
      // }
    }
  }

  if (!activated) return;

  if (activated == active) {
    // start fade between effects only once
    if (effect != previousEffect) {
      runEffectFade = true;
      logger("start fade");
      fadeModes();
      previousEffect = effect;
    }

    runMode();
  }
}

// runs one frame of the selected effect
void runMode() {
  switch (effect) {
    case 0:
      gradialFill();
      break;

    case 1:
      fillStripWithColor();
      break;

    case 2:
      rainbow();
      break;

    case 3:
      gradialColorFill();
      break;

    default:
      effect = 0;
      break;
  }
  powerLed();
}

// runs one animation between the effects
void fadeModes() {
  while (runEffectFade) {
    // make a snapeshot of start & end
    logger("effect while fade");
    logger(effect);
    switch (effect) {

      case 0:
        gradialColorFill();
        fadeSnapshot(true);
        gradialFill();
        fadeSnapshot(false);
        break;

      case 1:
        gradialFill();
        fadeSnapshot(true);
        fillStripWithColor();
        fadeSnapshot(false);
        break;

      case 2:
        fillStripWithColor();
        fadeSnapshot(true);
        rainbow();
        fadeSnapshot(false);
        break;

      case 3:
        rainbow();
        fadeSnapshot(true);
        gradialColorFill();
        fadeSnapshot(false);
        break;

      default:
        return;
    }
    // execute fade
    fadeBetweenEffects();
  }
}
