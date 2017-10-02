/*
  Returns how much free dynamic memory exists (between heap and stack)
*/
#include "utils.h"

int16_t freeRam ()
{
//#if defined(CORE_AVR)
  extern int16_t __heap_start, *__brkval;
  int16_t v;
  return (int16_t) &v - (__brkval == 0 ? (int16_t) &__heap_start : (int16_t) __brkval);
//#elif defined(CORE_TEENSY)
//  uint32_t stackTop;
//  uint32_t heapTop;

  // current position of the stack.
//  stackTop = (uint32_t) &stackTop;

  // current position of heap.
//  void* hTop = malloc(1);
//  heapTop = (uint32_t) hTop;
//  free(hTop);

  // The difference is the free, available ram.
//  return (uint16_t)stackTop - heapTop;
//#elif defined(CORE_STM32)
  //Figure this out some_time
//  return 0;
//#endif
}

void setGearboxConfig()
{
    byte gearID = configPage1.gearbox_type & B00011111;  //need this filter to block out the unsed bits which could be randomly set
    //common outputs
    for(byte i = 9; i; i--)
         shiftsolenoid[i - 1] = 255;
    for(byte i = 33; i; i--)
         spareOut[i - 1] = 255;
    shiftsolenoid[1] = 1;            // shift solenoid active == 1 , pwm == 100, and  not used == 255
    shiftsolenoid[2] = 1;
    lockup_solenoid[1] = 1;     //
    lockup_solenoid[2] = 255;     // lockup solenoid pin active
    starter_inhibit = 1;        //inhibit relay use enable/disable
    //common shift pattern
    for(byte i = 9; i; i--)
         gear[i - 1] = 255;
    rev_gear = 1;
    switch (gearID)
    {
      case 0 ... 2:  //A340E, AODE, and 5R55s
            gear[1] = 1;                // solenoid pattern eg 1 is only shiftsolenoid1
            gear[2] = 3;                // solenoid pattern eg 3 is both shiftsolenoid1 and solenoid2
                
    }
    switch (gearID)
    {
      case 0: //A340E
            //shift pattern
            gear[3] = 2;                // shiftsolenoid pattern eg 2 is only solenoid2

      break;

      case 1: //AODE
            shiftsolenoid[3] = 1;
            shiftsolenoid[4] = 1;
            gear[3] = 2;                // shiftsolenoid pattern eg 2 is only solenoid2
            gear[4] = 0;
                              
      break;

      case 2: //5R55s
            //outputs
            shiftsolenoid[3] = 1;
            shiftsolenoid[4] = 1;
            shiftsolenoid[5] = 1;
            //shift pattern
            gear[3] = 5;                // solenoid pattern 
            gear[4] = 2;
            gear[5] = 4;
            
      break;
      
      case 3: //HR16DE 4speed
            //outputs
            //shift pattern
            gear[1] = 3;                // solenoid pattern 
            gear[2] = 2;                // solenoid pattern 
            gear[3] = 0;                // solenoid pattern 
            gear[4] = 1;
            
      break;
    }
}

void setPinMapping(byte boardID)
{
    for(byte i = 17; i; i--)
    {
         pinOut[i - 1] = 255;
         pinIn[i - 1] = 255;
         pinAin[i - 1] = 255;
    }
    switch (boardID)
  {
    case 0:   //pro-mini demo V0.001
      pinOut[1] = 2; //
      pinOut[2] = 3; //
      pinOut[3] = 4; //

      pinIn[1] = 5;
      pinIn[2] = 6;
      pinIn[3] = 7;
      pinIn[4] = 8;
      pinIn[5] = 9;
      pinIn[6] = A3;
             
      pinAin[1] = A0;
      pinAin[2] = A1;
      pinAin[3] = A2;
      
      break;
      
  #if defined(ARDUINO_AVR_MEGA2560)
      case 1:   // mega2560 demo V0.001
      pinOut[1] = 33; //
      pinOut[2] = 35; //
      pinOut[3] = 37; //
      pinOut[4] = 39; //
      pinOut[5] = 41; //
      pinOut[6] = 43; //
      pinOut[13] = 13; //

      pinIn[1] = 28;
      pinIn[2] = 30;
      pinIn[3] = 32;
      pinIn[4] = 34;
      pinIn[5] = 36;
      pinIn[6] = 38;
      pinIn[7] = 40;
      pinIn[8] = 42;
      pinIn[9] = 44;
      pinIn[10] = 46;
      pinIn[11] = 48;
             
      pinAin[1] = A0;
      pinAin[2] = A1;
      pinAin[3] = A2;
      pinAin[4] = A3;
      
      break;
  #endif      

  #if defined(ARDUINO_AVR_MEGA2560)
      case 3:   // mega2560 using Speeduino V0.3 board
      pinOut[1] = 8; //
      pinOut[2] = 9; //
      pinOut[3] = 10; //
      pinOut[4] = 11; //
      pinOut[5] = 28; //
      pinOut[6] = 24; //
      pinOut[7] = 40; //
      pinOut[8] = 36; //
      pinOut[9] = 4; //
      pinOut[10] = 5; //
      pinOut[11] = 6; //

      pinIn[1] = 12;
             
      pinAin[1] = A0;
      pinAin[2] = A1;
      pinAin[3] = A2;
      pinAin[4] = A3;
      pinAin[5] = A8;
      
      break;
  #endif

  #if defined(ARDUINO_AVR_MEGA2560)
      case 4:   // mega2560 using Speeduino V0.4 board
      pinOut[1] = 8; //
      pinOut[2] = 9; //
      pinOut[3] = 10; //
      pinOut[4] = 11; //
      pinOut[5] = 40; //
      pinOut[6] = 38; //
      pinOut[7] = 52; //
      pinOut[8] = 50; //
      pinOut[9] = 4; //
      pinOut[10] = 5; //
      pinOut[11] = 6; //
      pinOut[12] = 7; //
      pinOut[13] = 45; //
      pinOut[14] = 47; //

      pinIn[1] = 12;
             
      pinAin[1] = A0;
      pinAin[2] = A1;
      pinAin[3] = A2;
      pinAin[4] = A3;
      pinAin[5] = A8;
      pinAin[16] = 0;
      
      break;
  #endif
  }


//set pinmode states      
  for(byte i = 17; i; i--)
  {
      if (pinOut[i - 1] < 255) pinMode(pinOut[i - 1], OUTPUT);
      if (pinIn[i - 1] < 255) pinMode(pinIn[i - 1], INPUT);
      if (pinAin[i - 1] < 255) pinMode(pinAin[i - 1], INPUT);
  } 
}

void setUsageflags()
{
//setup pin use flags for digouts
    for(byte p = 17; p; p--)
        {
        if (pinOut[p - 1] <255)
            BIT_SET(configPage1.DoutchanActive, (p-1));
        else
            BIT_CLEAR(configPage1.DoutchanActive, (p-1));    

// setup pin use flags for dig ins
        if (pinIn[p - 1] < 255)
            BIT_SET(configPage1.DinchanActive, (p-1));
        else
            BIT_CLEAR(configPage1.DinchanActive, (p-1));    

//seup pin use flags for analog ins
        if (pinAin[p - 1] < 255)
            BIT_SET(configPage1.AinchanActive, (p-1));
        else
            BIT_CLEAR(configPage1.AinchanActive, (p-1));    
        }

//setup gearbox configuration flags for  valid outputs
    for(byte i = 8; i; i--)
    {
        if (shiftsolenoid[i] < 255)
          BIT_SET(configPage1.FunctionActive, (i - 1));          //if the shiftsolenoid is in use ie !255 then set the functionactive flag so TS knows its used 
        else
          BIT_CLEAR(configPage1.FunctionActive, (i - 1));
    }      
        if (lockup_solenoid[1] < 255)
           BIT_SET(configPage1.FunctionActive, 8);
        else  
           BIT_CLEAR(configPage1.FunctionActive, 8); 
        if (lockup_solenoid[2] < 255)
           BIT_SET(configPage1.FunctionActive, 9);
        else  
           BIT_CLEAR(configPage1.FunctionActive, 9);
        
        if (starter_inhibit < 255)
           BIT_SET(configPage1.FunctionActive, (10)); 
        else
           BIT_CLEAR(configPage1.FunctionActive, (10)); 
        
    for(byte i = 16; i; i--)
    {
        if (spareOut[i] < 255)
           BIT_SET(configPage1.FunctionActive, (i - 1)); 
        else  
           BIT_CLEAR(configPage1.FunctionActive, (i - 1));
        if (spareOut[i + 16] < 255)
           BIT_SET(configPage1.FunctionActive_2, (i - 1));
        else  
           BIT_CLEAR(configPage1.FunctionActive_2, (i - 1));
    }
        
//setup use flags for gear active
    for(byte p=1; p < 9; p++)
        {
        if (gear[p] < 255)
            BIT_SET(configPage1.GearActive, p-1);
        else
            BIT_CLEAR(configPage1.GearActive, p-1);    
        }

//setup gear_port_Enabled array values , gear_port_Enabled uses [0] to [15]
// boxoutput holds the pinout[x] value for the specific output function  
      boxOutput[0] = configPage1.shiftsolenoid_1 & B00011111;
      boxOutput[1] = configPage1.shiftsolenoid_2 & B00011111;
      boxOutput[2] = configPage1.shiftsolenoid_3 & B00011111;
      boxOutput[3] = configPage1.shiftsolenoid_4 & B00011111;
      boxOutput[4] = configPage1.shiftsolenoid_5 & B00011111;
      boxOutput[5] = configPage1.shiftsolenoid_6 & B00011111;
      boxOutput[6] = configPage1.shiftsolenoid_7 & B00011111;
      boxOutput[7] = configPage1.shiftsolenoid_8 & B00011111;
      boxOutput[8] = configPage1.lockup_solenoid_1 & B00011111;
      boxOutput[9] = configPage1.lockup_solenoid_2 & B00011111;
      boxOutput[10] = configPage1.starter_inhibit_out & B00011111;
      boxOutput[11] = configPage1.boxOutput_11 & B00011111; 
      boxOutput[12] = configPage1.boxOutput_12 & B00011111; 
      boxOutput[13] = configPage1.boxOutput_13 & B00011111; 
      boxOutput[14] = configPage1.boxOutput_14 & B00011111; 
      boxOutput[15] = configPage1.boxOutput_15 & B00011111; 
      
    //  for (byte ge = 1; ge < 17 ; ge++)         //ge = 1 is start value , ge < 17 is one more thnan last value
    //      {
          //  byte bo = boxOutput[ge] &B00011111;
    //         if(BIT_CHECK(configPage1.FunctionActive, (ge-1)) == 1){gear_port_Enabled[(boxOutput[ge])] = 1; }
    //      }
      for(byte i = 11; i; i--)
          {  if(BIT_CHECK(configPage1.FunctionActive, (i - 1)) == 1)
                gear_port_Enabled[(boxOutput[i - 1])] = 1;}
                
            activestate[0] = configPage1.shiftsolenoid_1 & B00100000;
            activestate[1] = configPage1.shiftsolenoid_2 & B00100000;
            activestate[2] = configPage1.shiftsolenoid_3 & B00100000;
            activestate[3] = configPage1.shiftsolenoid_4 & B00100000;
            activestate[4] = configPage1.shiftsolenoid_5 & B00100000;
            activestate[5] = configPage1.shiftsolenoid_6 & B00100000;
            activestate[6] = configPage1.shiftsolenoid_7 & B00100000;
            activestate[7] = configPage1.shiftsolenoid_8 & B00100000;
            activestate[8] = configPage1.lockup_solenoid_1 & B00100000;
            activestate[9] = configPage1.lockup_solenoid_2 & B00100000;
            activestate[10] = configPage1.starter_inhibit_out & B00100000;

}


