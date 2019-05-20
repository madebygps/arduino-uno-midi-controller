#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Your buttons
const int numberOfButtons = 4; // Change for number of buttons on your controller.
const int buttonPin[numberOfButtons] = {2, 3, 4, 5};     // The numbers where your buttons are plugged into on your arudino board.
int buttonCurrentState[numberOfButtons] = {0};         // stores the button current value
int buttonPreviousState[numberOfButtons] = {0};        // stores the button previous value


// Dealing with debounce.
unsigned long lastDebounceTime[numberOfButtons] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //* the debounce time; increase if the output flickers

// Your potentiometers.
const int numberOfPots = 2; //*
int potPin[numberOfPots] = {A0, A1}; //* Pin where the potentiometer is
int potCurrentState[numberOfPots] = {0}; // Current state of the pot
int potPreviousState[numberOfPots] = {0}; // Previous state of the pot
int potVar = 0; // Difference between the current and previous state of the pot

int midiCurrentState[numberOfPots] = {0}; // Current state of the midi value
int midiPreviousState[numberOfPots] = {0}; // Previous state of the midi value

int TIMEOUT = 300; //* Amount of time the potentiometer will be read after it exceeds the varThreshold
int varThreshold = 6; //* Threshold for the potentiometer signal variation
boolean potMoving = true; // If the potentiometer is moving
unsigned long PTime[numberOfPots] = {0}; // Previously stored time
unsigned long timer[numberOfPots] = {0}; // Stores the time that has elapsed since the timer was reset

// Midi information
byte midiCh = 1; //* MIDI channel to be used
byte note = 36; //* Lowest note to be used
byte cc = 1; //* Lowest MIDI CC to be used

void setup() {

  Serial.begin(115200);

    for (int i = 0; i < numberOfButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

void loop() {
  buttons();
  potentiometers();
}

// BUTTONS
void buttons() {

    for (int i = 0; i < numberOfButtons; i++) {

    buttonCurrentState[i] = digitalRead(buttonPin[i]);

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPreviousState[i] != buttonCurrentState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCurrentState[i] == LOW) {
          MIDI.sendNoteOff(note + i, 127, midiCh);
        }
        else {
          MIDI.sendNoteOn(note + i, 127, midiCh);
        }
        buttonPreviousState[i] = buttonCurrentState[i];
      }
    }
  }
}

// POTENTIOMETERS
void potentiometers() {
  
    for (int i = 0; i < numberOfPots; i++) { // Loops through all the potentiometers
    
    potCurrentState[i] = analogRead(potPin[i]); // Reads the pot and stores it in the potCurrentState variable
    midiCurrentState[i] = map(potCurrentState[i], 0, 1023, 0, 127); // Maps the reading of the potCurrentState to a value usable in midi

    potVar = abs(potCurrentState[i] - potPreviousState[i]); // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) { // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis(); // Stores the previous time
    }

    timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) { // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) { // If the potentiometer is still moving, send the change control
      if (midiPreviousState[i] != midiCurrentState[i]) {
        MIDI.sendControlChange(cc+i, midiCurrentState[i], midiCh);
        potPreviousState[i] = potCurrentState[i]; // Stores the current reading of the potentiometer to compare with the next
        midiPreviousState[i] = midiCurrentState[i];
      }
    }
  }
}
