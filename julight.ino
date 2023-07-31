#include <Arduino.h>
#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>
#include <Smoothed.h>
#include <MultiButton.h>

#define POTENTIOMETER_POTI 0
#define HIGH_POWER_LED_PIN 5
#define LED_PIN 6
#define NUM_LEDS 48
#define STRIP_SPLIT_AT 16
#define FADE_DURATION 800

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);
CapacitiveSensor capaSensor = CapacitiveSensor(3, 2);
Smoothed<int> smooth;
MultiButton button;

//// VARIABLES
// time
unsigned long currentTime;          // Store current millis().
unsigned long previousTime;         // store last measured millis().
unsigned long pixelPrevious = 0;    // Previous Pixel Millis
unsigned long patternPrevious = 0;  // Previous Pattern Millis
int pixelInterval = 20;

// led
float brightnessFactor = 0.2;
float brightnessLimiter = 0.2;
// mode
bool active = false;
bool activated = false;
int oldPrintedValue;
int previousEffect = 0;
bool effectChange = false;
int effect = 0;
int maxEffects = 4;
int step = 0;

// btn & poti
int potiValue = 10;
int secondaryPoti = 0;
int oldPotiValue;
long capaValue = 0;
bool oldBtnState = 1, newBtnState = 1;
int maxPotiValue = 700;
int minPotiValue = 40;
int smoothPotiValue;
int jitterThreshold = 2;  //reduces led flickering when power source is unstable

//rainbow
int pixelCycle = 0;
// fade
bool runEffectFade = false;
bool fadeRun = false;
int currentIntensity;
int intensityStep = 1;
uint32_t stripSnapshot[NUM_LEDS - 1];
uint32_t startColors[NUM_LEDS - 1];
uint32_t endColors[NUM_LEDS - 1];
uint32_t stripColors[NUM_LEDS][2];
bool animationInProgress = true;

  // declare functions for optional parameter
  void
  gradialFill(int brightness, bool fadeStart = true);
void gradialColorFill(int brightness = 255);
void fillStripWithColor(int brightness, bool fadeStart = true);
void rainbow(int brightness, bool fadeStart = true);
void powerLed(int brightness = 255);

//// SETUP /////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("Bord reload");
  strip.begin();
  strip.setBrightness(80);
  strip.show();
  capaSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
  smooth.begin(SMOOTHED_AVERAGE, 20);

  oldBtnState = (capaValue > 150);
  newBtnState = oldBtnState;
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

  // adjust the range to 0-255
  potiValue = map(rawPotiValue, minPotiValue, maxPotiValue, 0, 255);
  brightnessFactor = map(rawPotiValue, minPotiValue, maxPotiValue, 0, 1000) / 1000.0;
  if (potiValue < 0) {
    potiValue = 0;
  }
  if (potiValue > 255) {
    potiValue = 255;
  }
  // smooth out the pin value
  smooth.add(potiValue);
  smoothPotiValue = smooth.get();
}

void buttonTick() {
  capaValue = capaSensor.capacitiveSensor(30);
  button.update(capaValue > 150);

  // while button is pressed add alternative poti mode
  if (capaValue > 150) {
    secondaryPoti = smoothPotiValue;
  }
  if (button.isSingleClick()) {
    Serial.println("on / off");
    activated = !activated;
    Serial.println(activated);
  }
  if (button.isDoubleClick()) {
    Serial.println("effect increase");
    effect++;
    if (effect >= maxEffects) effect = 0;
  }
  if (button.isLongClick()) {
    Serial.println("long click");
    // alternative poti mode
  }
}

// helper

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

  if (WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3) * brightnessFactor, 0, (WheelPos * 3) * brightnessFactor);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (WheelPos * 3) * brightnessFactor, 255 - (WheelPos * 3) * brightnessFactor);
  }
  WheelPos -= 170;
  return strip.Color((WheelPos * 3) * brightnessFactor, (255 - WheelPos * 3) * brightnessFactor, 0);
};

int minutes(int millis) {
  return 1000 * 60 * millis;
}

int keepWithinBounds(int input, int min = 0, int max = 255) {
  if (input > 255) {
    return input = 255;
  }
  if (input < 0) {
    return input = 0;
  }
  return input;
}

float easeInOutQuad(float t) {
  return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
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