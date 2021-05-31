#include <Wire.h>
#include <Adafruit_MCP4725.h>
const int i2cAddress = 0x60;
Adafruit_MCP4725 dac;

int pots[8] = {A0, A1, A2, A3, A6, A7, A8, A9};
float noteVolt = 0.08333;
// Change these notes to the ones you'd like to quantize to. Include the full list of 0-11 if you
// don't want to quantize against a specific scale, but just notes
float _scale[] = {0, 2, 4, 5, 7, 9, 11};

// adjust for voltages that go over 5. This recales the incoming voltages from the pots to the analog pins.
float rescale = 0.9;

// This is the pin that either the external or internal sync (via the 555 circuit) comes from
int SYNCPIN = 16;
// this is the pin that the gate signal is sent to (via the op-amp circuit)
int GATEOUTPIN = 15;
// incoming voltage that tells the arduino how long to send the gate signal on each beat
int GATELENGTHPIN = 10;
bool gateup = false;

// don't worry about these
int scaleLength = 7;
int totalLength = 35;
float scale[35];

long lastNoteMillis = 0;

void setup() {
  for(int i=0; i<8; i++) {
    pinMode(i, INPUT);
  }
  dac.begin(i2cAddress);
  pinMode(SYNCPIN, INPUT);

  pinMode(GATEOUTPIN, OUTPUT);

  int remain;
  int leave;
  for(int x=0; x<totalLength; x++) {
    remain = x % scaleLength;
    leave = floor(x / scaleLength);
    if (x < scaleLength) {
      scale[x] = _scale[x];
    } else {
      scale[x] = _scale[remain] + leave * 12;
    }
  }
}

int currentNote = 0;
int notes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float newNote;
int note;
long curMillis;

float noteVoltage;
float newVoltage;
int noteNote;
int quantNote;
int aheadNote = -1;
int outNote = -1;

int lastSyncPin = 0;
int syncRead;

long fullLength;
float gateLength = 1;
long nextStopGate = 1;

void loop() {
  // adjust sync
  syncRead = digitalRead(SYNCPIN);
  curMillis = millis();

  if (syncRead == HIGH && lastSyncPin == LOW) {
    // rising signal, it's a note
    note = analogRead(pots[currentNote]);
    noteVoltage = note / 1024.0 * 5.0 * rescale;
    if (noteVoltage > 5.0 * rescale) {
      noteVoltage = 5.0 * rescale;
    }
    noteNote = round(noteVoltage / noteVolt);
    for(int x=0; x<totalLength; x++) {
      quantNote = scale[x];
      if (quantNote == -1 && noteNote >= quantNote) {
        aheadNote = quantNote;
      } else if (quantNote >= 0 && quantNote > noteNote) {
        // we have a lower bound and now an upper bound
        if (noteNote - aheadNote < quantNote - noteNote) {
          outNote = aheadNote;
        } else {
          outNote = quantNote;
        }
        // now we have a note
        newVoltage = floor(outNote / 12.0) + outNote % 12 * noteVolt;
        dac.setVoltage(newVoltage / 5 * 4096, false);
        aheadNote = -1;
        outNote = -1;
        break;
      }
    }
    currentNote++;
    if (currentNote == 8) {
      currentNote = 0;
    }

    // figure out gate length
    fullLength = curMillis - lastNoteMillis;
    int gateLengthValue = analogRead(GATELENGTHPIN);
    gateLength = fullLength * gateLengthValue / 1024.0;
    nextStopGate = curMillis + gateLength;
    lastNoteMillis = curMillis;
    digitalWrite(GATEOUTPIN, HIGH);
    gateup = true; 
  }

  if (curMillis >= nextStopGate && gateup == true) { 
    digitalWrite(GATEOUTPIN, LOW);
    gateup = false;
  }

  lastSyncPin = syncRead;
}
