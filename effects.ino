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
  animationInProgress = true;

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
void bouncingOutro() {
  unsigned long startTime = millis();
  easeInSlow.Init(1);
  easeInOutFast.Init(brightnessFactor);

  while (millis() - startTime <= FADE_DURATION_SLOW) {
    brightnessFactor = easeInOutFast.SetSetpoint(0);
    powerLed();

    int pixel = STRIP_SPLIT_AT * easeInSlow.SetSetpoint(0);
    // strip.setPixelColor(pixel, 0, 0, 0, 0);
    strip.setPixelColor(pixel - 1, 0, 0, 0, 200);
    strip.show();
    strip.clear();
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

// let powerled blink
void smoothBlink() {
  unsigned long startTime = millis();
  easeInOutFast.Init(0);
  while (millis() - startTime <= FADE_DURATION_FAST) {
    brightnessFactor = easeInOutFast.SetSetpoint(0.5);
    powerLed();
  }
  easeInOutFast.Init(0.5);
  startTime = millis();
  while (millis() - startTime <= FADE_DURATION_FAST) {
    brightnessFactor = easeInOutFast.SetSetpoint(0);
    powerLed();
  }
}

// TODO repair ease out
// fades powerled and strip to selected brightness
void fadeAllIn() {
  float initialBrightness = brightnessFactor;
  unsigned long startTime = millis();
  easeInSlow.Init(0.1);

  while (millis() - startTime <= FADE_DURATION_SLOW) {
    easeInSlow.SetSetpoint(initialBrightness);
    brightnessFactor = easeInSlow.GetValue();
    runMode();
    strip.show();
    powerLed();
  }
}

// fades powerled and strip to 0
void fadeAllOut() {
  float initialBrightness = brightnessFactor;
  unsigned long startTime = millis();
  easeInSlow.Init(initialBrightness);

  while (millis() - startTime <= FADE_DURATION_SLOW) {
    brightnessFactor = easeInSlow.SetSetpoint(0.1);
    runMode();
    strip.show();
    powerLed();
  }
}

// TODO: BUG -> flickering at beginnig on animation
// Function to fade between effects
void fadeBetweenEffects() {
  // int fadeInterval = 10;
  unsigned long startTime = millis();
  float progress = 0;
  bool once = true;

  uint32_t startColor;
  uint32_t endColor;
  uint32_t currentColor;

  // Calculate the blended color value for pixel i between the start and end colors
  uint8_t startW;
  uint8_t startR;
  uint8_t startG;
  uint8_t startB;

  uint8_t endW;
  uint8_t endR;
  uint8_t endG;
  uint8_t endB;

  while (progress < 1.0) {
    currentTime = millis();
    progress = min(1.0, (float)(currentTime - startTime) / FADE_DURATION_SLOW);

    // Update the LED strip for each pixel based on the fade progress
    for (uint16_t i = 0; i < NUM_LEDS; i++) {

      startColor = stripColors[i][0];
      endColor = stripColors[i][1];
      currentColor;

      // Calculate the blended color value for pixel i between the start and end colors
      startW = (startColor >> 24) & 0xFF;
      startR = (startColor >> 16) & 0xFF;
      startG = (startColor >> 8) & 0xFF;
      startB = startColor & 0xFF;

      endW = (endColor >> 24) & 0xFF;
      endR = (endColor >> 16) & 0xFF;
      endG = (endColor >> 8) & 0xFF;
      endB = endColor & 0xFF;

      uint8_t currentW = startW + (endW - startW) * progress;
      uint8_t currentR = startR + (endR - startR) * progress;
      uint8_t currentG = startG + (endG - startG) * progress;
      uint8_t currentB = startB + (endB - startB) * progress;

      currentColor = strip.Color(currentR, currentG, currentB, currentW);

      strip.setPixelColor(i, currentColor);
      if (progress > 0.1) {
        strip.show();  // Skip updating colors to prevent flickering
      }
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
  uint16_t hue = 65555 * effectFactor;

  for (int i = 0; i < NUM_LEDS; i++) {

    int intensity = map(i, 0, NUM_LEDS - 1, 255, 0);
    uint8_t white = map(i, 0, NUM_LEDS - 1, 0, 255);

    uint32_t rgbColor = strip.gamma32(strip.ColorHSV(hue, 255, intensity * brightnessFactor));
    uint8_t red = (rgbColor >> 16) & 0xFF;
    uint8_t green = (rgbColor >> 8) & 0xFF;
    uint8_t blue = rgbColor & 0xFF;

    strip.setPixelColor(i, strip.Color(
                             red,
                             green,
                             blue,
                             white * brightnessFactor));
  }
  strip.show();
}

void fillStripWithColor() {
  uint16_t hue = 65555 * effectFactor;

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255 * brightnessFactor));
  }
}

void rainbow() {

  if (currentTime - pixelPrevious >= 100 * effectFactor) {

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
