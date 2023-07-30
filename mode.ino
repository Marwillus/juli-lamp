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
      int b = fadeFromTo(smoothPotiValue, 0, 2);
      gradialFill(b);
      powerLed(b);

      if (b <= 0) {
        // TODO outro animation
        // if (animationInProgress) {
        //   bool done = bouncingOutro();
        //   if (done) {
        // animationInProgress = false;  // Animation completed, prevent further execution
        active = b;
        //   }
        // }
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
        // gradialFill(smoothPotiValue);
        gradialColorFill();

        break;
      case 1:
        fillStripWithColor();
        break;
      case 2:
        rainbow(100);
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
        rainbow(20);
        gradialFill(smoothPotiValue, false);
        break;
      case 1:
        gradialFill(smoothPotiValue);
        fillStripWithColor(false);
        break;
      case 2:
        fillStripWithColor();
        rainbow(20, false);
        break;
      default:
        return;
    }
    // execute fade
    fadeBetweenEffects();
  }
}
