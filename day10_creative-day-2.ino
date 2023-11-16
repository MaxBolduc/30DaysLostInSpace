/*
 * 30 Days - Lost in Space
 * Day 10 - Some luxury
 *
 */
// Explicitly include Arduino.h
#include "Arduino.h"

// Our photoresistor will give us a reading of the current light level on this analog pin
const byte PHOTORESISTOR_PIN = A0;  // Photoresistor analog pin

// RGB LED pins
const byte RED_PIN = 11;    // pin controlling the red leg of our RGB LED
const byte GREEN_PIN = 10;  // pin ccontrolling the green leg of our RGB LED
const byte BLUE_PIN = 9;    // pin ccontrolling the blue leg of our RGB LED

//SWITCH Pin
const byte CHARGE_BATTERY_SWITCH_PIN = 2;
const byte TV_SWITCH_PIN = 3;

//TV Pin
const byte TV_PIN = 7;
const byte TV_CONSUMPTION_RATE = 250;

const unsigned long BATTERY_CAPACITY = 50000;  // Maximum battery capacity

/*
 * Display a color on our RGB LED by providing an intensity for
 * our red, green and blue LEDs.
 */
void displayColor(
  byte red_intensity,    // red LED intensity (0-255)
  byte green_intensity,  // green LED intensity (0-255)
  byte blue_intensity    // blue LED intensity (0-255)
) {
  analogWrite(RED_PIN, red_intensity);      // write red LED intensity using PWM
  analogWrite(GREEN_PIN, green_intensity);  // write green LED intensity using PWM
  analogWrite(BLUE_PIN, blue_intensity);    // write blue LED intensity using PWM
}

void displayBatteryLevel(float percentage) {
  if (percentage == 100.0)
    displayColor(128, 128, 128);
  else if (percentage >= 50.0) {  // battery level is OK, display green
    displayColor(0, 128, 0);      // display green
  } else if (percentage >= 25.0 && percentage < 50.0) {
    displayColor(128, 80, 0);  // display yellow-ish/amber for early warning
  } else {                     // Level must be less than 25%, display "pulsating" red
    // To pulsate the red light we briefly turn the LED off and then display red, giving it
    // a pulsating effect.
    displayColor(0, 0, 0);    // Turn off our LED
    delay(20);                // ...and delay briefly
    displayColor(128, 0, 0);  // then display red
  }

  Serial.print(percentage);  // Display our floating point percentage (like 12.34) WITHOUT a newline
  Serial.println("%");       // then display the percent sign ("%") with a newline.
}


void setup() {
  // Switch setup
  pinMode(CHARGE_BATTERY_SWITCH_PIN, INPUT);
  pinMode(TV_SWITCH_PIN, INPUT);

  //TV
  pinMode(TV_PIN, OUTPUT);

  pinMode(PHOTORESISTOR_PIN, INPUT);

  // Declare the RGB LED pins as outputs:
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Start serial monitor
  Serial.begin(9600);
}

long battery_level = 0;  // Current battery charge level (set to 0 first time used)

void loop() {

  //Are the solar panel extended?
  if (digitalRead(CHARGE_BATTERY_SWITCH_PIN) == HIGH) {
    Serial.print("Extending solar array...");
    battery_level += analogRead(PHOTORESISTOR_PIN);  // Add current "charge amount" to our battery
                                                     // We can't charge the battery higher than it's capacity, set level as full if it goes over
    if (battery_level > BATTERY_CAPACITY) {
      battery_level = BATTERY_CAPACITY;
    }
  } else {
    Serial.print("Retracting solar array...");
  }

  //Is the TV on?
  //Turn on only if there is enough charge left
  if (digitalRead(TV_SWITCH_PIN) == HIGH && battery_level >= TV_CONSUMPTION_RATE) {
    // turn LED ON
    battery_level -= TV_CONSUMPTION_RATE;
    if (battery_level < 0) {  //Cannot go lower than empty
      battery_level = 0;
    }
    Serial.print("Turning TV on...");
    digitalWrite(TV_PIN, HIGH);
  } else {
    Serial.print("Turning TV off...");
    digitalWrite(TV_PIN, LOW);  // turn LED OFF
  }

  // Compute battery charge percentage from our function
  float percentage = ((float)battery_level / (float)BATTERY_CAPACITY) * 100;
  displayBatteryLevel(percentage);

  delay(200);  // Delay 1/10 of a second so displayed values don't scroll too fast
}
