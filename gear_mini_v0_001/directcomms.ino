#include <Arduino.h>

/*
Speeduino Can interface
Copyright (C) Darren Siepka
A full copy of the license may be found in the projects root directory

Based on code by Josh Stewart for the Speeduino project , see www.Speeduino.com for more info
*/

/*
This is called when a command is received over serial from TunerStudio / Megatune
It parses the command and calls the relevant function
*/
#include "directcomms.h"
#include "globals.h"
//#include "storage.h"


void direct_serial_command()
{
    switch (Serial.read())
          {
          case 'A':
                  direct_sendValues(0, direct_packetSize, 60);
          break; 
           
          case 'B': // Burn current values to eeprom
                    writeConfig();
          break;

          case 'C': // test communications. This is used by Tunerstudio to see whether there is an ECU on a given serial port
                  //      testComm();
          break;

          case 'E': // receive command button commands
                  byte Etmp;
                  uint16_t Eoffset;
                  while (!Serial.available());
                  Etmp = Serial.read();
                  while (Serial.available());
                  Eoffset = word(Serial.read(), Etmp);
                  commandButtons(Eoffset);
          break;
          
          case 'F': // send serial protocol version
                  Serial.print("001");
          break;

          case 'P': // set the current page
                    //A 2nd byte of data is required after the 'P' specifying the new page number.
                  while (Serial.available());
                  //if (Serial.available() == 0) { return; }
                  currentStatus.currentPage = Serial.read();
                  if (currentStatus.currentPage >= '0')//This converts the ascii number char into binary
                     currentStatus.currentPage -= '0';
                  
          break;
      
          case 'Q': // send code version
                    for (uint16_t sg = 0; sg < sizeof(simple_remote_signature) - 1; sg++)
                        Serial.write(simple_remote_signature[sg]);  
          break;
          
          case 'S': // send code version
                    for (uint16_t sg = 0; sg < sizeof(simple_remote_RevNum) - 1; sg++)
                        {
                        Serial.write(simple_remote_RevNum[sg]);
                        currentStatus.secl = 0; //This is required in TS3 due to its stricter timings
                        }
          break;

          case 'V': // send VE table and constants in binary
                  direct_sendPage(0,thistsCanId,0);
          break;

          case 'W': // receive new VE obr constant at 'W'+<offset>+<newbyte>
                while (!Serial.available());
                byte Wtmp;
                uint16_t Woffset;
                while (!Serial.available());
                Wtmp = Serial.read();
                while (!Serial.available());
                Woffset = word(Serial.read(), Wtmp);
                while (!Serial.available());
                direct_receiveValue(Woffset, Serial.read());
          break;
     
          case 'r': 
                byte cmd;
                byte tsCanId_sent;         
                uint16_t offset, length;
                byte tmp;
                while (!Serial.available());
                tsCanId_sent = Serial.read(); //Read the $tsCanId
                while (!Serial.available());
                cmd = Serial.read();
                while (!Serial.available());
                tmp = Serial.read();
                while (!Serial.available());
                offset = word(Serial.read(), tmp);
                while (!Serial.available());
                tmp = Serial.read();
                if (cmd != 87)          //if is "W" only 1 more byte is sent
                {
                    while (!Serial.available());
                    length = word(Serial.read(), tmp); 
                }
                else
                    length = tmp;
                      
                dodirect_rCommands(cmd,tsCanId_sent,offset,length);
                
          break;
    }

return;
 
}
void dodirect_rCommands(byte commandletter, byte canid, uint16_t cmdoffset, uint16_t cmdlength)
{
  
    switch (commandletter)
           {
           case 15:    //
                    for (uint16_t sg = 0; sg < sizeof(simple_remote_signature) - 1; sg++)
                        Serial.write(simple_remote_signature[sg]);  
                        // Serial.write(simple_remote_signature[sg]);
           break;
                        
           case 14:  //
                    for (uint16_t sg = 0; sg < sizeof(simple_remote_RevNum) - 1; sg++)
                        {
                        Serial.write(simple_remote_RevNum[sg]);
                        currentStatus.secl = 0; //This is required in TS3 due to its stricter timings
                        }     
           break;
                        
           case 48:    //previously 0x30:
                                 Serial.print("got to 3d");
                                 // direct_sendValues(offset, length, cmd);
           break;
                        
           case 60:  //(0x3c+120 == 0xB4(112dec)):       
                   direct_sendValues(cmdoffset, cmdlength, 60);
           break;

           case 66: // r version of B == 0x42
                    // Burn current values to eeprom
                    writeConfig();
           break;
           
           case 69: // r version of E == 0x45
                  commandButtons(cmdoffset);
           break;
                               
           case 80:  //r version of P == dec80
                  currentStatus.currentPage = byte(cmdoffset);
           break;
          
           case 86:  //r version of V == dec86
                  direct_sendPage(cmdlength,thistsCanId,86);
           break;
                    
           case 87:  //r version of W
                 // int16_t valueOffset; //cannot use offset as a variable name, it is a reserved word for several teensy libraries
                  direct_receiveValue(cmdoffset, cmdlength);  //Serial.read());                    
           break;
       } //closes the switch/case 
}

void direct_sendTheCommand(byte commandletter, byte canid, uint16_t cmdoffset, uint16_t cmdlength)
{

}

void direct_receiveValue(uint16_t rvOffset, byte newValue)
{      
        
  void* pnt_configPage;//This only stores the address of the value that it's pointing to and not the max size

  switch (currentStatus.currentPage)
  {

    case 1: //simple_remote_setupPage:
      pnt_configPage = &configPage1; //Setup a pointer to the relevant config page
     //For some reason, TunerStudio is sending offsets greater than the maximum page size. I'm not sure if it's their bug or mine, but the fix is to only update the config page if the offset is less than the maximum size
      if ( rvOffset < page_1_size)
        *((byte *)pnt_configPage + (byte)rvOffset) = newValue; //
      break;

    case 2: //port editor config Page:
      pnt_configPage = &configPage2; //Setup a pointer to the relevant config page
     //For some reason, TunerStudio is sending offsets greater than the maximum page size. I'm not sure if it's their bug or mine, but the fix is to only update the config page if the offset is less than the maximum size
      if ( rvOffset < page_2_size)
        *((byte *)pnt_configPage + (uint16_t)rvOffset) = newValue; //
      break;
  
  }
}

 //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
sendPage() packs the data within the current page (As set with the 'P' command)
into a buffer and sends it.
Note that some translation of the data is required to lay it out in the way Megasqurit / TunerStudio expect it
useChar - If true, all values are send as chars, this is for the serial command line interface. TunerStudio expects data as raw values, so this must be set false in that case
*/
void direct_sendPage(uint16_t send_page_Length, byte can_id, byte cmd)
{

 //currentPage = pagenum;
 
        void* pnt_configPage;

        switch (currentStatus.currentPage)
          {

            case simple_remote_setupPage:  //veSetPage:
                // currentTitleIndex = 27;

                pnt_configPage = &configPage1; //Create a pointer to Page 1 in memory  
                send_page_Length = page_1_size; 
            break;  

            case port_editor_config:  //port editor config Page:
                // currentTitleIndex = 27;

                pnt_configPage = &configPage2; //Create a pointer to Page 2 in memory  
                send_page_Length = page_2_size; 
            break;

            default:
                // currentTitleIndex = 27;

                pnt_configPage = &configPage1; //Create a pointer to Page 1 in memory  
                send_page_Length = page_1_size; 

          }
    
          //All other bytes can simply be copied from the config table
          
          byte response[send_page_Length];
          for ( uint16_t x = 0; x < send_page_Length; x++)
              response[x] = *((byte *)pnt_configPage + (uint16_t)(x)); //Each byte is simply the location in memory of the configPage + the offset(not used) + the variable number (x)

          if (cmd == 206)   //came via passthrough from serial3
            {
              SERIALLink.print("r");
              SERIALLink.write(thistsCanId);                //canId of the device you are requesting data from
              SERIALLink.write(cmd);                       //0x56+120dec = 0xCE  
              SERIALLink.write(zero);                       // dummy offset lsb
              SERIALLink.write(zero);                       // dummy offset msb
              SERIALLink.write(lowByte(send_page_Length));  // length lsb
              SERIALLink.write(highByte(send_page_Length)); // length msb
              SERIALLink.write((byte *)&response, sizeof(response));          
            }
          else
              Serial.write((byte *)&response, sizeof(response));
      
}
/*
This function is used to store calibration data sent by Tuner Studio.
*/
void direct_receiveCalibration(byte tableID)
{

}

void direct_sendValues(uint16_t offset, uint16_t packetLength, byte cmd)
{
  
  byte fullStatus[direct_packetSize];
  byte response[packetLength];

    if(direct_requestCount == 0)
       currentStatus.secl = 0;
    direct_requestCount++;

  fullStatus[0] = currentStatus.secl; //secl is simply a counter that increments each second. Used to track unexpected resets (Which will reset this count to 0)
  fullStatus[1] = currentStatus.systembits; //Squirt Bitfield
  fullStatus[2] = currentStatus.GearNow;
  fullStatus[3] = lowByte(currentStatus.loopsPerSecond);
  fullStatus[4] = highByte(currentStatus.loopsPerSecond);
    //The following can be used to show the amount of free memory
  currentStatus.freeRAM = freeRam();
  fullStatus[5] = lowByte(currentStatus.freeRAM); //(byte)((currentStatus.loopsPerSecond >> 8) & 0xFF);
  fullStatus[6] = highByte(currentStatus.freeRAM);
  fullStatus[7] = lowByte(mainLoopCount);
  fullStatus[8] = highByte(mainLoopCount);
  fullStatus[9] = lowByte(currentStatus.dev1);
  fullStatus[10] = highByte(currentStatus.dev1);
  fullStatus[11] = lowByte(currentStatus.dev2);
  fullStatus[12] = highByte(currentStatus.dev2);
  fullStatus[13] = lowByte(currentStatus.dev3);
  fullStatus[14] = highByte(currentStatus.dev3);
  fullStatus[15] = lowByte(currentStatus.dev4);
  fullStatus[16] = highByte(currentStatus.dev4);
  fullStatus[17] = currentStatus.testIO_hardware;
  fullStatus[18] = lowByte(currentStatus.digIn);
  fullStatus[19] = highByte(currentStatus.digIn);
  fullStatus[20] = lowByte(currentStatus.digOut);    
  fullStatus[21] = highByte(currentStatus.digOut);
  fullStatus[22] = lowByte(currentStatus.Analog[1]);
  fullStatus[23] = highByte(currentStatus.Analog[1]);
  fullStatus[24] = lowByte(currentStatus.Analog[2]);
  fullStatus[25] = highByte(currentStatus.Analog[2]);
  fullStatus[26] = lowByte(currentStatus.Analog[3]);
  fullStatus[27] = highByte(currentStatus.Analog[3]);
  fullStatus[28] = lowByte(currentStatus.Analog[4]);
  fullStatus[29] = highByte(currentStatus.Analog[4]);  
  fullStatus[30] = lowByte(currentStatus.Analog[5]);
  fullStatus[31] = highByte(currentStatus.Analog[5]);
  fullStatus[32] = lowByte(currentStatus.Analog[6]);
  fullStatus[33] = highByte(currentStatus.Analog[6]);
  fullStatus[34] = lowByte(currentStatus.Analog[7]);
  fullStatus[35] = highByte(currentStatus.Analog[7]);
  fullStatus[36] = lowByte(currentStatus.Analog[8]);
  fullStatus[37] = highByte(currentStatus.Analog[8]);
  fullStatus[38] = lowByte(currentStatus.Analog[9]);
  fullStatus[39] = highByte(currentStatus.Analog[9]);
  fullStatus[40] = lowByte(currentStatus.Analog[10]);
  fullStatus[41] = highByte(currentStatus.Analog[10]);
  fullStatus[42] = lowByte(currentStatus.Analog[11]);
  fullStatus[43] = highByte(currentStatus.Analog[11]);
  fullStatus[44] = lowByte(currentStatus.Analog[12]);
  fullStatus[45] = highByte(currentStatus.Analog[12]);  
  fullStatus[46] = lowByte(currentStatus.Analog[13]);
  fullStatus[47] = highByte(currentStatus.Analog[13]);
  fullStatus[48] = lowByte(currentStatus.Analog[14]);
  fullStatus[49] = highByte(currentStatus.Analog[14]);
  fullStatus[50] = lowByte(currentStatus.Analog[15]);
  fullStatus[51] = highByte(currentStatus.Analog[15]);
  fullStatus[52] = lowByte(currentStatus.Analog[16]);
  fullStatus[53] = highByte(currentStatus.Analog[16]);    
 
  for(byte x=0; x<packetLength; x++)
    response[x] = fullStatus[offset+x];

  if (cmd == 60)
      Serial.write(response, (size_t)packetLength); 
      //Serial.write(response, (size_t)packetLength);
  else if (cmd == 180)
    {
      //Serial.print("r was sent");
      SERIALLink.write("r");         //confirm cmd letter 
      SERIALLink.write(zero);           //canid
      SERIALLink.write(cmd);          //confirm cmd
      SERIALLink.write(lowByte(offset));                       //start offset lsb
      SERIALLink.write(highByte(offset));                      //start offset msb
      SERIALLink.write(lowByte(packetLength));      //confirm no of byte to be sent
      SERIALLink.write(highByte(packetLength));      //confirm no of byte to be sent
      SERIALLink.write(response, (size_t)packetLength); //stream the realtime data requested
    }   
  return;
}

void commandButtons(uint16_t cmdCombined)
{

  switch (cmdCombined)
  {   
    case 256: // cmd is stop    
      BIT_CLEAR(currentStatus.testIO_hardware, 1);    //clear testactive flag
      currentStatus.digOut = 0;                   //reset all outputs to off
      //currentStatus.digin = 0;                   //reset all outputs to off
      break;

    case 257: // cmd is enable
      // currentStatus.testactive = 1;
      BIT_SET(currentStatus.testIO_hardware, 0);  //set testenabled flag(bit 0)    
      BIT_SET(currentStatus.testIO_hardware, 1);  //set testactive flag (bit 1)
      break;
          
    case 513 ... 528: // cmd group is on actions
      if(BIT_CHECK(currentStatus.testIO_hardware, 1))
          BIT_SET(currentStatus.digOut, (cmdCombined-513));
      break;
      
    case 769 ... 784: // cmd group is off actions
      if(BIT_CHECK(currentStatus.testIO_hardware, 1))
          BIT_CLEAR(currentStatus.digOut, (cmdCombined-769));
      break;

    case 1537 ... 1552: // cmd group is on actions
      if(BIT_CHECK(currentStatus.testIO_hardware, 1))
          BIT_SET(currentStatus.digIn, (cmdCombined-1537));
        // currentStatus.dev1 = BIT_CHECK(currentStatus.digIn, (cmdCombined-1537));
        // currentStatus.dev2 = BIT_CHECK(currentStatus.testIO_hardware, 1);
      break;
      
    case 1788 ... 1793: // cmd group is off actions
      if(BIT_CHECK(currentStatus.testIO_hardware, 1))
          BIT_CLEAR(currentStatus.digIn, (cmdCombined-1793));
        // currentStatus.dev1 = BIT_CHECK(currentStatus.digIn, (cmdCombined-1793));
        // currentStatus.dev2 = BIT_CHECK(currentStatus.testIO_hardware, 1);
      break;  
  }
}        
