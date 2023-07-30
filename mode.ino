// Function pointer array to store function pointers with the appropriate signatures
// Note the use of void (*)() for function pointers that take no parameters and return void
void (*functionArray[])() = { gradialFill, fillStripWithColor, rainbow };

// Function to run the selected effect based on the index
void runEffect(int effect) {
  if (effect >= 0 && effect < sizeof(functionArray) / sizeof(functionArray[0])) {
    functionArray[effect]();
  }
}

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
      if (effect < 1) {
        int b = fadeFromTo(smoothPotiValue, 0, 2);
        gradialFill(b, 0);
        powerLed(b);

        if (b <= 0) {
          active = b;
        }
      } else {
        strip.clear();
        strip.show();
        powerLed(0);
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
        gradialFill(smoothPotiValue, effect);
        powerLed(smoothPotiValue);
        break;
      case 1:
        fillStripWithColor(effect);
        powerLed(secondaryPoti);
        break;
      case 2:
        rainbow(100, effect);
        powerLed(secondaryPoti);
        break;
      default:
        effect = 0;
        break;
    }
  }
}

void fadeMode() {
  while (runEffectFade) {
    // make a snapeshot of start & end
    logger("effect while fade");
    logger(effect);
    switch (effect) {
      case 0:
        rainbow(20, effect);
        gradialFill(smoothPotiValue, effect);
        break;
      case 1:
        gradialFill(smoothPotiValue, effect);
        fillStripWithColor(effect);
        break;
      case 2:
        fillStripWithColor(effect);
        rainbow(20, effect);
        break;
      default:
        //nothing
        break;
    }
    // execute fade
    fadeBetweenEffects();
  }
}
