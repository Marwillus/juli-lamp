#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>
#include <Smoothed.h>
#include <MultiButton.h>
#include "EasingLib.h"

#define POTENTIOMETER_POTI 0
#define HIGH_POWER_LED_PIN 5
#define LED_PIN 6
#define NUM_LEDS 48
#define STRIP_SPLIT_AT 16
#define FADE_DURATION_SLOW 500
#define FADE_DURATION_FAST 100

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);
CapacitiveSensor capaSensor = CapacitiveSensor(3, 2);
Smoothed<int> smooth;
MultiButton button;
Easing easeInSlow(ease_mode::EASE_IN_CUBIC, FADE_DURATION_SLOW);
Easing easeInOutFast(ease_mode::EASE_IN_CUBIC, FADE_DURATION_FAST);

//// VARIABLES
// time
unsigned long currentTime;          // Store current millis().
unsigned long previousTime;         // store last measured millis().
unsigned long pixelPrevious = 0;    // Previous Pixel Millis
unsigned long patternPrevious = 0;  // Previous Pattern Millis

// led
float brightnessLimiter = 0.2;
// mode
bool active = false;
bool activated = false;
int previousEffect = 0;
bool effectChange = false;
int effect = 0;
bool animationInProgress = true;

// btn & poti
int potiValue = 100;
float brightnessFactor = 0.2;
float clipBrightnessFactor;
float effectFactor = 1.0;
long capaValue = 0;
int longPressDelay = 800;
int oldPotiValue;
int maxPotiValue = 700;
int minPotiValue = 40;
int jitterThreshold = 2;  //reduces led flickering when power source is unstable

// effects
int pixelCycle = 0;
bool runEffectFade = false;
bool fadeRun = false;
int currentIntensity;
float intensityStep = 0.02;
uint32_t stripColors[NUM_LEDS][2];


//// SETUP /////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("Bord reload");
  strip.begin();
  strip.setBrightness(80);
  strip.show();
  capaSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
  smooth.begin(SMOOTHED_AVERAGE, 20);
}

//// FUNCTIONS /////////////////////////////////////////////////////////////////////////////////
void potiTick() {

  // read and map potentiometer
  int rawPotiValue = analogRead(POTENTIOMETER_POTI);

  // adjust max min value of poti
  if (rawPotiValue < minPotiValue) {
    minPotiValue = rawPotiValue;
  }
  if (rawPotiValue > maxPotiValue) {
    maxPotiValue = rawPotiValue;
  }

  potiValue = map(rawPotiValue, minPotiValue, maxPotiValue, 0, 1000);
  // smooth out the pin value
  smooth.add(potiValue);
  brightnessFactor = smooth.get() / 1000.0;
}

// checks button update
void buttonTick() {
  capaValue = capaSensor.capacitiveSensor(30);
  button.update(capaValue > 150);

  if (button.isClick()) {
      logger("click");
    pixelPrevious = currentTime;
    clipBrightnessFactor = brightnessFactor;
  }
  if (capaValue > 150) {
    if (currentTime - pixelPrevious >= longPressDelay) {
      effectFactor = brightnessFactor;
      brightnessFactor = clipBrightnessFactor;
    }
  }
  if (button.isSingleClick()) {
    Serial.println("on / off");
    activated = !activated;
    Serial.println(activated);
  }
  if (button.isDoubleClick()) {
    Serial.println("effect increase");
    effect++;
    logger(effect);
  }
}

// HELPER

// make a snapshot for fade transition
void fadeSnapshot(bool fadeStartEffect) {
  if (!runEffectFade) return;

  if (fadeStartEffect) {
    // TODO update snapshot logik
    for (int i = 0; i < NUM_LEDS; i++) {
      stripColors[i][0] = strip.getPixelColor(i);
    }
  } else {
    for (int i = 0; i < NUM_LEDS; i++) {
      stripColors[i][1] = strip.getPixelColor(i);
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  byte r, g, b;

  if (WheelPos < 85) {
    r = (255 - WheelPos * 3) * brightnessFactor;
    g = 0;
    b = (WheelPos * 3) * brightnessFactor;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    r = 0;
    g = (WheelPos * 3) * brightnessFactor;
    b = (255 - WheelPos * 3) * brightnessFactor;
  } else {
    WheelPos -= 170;
    r = (WheelPos * 3) * brightnessFactor;
    g = (255 - WheelPos * 3) * brightnessFactor;
    b = 0;
  }

  return strip.gamma32(strip.Color(r, g, b));
};

int minutes(int millis) {
  return 1000 * 60 * millis;
}

int keepWithinBounds(int input, int min = 0, int max = 255) {
  if (input > max) {
    return input = max;
  }
  if (input < min) {
    return input = min;
  }
  return input;
}

// logger functions
void logger(const char* message) {
  Serial.println(message);
}

void logger(bool value) {
  Serial.println(value ? "true" : "false");
}

void logger(byte value) {
  Serial.println(value);
}

void logger(int value) {
  Serial.println(value);
}

void logger(float value) {
  Serial.println(value, 2);  // Print float value with 2 decimal places
}

void logger(uint32_t value) {
  Serial.println(value);
}