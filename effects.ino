//// EFFECTS /////////////////////////////////////////////////////////////////////////////////

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
  int initialBrightness = smoothPotiValue;

  ClockTimeSinceLastBounce = millis();
  Height = StartHeight;
  Position = 0;
  ImpactVelocity = ImpactVelocityStart;
  TimeSinceLastBounce = 0;

  // one ball jump
  while (step == 0) {
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
      step++;
    }
  }

  // fade to brightness
  while (step == 1) {
    currentTime = millis();

    int brightness = fadeFromTo(0, smoothPotiValue, 10);
    gradialFill(brightness);
    powerLed(brightness * maxBrightness);

    if (brightness >= initialBrightness) {
      step = 0;
      active = activated;
    }
  }
}

int fadeFromTo(int start, int target, unsigned long fadeInterval) {
  if (!fadeRunning) {
    currentIntensity = start;
    fadeRunning = true;
  }

  int diff = target - start;
  if (diff == 0) {
    return start;
  }

  if (diff > 0) {
    intensityStep = abs(intensityStep); // Ensure positive step
  } else {
    intensityStep = -abs(intensityStep); // Ensure negative step
  }

  if (currentTime - pixelPrevious >= fadeInterval) {
    currentIntensity += intensityStep;
    Serial.println(currentIntensity);

    // Ensure the intensity does not overshoot the target
    if ((intensityStep > 0 && currentIntensity > target) || (intensityStep < 0 && currentIntensity < target)) {
      currentIntensity = target;
    }
    pixelPrevious = currentTime;
  }
  if (currentIntensity == target) {
    fadeRunning = false;
  }

  return currentIntensity;
}

void gradialFill(int intensity) {
  int numled = 50;
  int curve;
  int reverseCurve;

  for (int i = 0; i < STRIP_SPLIT_AT; i++) {
    // reziprocal curve
    // curve = ((150 * ((float)intensity)) / (255 * (float)i / (float)NUM_LEDS) + 20) - 50;

    // linear curve
    reverseCurve = 2 * i + (intensity * 2 - 66);

    if (reverseCurve > 255) {
      reverseCurve = 255;
    }
    if (reverseCurve < 0) {
      reverseCurve = 0;
    }

    strip.setPixelColor(i, 0, 0, 0, reverseCurve);
  }

  for (int i = STRIP_SPLIT_AT; i < NUM_LEDS; i++) {
    curve = -1 * 2 * i + intensity * 2;

    if (curve > 255) {
      curve = 255;
    }
    if (curve < 0) {
      curve = 0;
    }

    strip.setPixelColor(i, 0, 0, 0, curve);
  }

  strip.show();
}

void fillStripWithColor(int color) {
  uint16_t hue = 65555 * color / 255;
  strip.fill((strip.ColorHSV(hue, 255, 255)));
  // fill_rainbow( strip, NUM_LEDS, 0, 7)
  strip.show();
}

void warmWhite(int warmth) {
  // uint16_t hue = 8000;
  // int saturation = warmth;
  // int value = 255;
  // strip.fill((strip.ColorHSV(hue, saturation, value)));

  strip.fill(strip.Color(warmth, warmth / 1.5, 0, 255));
  strip.show();
}

void powerLed(int brightness) {
  analogWrite(HIGH_POWER_LED_PIN, brightness);
}

void rainbow(uint8_t wait) {
  if (pixelInterval != wait)
    pixelInterval = wait;
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, Wheel((i + pixelCycle) & 255));  //  Update delay time
  }
  strip.show();  //  Update strip to match
  pixelCycle++;  //  Advance current cycle
  if (pixelCycle >= 256)
    pixelCycle = 0;  //  Loop the cycle back to the begining
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

