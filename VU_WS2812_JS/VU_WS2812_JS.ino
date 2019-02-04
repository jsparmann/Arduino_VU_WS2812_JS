// --------------------------------
// -- VU Meter - Jens's version --
// -- ------ Version 1.0 ------ --
// --------------------------------

#include <FastLED.h>
#include <EEPROM.h>
#include "Button.h"

# define LEFT_OUT_PIN 6         // Left channel data out pin to LEDs
# define RIGHT_OUT_PIN 5        // Right channel data out pin to LEDs
# define LEFT_IN_PIN A5         // Left aux in signal
# define RIGHT_IN_PIN A4        // Right aux in signal
# define BTN_PIN 3              // Push button on this pin
# define DEBOUNCE_MS 20         // Number of ms to debounce the button 20 is default
# define LONG_PRESS 500         // Number of ms to hold the button to count as long press
# define N_PIXELS 72            // Number of pixels in each string
# define COLOR_ORDER GRB        // Try mixing up the letters (RGB, GBR, BRG, etc) for a whole new world of color combinations
# define BRIGHTNESS 80         // 0-255, higher number is brighter.
# define LED_TYPE WS2812B       // Probably WS2812B
# define DC_OFFSET 0            // DC offset in aux signal - if unusure, leave 0
# define NOISE 10               // Noise/hum/interference in aux signal
# define SAMPLES 32             // Length of buffer for dynamic level adjustment
# define TOP (N_PIXELS + 2)     // Allow dot to go slightly off scale
# define PEAK_FALL 20           // Rate of peak falling dot
# define N_PIXELS_HALF (N_PIXELS / 2)
# define PATTERN_TIME 10         // Seconds to show eaach pattern on auto
# define TWO_STRIPS             // Comment out this line if only using one strip attached to the left channel

# define debug                  // Auskommentieren um die Debug-Konsole zu deaktivieren
#ifdef debug
const long interval = 1000;     // Zeit für Pegel ausgabe (ms)
unsigned long previousMillis = 0; // letzte Zeit für Debug Ausgabe
#endif

 
 # define auto_helligkeit       // Funktion für Helligkeitsanpassung
 #ifdef auto_helligkeit
  # define helligkeit_pin A3     // Pin für Auto-Helligkeit
  const long interval_hell = 5000; // Zeit für Helligkeitsanpassung (ms)
  unsigned long prepreviousMillis = 0; // letzte Zeit für helligkeitsprüfung
  uint8_t prebright=0;           // Variable für Helligkeit
  uint8_t bright;                // Variable für Helligkeit
  #endif


uint8_t volCountLeft = 0;       // Frame counter for storing past volume data
int volLeft[SAMPLES];           // Collection of prior volume samples
int lvlLeft = 10;               // Current "dampened" audio level
int minLvlAvgLeft = 0;          // For dynamic adjustment of graph low & high
int maxLvlAvgLeft = 512;

uint8_t volCountRight = 0;      // Frame counter for storing past volume data
int volRight[SAMPLES];          // Collection of prior volume samples
int lvlRight = 10;              // Current "dampened" audio level
int minLvlAvgRight = 0;         // For dynamic adjustment of graph low & high
int maxLvlAvgRight = 512;

CRGB ledsLeft[N_PIXELS];
#ifdef TWO_STRIPS
  CRGB ledsRight[N_PIXELS];
#endif

uint8_t myhue = 0;

void vu4(bool is_centered, uint8_t channel);
void vu5(bool is_centered, uint8_t channel);
void vu6(bool is_centered, uint8_t channel);
void vu7(bool show_background);
void vu8();
void vu9();
void vu10();
void fire();
void sinelon();

// --------------------
// --- Button Stuff ---
// --------------------
uint8_t state = 0;
int buttonPushCounter = 0;
bool autoChangeVisuals = false;
Button modeBtn(BTN_PIN, DEBOUNCE_MS);


void incrementButtonPushCounter() {
  buttonPushCounter = ++buttonPushCounter %17;
  EEPROM.write(0, buttonPushCounter);
}

void setup() {
  delay(1000); // power-up safety delay

  FastLED.addLeds < LED_TYPE, LEFT_OUT_PIN, COLOR_ORDER > (ledsLeft, N_PIXELS).setCorrection(TypicalLEDStrip);
  #ifdef TWO_STRIPS
    FastLED.addLeds < LED_TYPE, RIGHT_OUT_PIN, COLOR_ORDER > (ledsRight, N_PIXELS).setCorrection(TypicalLEDStrip);
  #endif
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(57600);

  buttonPushCounter = (int)EEPROM.read(0); // load previous setting
  Serial.print("Starting pattern ");
  Serial.println(buttonPushCounter);
}

void loop() {
 unsigned long currentMillis = millis();
 // Debug Konsole
    #ifdef debug
      if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          Serial.print("Pegel Links: ");
          Serial.println(analogRead(LEFT_IN_PIN));
          Serial.print("Pegel Rechts: ");
          Serial.println(analogRead(RIGHT_IN_PIN));
      }
    #endif
    
 //Helligkeitsregelung
     #ifdef auto_helligkeit
        if (currentMillis - prepreviousMillis >= interval_hell) {
          prepreviousMillis = currentMillis;
          helligkeit();
        }
    #endif

  // Read button
  modeBtn.read(); 
  switch (state) {
    case 0:                
      if (modeBtn.wasReleased()) {
        Serial.print("Short press, pattern ");
        Serial.println(buttonPushCounter);
        incrementButtonPushCounter();
        autoChangeVisuals = false;
      }
      else if (modeBtn.pressedFor(LONG_PRESS))
        state = 1;
      break;
    
    case 1:
      if (modeBtn.wasReleased()) {
        state = 0;
        Serial.print("Long press, auto, pattern ");
        Serial.println(buttonPushCounter);
        autoChangeVisuals = true;
      }
      break;
  }
  
  // Switch pattern if on auto
  if(autoChangeVisuals){
    EVERY_N_SECONDS(PATTERN_TIME) { 
      incrementButtonPushCounter();
      Serial.print("Auto, pattern ");
      Serial.println(buttonPushCounter); 
    }
  }

  // Run selected pattern
  switch (buttonPushCounter) {
    case 0:
      vu4(false, 0);
      #ifdef TWO_STRIPS
        vu4(false, 1);
      #endif
      break;
  
    case 1:
      vu4(true, 0);
      #ifdef TWO_STRIPS
        vu4(true, 1);
      #endif
      break;
  
    case 2:
      vu5(false, 0);
      #ifdef TWO_STRIPS
        vu5(false, 1);
      #endif
      break;
  
    case 3:
      vu5(true, 0);
      #ifdef TWO_STRIPS
        vu5(true, 1);
      #endif
      break;
  
    case 4:
      vu6(false, 0);
      #ifdef TWO_STRIPS
        vu6(false, 1);
      #endif
      break;
      
    case 5:
      vu7(true);
      break;
  
    case 6:
      vu8();
      break;
  
    case 7:
      vu9();
      break;
  
    case 8:
      vu10();
      break;
    
    case 9:
      vu7(false);
      break;
  
    case 10:
      fire();
      break;
  
    case 11:
      sinelon();
      break;
  }
}

// ------------------
// -- VU functions --
// ------------------

uint16_t auxReading(uint8_t channel) {

  int n = 0;
  uint16_t height = 0;

  if(channel == 0) {
    int n = analogRead(LEFT_IN_PIN); // Raw reading from left line in
    n = abs(n - 512 - DC_OFFSET); // Center on zero
    n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
    lvlLeft = ((lvlLeft * 7) + n) >> 3; // "Dampened" reading (else looks twitchy)
    volLeft[volCountLeft] = n; // Save sample for dynamic leveling
    volCountLeft = ++volCountLeft % SAMPLES;
    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvlLeft - minLvlAvgLeft) / (long)(maxLvlAvgLeft - minLvlAvgLeft);
  }
  
  else {
    int n = analogRead(RIGHT_IN_PIN); // Raw reading from mic
    n = abs(n - 512 - DC_OFFSET); // Center on zero
    n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
    lvlRight = ((lvlRight * 7) + n) >> 3; // "Dampened" reading (else looks twitchy)
    volRight[volCountRight] = n; // Save sample for dynamic leveling
    volCountRight = ++volCountRight % SAMPLES;
    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvlRight - minLvlAvgRight) / (long)(maxLvlAvgRight - minLvlAvgRight);
  }

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = constrain(height, 0, TOP);

  return height;
}

/*
 * Function for dropping the peak
 */
uint8_t peakLeft, peakRight;
void dropPeak(uint8_t channel) {
  
  static uint8_t dotCountLeft, dotCountRight;
 
  if(channel == 0) {
    if(++dotCountLeft >= PEAK_FALL) { //fall rate 
      if(peakLeft > 0) peakLeft--;
      dotCountLeft = 0;
    }
  } else {
    if(++dotCountRight >= PEAK_FALL) { //fall rate 
      if(peakRight > 0) peakRight--;
      dotCountRight = 0;
    }
  }
}

/*
 * Function for averaging the sample readings
 */
void averageReadings(uint8_t channel) {

  uint16_t minLvl, maxLvl;

  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if(channel == 0) {
    minLvl = maxLvl = volLeft[0];
    for (int i = 1; i < SAMPLES; i++) {
      if (volLeft[i] < minLvl) minLvl = volLeft[i];
      else if (volLeft[i] > maxLvl) maxLvl = volLeft[i];
    }
    if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
    
    minLvlAvgLeft = (minLvlAvgLeft * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvgLeft = (maxLvlAvgLeft * 63 + maxLvl) >> 6; // (fake rolling average)
  }

  else {
    minLvl = maxLvl = volRight[0];
    for (int i = 1; i < SAMPLES; i++) {
      if (volRight[i] < minLvl) minLvl = volRight[i];
      else if (volRight[i] > maxLvl) maxLvl = volRight[i];
    }
    if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
    minLvlAvgRight = (minLvlAvgLeft * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvgRight = (maxLvlAvgLeft * 63 + maxLvl) >> 6; // (fake rolling average)
  }
}
 
 void helligkeit() {
  bright=map(analogRead(helligkeit_pin),0,1023,0,255);
 
  #ifdef debug
  Serial.print("Helligkeitsregelung_INFUNCTION_bright: ");
  Serial.println(bright);
  #endif
    
  if (abs(bright - prebright) >= 10) {
    prebright = bright;
    FastLED.setBrightness(bright);
    
    #ifdef debug
    Serial.print("BRIGHTNESS correction: ");
    Serial.println(bright); 
    #endif
  }
 }
