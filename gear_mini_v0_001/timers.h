

/*
NOTE - This file and it's associated functions need a CLEARER NAME

//Purpose
We're implementing a lower frequency interrupt loop to perform calculations that are needed less often, some of which depend on time having passed (delta/time) to be meaningful.


//Technical
Timer2 is only 8bit so we are setting the prescaler to 128 to get the most out of it. This means that the counter increments every 0.008ms and the overflow at 256 will be after 2.048ms
Max Period = (Prescale)*(1/Frequency)*(2^8)
(See http://arduinomega.blogspot.com.au/2011/05/timer2-and-overflow-interrupt-lets-get.html)

We're after a 1ms interval so we'll need 131 intervals to reach this ( 1ms / 0.008ms per tick = 125).
Hence we will preload the timer with 131 cycles to leave 125 until overflow (1ms).

*/
#ifndef TIMERS_H
#define TIMERS_H

//#if defined(CORE_AVR)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)|| defined (ARDUINO_AVR_PRO) 
  #include <avr/interrupt.h>
  #include <avr/io.h>
#endif
  
#if defined (CORE_TEENSY)
  IntervalTimer lowResTimer;
  void oneMSInterval();
#elif defined(CORE_STM32)
  void oneMSInterval();
#endif  
  
volatile int loop100ms;
volatile int loop250ms;
volatile int loopSec;

void initialiseTimers();

#endif // TIMERS_H
