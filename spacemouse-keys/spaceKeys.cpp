// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>
// check config.h if this functions and variables are needed
#if NUMKEYS > 0

// array with the pin definition of all keys
int keyList[NUMKEYS] = KEYLIST;

//Needed for key evaluation
int8_t key_waspressed[NUMKEYS];
unsigned long timestamp[NUMKEYS];

// Function to setup up all keys in keyList
void setupKeys() {
  for (int i = 0; i < NUMKEYS; i++) {
    pinMode(keyList[i], INPUT_PULLUP);
  }
}


// Function to read and store the digital states for each of the keys
void readAllFromKeys(int* keyVals) {

  for (int i = 0; i < NUMKEYS; i++) {
    keyVals[i] = digitalRead(keyList[i]);
  }

}

// Evaluate and debounce all keys from the raw keyVals into the debounced keyOut.
// The keyOut is only 1 for one iteration of the loop.
// If you want to know, if the key is pressed all the time (for the kill-key feature), use the not debounced raw keyVal!
// returns true if any key was pressed
bool evalKeys(int* keyVals, uint8_t* keyOut) {
  bool anyKeyPressed = 0;

  //Button Evaluation
  for (int i = 0; i < NUMKEYS; i++) {
    // The keys are configured with pull_up, see setupKeys() and are pulled to ground, when pressed.
    // Therefore, the pressed key is false, which is an inverted logic
    if (!keyVals[i]) {  // the key is pressed
      // Making sure button cannot trigger multiple times which would result in overloading HID.
      if (key_waspressed[i] == 0) {  // if the button has not been pressed lately:
        keyOut[i] = 1; 
        anyKeyPressed = 1;              // this is the variable telling the outside world only one iteration, that the key was pressed
        key_waspressed[i] = 1;       // remember, that we already told the outside world about this key
        timestamp[i] = millis();     // remember the time, the button was pressed
        Serial.print("Key: ");       // this is always sent over the serial console, and not only in debug
        Serial.println(i);
      } else {  // the button was already pressed and is still pressed (and the event sent in the last loop), don't send the keyOut event again.
        keyOut[i] = 0;
      }
    } else {                         // the button is not pressed
      if (key_waspressed[i] == 1) {  // has it been pressed lately?
        // debouncing:
        if (millis() - timestamp[i] > DEBOUNCE_KEYS_MS) {  // check if the last button press is long enough in the past
          key_waspressed[i] = 0;                           // reset this marker and allow a new button press
        }
      }
    }
  }
  return anyKeyPressed;
}
#endif