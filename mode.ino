void mode() {
  // if just activated play intro animation
  if (activated != active) {

    // intro animation only once
    if (activated) {
      if (effect < 1) {
        bouncingIntro();
      } else {
        active = activated;
      }
    }

    // outro animation
    if (!activated) {
      int b = fadeFromTo((255 * brightnessFactor), 0, 2);
      gradialFill(b);
      powerLed(b + 10);
      animationInProgress = true;
      if (b <= 0) {
        delay(200);
        powerLed(0);
        while (animationInProgress) {
          currentTime = millis();
          bool done = bouncingOutro();
          if (done) {
            animationInProgress = false;  // Animation completed, prevent further execution
            active = b;
          }
        }
      }
    }
  }


  if (!activated) return;

  // start fade between effects only once
  if (effect != previousEffect) {
    runEffectFade = true;
    logger("start fade");
    fadeMode();
    previousEffect = effect;
  }

  // normal running mode
  if (activated == active) {
    switch (effect) {
      case 0:
        gradialFill(255);
        break;

      case 1:
        fillStripWithColor();
        break;

      case 2:
        rainbow(100);
        break;

      case 3:
        gradialColorFill();
        break;

      default:
        effect = 0;
        break;
    }
    powerLed(smoothPotiValue);
  }
}

void fadeMode() {
  while (runEffectFade) {
    // make a snapeshot of start & end
    logger("effect while fade");
    logger(effect);
    switch (effect) {

      case 0:
        gradialColorFill(20);
        fadeSnapshot(true);
        gradialFill(smoothPotiValue, false);
        fadeSnapshot(false);
        break;

      case 1:
        gradialFill(smoothPotiValue);
        fadeSnapshot(true);
        fillStripWithColor(false);
        fadeSnapshot(false);
        break;

      case 2:
        fillStripWithColor();
        fadeSnapshot(true);
        rainbow(20, false);
        fadeSnapshot(false);
        break;

      case 3:
        rainbow(20);
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
