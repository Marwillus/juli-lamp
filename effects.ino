//// ANIMATIONS /////////////////////////////////////////////////////////////////////////////////

// bounces one pixel from 0 to split led
void bouncingIntro() {
  float gravity = -9.81;
  int StartHeight = 1;

  float Height;
  float ImpactVelocityStart = sqrt(-2 * gravity * StartHeight);
  float ImpactVelocity;
  float TimeSinceLastBounce;
  long ClockTimeSinceLastBounce;
  int Position;
  bool reachedPeak = false;

  ClockTimeSinceLastBounce = millis();
  Height = StartHeight;
  Position = 0;
  ImpactVelocity = ImpactVelocityStart;
  TimeSinceLastBounce = 0;

  // one ball jump
  while (animationInProgress) {
    TimeSinceLastBounce = millis() - ClockTimeSinceLastBounce;
    Height = 0.5 * gravity * pow(TimeSinceLastBounce / 1000, 2.0) + ImpactVelocity * TimeSinceLastBounce / 1000;

    // set position of led
    Position = round(Height * (NUM_LEDS - 1) / StartHeight);

    strip.setPixelColor(Position, 0, 0, 0, 200);

    strip.show();
    strip.clear();

    if (Position == (NUM_LEDS - 1)) {
      reachedPeak = true;
    }
    if (reachedPeak && Position == STRIP_SPLIT_AT) {
      reachedPeak = false;
      animationInProgress = false;
      break;
    }
  }
}
// bounces one pixel from split led to 0
// bool bouncingOutro() {

//   if (currentTime - pixelPrevious >= pixelInterval) {
//     int paintCurrentPixel = (STRIP_SPLIT_AT - pixelCycle);
//     strip.setPixelColor(paintCurrentPixel, 0, 0, 0, 200);
//     strip.setPixelColor(paintCurrentPixel + 1, 0, 0, 0, 0);
//     strip.show();

//     if (pixelCycle < STRIP_SPLIT_AT) {
//       pixelCycle++;
//       pixelInterval--;
//     } else {
//       pixelInterval = 30;
//       pixelCycle = 0;
//       strip.clear();
//       strip.show();
//       return true;
//     }

//     pixelPrevious = currentTime;
//   }

//   return false;
// }

void bouncingOutro() {
  unsigned long startTime = millis();  // Get the current time in milliseconds
  float fromTo = 1;
  int splitLed = STRIP_SPLIT_AT;

  while (millis() - startTime <= FADE_DURATION) {
    logger(easeInSlow.SetSetpoint(fromTo));
    int pixel = splitLed * easeInSlow.SetSetpoint(fromTo);
    //revert pixel
    pixel = pixel + (pixel - splitLed);
    logger(pixel);
    strip.setPixelColor(pixel + 1, 0, 0, 0, 0);
    strip.setPixelColor(pixel, 0, 0, 0, 200);
    strip.show();
  }
}

// return a value for one pixel
float fadeFromTo(float start, float target, unsigned long fadeInterval) {
  // set startValue
  if (!fadeRun) {
    currentIntensity = start;
    fadeRun = true;
  }

  int diff = target - start;
  if (diff == 0) {
    return start;
  }

  if (diff > 0) {
    intensityStep = abs(intensityStep);  // Ensure positive step
  } else {
    intensityStep = -abs(intensityStep);  // Ensure negative step
  }

  if (currentTime - pixelPrevious >= fadeInterval) {
    currentIntensity += intensityStep;

    // Ensure the intensity does not overshoot the target
    if ((intensityStep > 0 && currentIntensity > target) || (intensityStep < 0 && currentIntensity < target)) {
      currentIntensity = target;
    }
    pixelPrevious = currentTime;
  }
  if (currentIntensity == target) {
    fadeRun = false;
  }

  return currentIntensity;
}

// fades powerled and strip to 0
void fadeAllOut() {
  float initialBrightness = brightnessFactor;
  unsigned long startTime = millis();  // Get the current time in milliseconds
  float fromTo = initialBrightness;

  while (millis() - startTime <= FADE_DURATION) {
    brightnessFactor = easeInSlow.SetSetpoint(fromTo);
    fromTo = 0;
    runMode();
    strip.show();
    powerLed();
  }
  active = activated;
}

// TODO repair ease out
// fades powerled and strip to selected brightness
void fadeAllIn() {
  float initialBrightness = brightnessFactor;
  unsigned long startTime = millis();  // Get the current time in milliseconds
  float fromTo = 0;
  logger(initialBrightness);

  while (millis() - startTime <= FADE_DURATION) {
    brightnessFactor = easeInSlow.SetSetpoint(fromTo);
    fromTo = initialBrightness;
    runMode();
    strip.show();
    powerLed();
  }
  active = activated;
}

// TODO: BUG -> flickering at beginnig on animation
// Function to fade between effects
void fadeBetweenEffects() {
  // int fadeInterval = 10;
  unsigned long startTime = millis();
  float progress = 0;
  bool once = true;

  while (progress < 1.0) {
    currentTime = millis();
    progress = min(1.0, (float)(currentTime - startTime) / FADE_DURATION);

    if (progress < 0.05) {
      continue;  // Skip updating colors to prevent flickering
    }

    // Update the LED strip for each pixel based on the fade progress
    for (uint16_t i = 0; i < NUM_LEDS; i++) {

      uint32_t startColor = stripColors[i][0];
      uint32_t endColor = stripColors[i][1];
      uint32_t currentColor;
      // logger(endColor);
      // float easedProgress = easeInOutQuad(progress);

      // Calculate the blended color value for pixel i between the start and end colors
      uint8_t startW = (startColor >> 24) & 0xFF;
      uint8_t startR = (startColor >> 16) & 0xFF;
      uint8_t startG = (startColor >> 8) & 0xFF;
      uint8_t startB = startColor & 0xFF;

      uint8_t endW = (endColor >> 24) & 0xFF;
      uint8_t endR = (endColor >> 16) & 0xFF;
      uint8_t endG = (endColor >> 8) & 0xFF;
      uint8_t endB = endColor & 0xFF;


      uint8_t currentW = startW + (endW - startW) * progress;
      uint8_t currentR = startR + (endR - startR) * progress;
      uint8_t currentG = startG + (endG - startG) * progress;
      uint8_t currentB = startB + (endB - startB) * progress;

      currentColor = strip.Color(currentR, currentG, currentB, currentW);

      // if (i == 0) {
      //   logger(progress);
      //   logger(currentColor);
      // logger(currentW);
      // logger(currentR);
      // logger(currentG);
      // logger(currentB);
      // }
      strip.setPixelColor(i, currentColor);
      strip.show();
    }
  }
  logger("end fade");
  runEffectFade = false;
}

//// EFFECTS /////////////////////////////////////////////////////////////////////////////////

void powerLed() {
  analogWrite(HIGH_POWER_LED_PIN, 255 * brightnessFactor * brightnessLimiter);
}


void gradialFill() {
  int numled = 50;
  int curve;
  int reverseCurve;

  for (int i = 0; i < STRIP_SPLIT_AT; i++) {
    // reziprocal curve
    // curve = ((150 * ((float)intensity)) / (255 * (float)i / (float)NUM_LEDS) + 20) - 50;

    // linear curve
    reverseCurve = 2 * i + (255 * brightnessFactor * 2 - 66);
    reverseCurve = keepWithinBounds(reverseCurve);

    strip.setPixelColor(i, strip.Color(0, 0, 0, reverseCurve));
  }

  for (int i = STRIP_SPLIT_AT; i < NUM_LEDS; i++) {
    curve = -1 * 2 * i + 255 * brightnessFactor * 2;
    curve = keepWithinBounds(curve);

    strip.setPixelColor(i, strip.Color(0, 0, 0, curve));
  }
}

void gradialColorFill() {
  uint16_t hue = 65555 * brightnessFactor;

  for (int i = 0; i < NUM_LEDS; i++) {

    int intensity = map(i, 0, NUM_LEDS - 1, 255 * brightnessFactor, 0);
    uint8_t white = map(i, 0, NUM_LEDS - 1, 0, 255 * brightnessFactor);

    uint32_t rgbColor = strip.ColorHSV(hue, 255, intensity);
    uint8_t red = (rgbColor >> 16) & 0xFF;
    uint8_t green = (rgbColor >> 8) & 0xFF;
    uint8_t blue = rgbColor & 0xFF;

    strip.setPixelColor(i, strip.Color(red, green, blue, white));
  }
  strip.show();
}

void fillStripWithColor() {
  uint16_t hue = 65555 * brightnessFactor;

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255 * brightnessFactor));
  }
}

void rainbow() {

  if (currentTime - pixelPrevious >= pixelInterval * effectFactor) {

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      uint32_t pixelColor = Wheel((i + pixelCycle) & 255);
      strip.setPixelColor(i, pixelColor);  //  Update delay time
    }
    pixelCycle++;  //  Advance current cycle
    if (pixelCycle >= 256)
      pixelCycle = 0;  //  Loop the cycle back to the begining

    pixelPrevious = currentTime;
  }
}

void bouncingBalls(byte red, byte green, byte blue, byte white, int BallCount) {
  float gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt(-2 * gravity * StartHeight);
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int Position[BallCount];
  long ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0; i < BallCount; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
  }

  while (true) {
    for (int i = 0; i < BallCount; i++) {
      TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * gravity * pow(TimeSinceLastBounce[i] / 1000, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if (Height[i] < 0) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if (ImpactVelocity[i] < 0.01) {

          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round(Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    for (int i = 0; i < BallCount; i++) {
      strip.setPixelColor(Position[i], red, green, blue, white);
    }

    strip.show();
    strip.fill(0, 0, 0);
  }
}
