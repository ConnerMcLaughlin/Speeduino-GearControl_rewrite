#ifndef AUX_H
#define AUX_H

void selectorInput();
void gearStatus();
void gearOutput();
void stepgear(byte);
void driveOutputs();
void outputaction(byte chanX);
void initialOutputs();
void porteditorOutput();

  bool state;
  bool invstate;
  byte portX;

#endif
