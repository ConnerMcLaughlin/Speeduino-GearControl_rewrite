#ifndef COMMS_H
#define COMMS_H

//These are the page numbers that the Tuner Studio serial protocol uses to transverse the different map and config pages.

#define simple_remote_setupPage    1 //Config Page 1
#define port_editor_config         2 // port editor config page

byte cmdreply;
byte realtimebuffer[60];   //buffer array to store the realtime data read
volatile byte replylength;          // the length of the reply variable
byte zero = 0;
byte packetSize = 6;
byte requestCount = 0;

void command();
void do_rCommands(byte commandletter, byte canid, uint16_t cmdoffset, uint16_t cmdlength);
void receiveValue(uint16_t rvOffset, byte newValue);
void sendPage(uint16_t send_page_Length,bool useChar, byte can_id);
void sendValues(uint16_t offset, uint16_t packetLength, byte cmd);
void sendTheCommand(byte commandletter, byte canid, uint16_t cmdoffset, uint16_t cmdlength);

#endif // COMMS_H
