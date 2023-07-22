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
#define STRIP_SPLIT_AT 17


Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);
CapacitiveSensor capaSensor = CapacitiveSensor(3, 2);
Smoothed<int> smooth;
MultiButton button;

//// VARIABLES
bool active = false;
bool activated = false;
unsigned long currentTime;          // Store current millis().
unsigned long previousTime;         // store last measured millis().
unsigned long pixelPrevious = 0;    // Previous Pixel Millis
unsigned long patternPrevious = 0;  // Previous Pattern Millis
int pixelInterval = 50;
float maxBrightness = 0.2;
int potiValue = 10;
int oldPotiValue;
bool oldBtnState = 1, newBtnState = 1;
int oldPrintedValue;
int selectedEffect;
long capaValue = 0;
int effect = 0;
int step = 0;
int maxPotiValue = 700;
int minPotiValue = 40;
int smoothPotiValue;
int jitterThreshold = 2;  //reduces led flickering when power source is unstable
int pixelCycle = 0;
// fade
bool fadeRunning = false;
int currentIntensity;
int intensityStep = 1;
unsigned long fadePrevious = 0;         // store last measured millis().

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

  // if (currentTime - previousTime > debounce) {
  //   if (buttonChanged(capaValue > 150)) {
  //     if (newBtnState == 1) {
  //       Serial.println("Button released ");
  //       effect++;
  //     } else {
  //       Serial.println("Button pressed ");
  //     };
  //   }

  //   previousTime = currentTime;
  // }
}

bool buttonChanged(int bttn) {
  bool changed = 0;
  newBtnState = (capaValue > 150);
  if (newBtnState != oldBtnState) {  // its changed
    changed = 1;
    oldBtnState = newBtnState;
  }
  return (changed);
}

void printStuff() {
    Serial.print("effect selected: ");
  Serial.println(effect);

  Serial.print("poti value: ");
  Serial.println(smoothPotiValue);
}

uint32_t Wheel(byte WheelPos) {
  float blur = 3;  // smaller = longer
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * blur, 0, WheelPos * blur);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * blur, 255 - WheelPos * blur);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * blur, 255 - WheelPos * blur, 0);
};