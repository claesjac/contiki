#include "contiki.h"

#include <sys/clock.h>
#include <sys/cc.h>
#include <sys/etimer.h>
#include <debug-uart.h>
#include <energest.h>
#include <stdint.h>

static volatile clock_time_t count = 0;
static volatile unsigned long secs = 0;
static unsigned int second_countdown = CLOCK_SECOND;


void
SysTick_Handler(void)
{
	  ENERGEST_ON(ENERGEST_TYPE_IRQ);
	  count++;
	  if(etimer_pending()) {
	    etimer_request_poll();
	  }

	  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}


void
clock_init()
{
#if 1
	if(SysTick_Config(1000000/CLOCK_SECOND)) {
		while(1);
	}
#endif
}

clock_time_t
clock_time(void)
{
  return count;
}

void
clock_delay(unsigned int us) {
	uint32_t target_time = SysTick->VAL;;
	us *= CLOCK_SECOND;

	/* since this is a count down and target time is lower we know the timer needs to reset */
	if(target_time < us) {
		/* subtract the time we are going to wait from the time left so we know how much longer to wait */
		us -= target_time;
		while(target_time < (SysTick->VAL)){;}
		/* new target time to wait. We wait for value to be lower than target time now */
		target_time = SysTick->LOAD - us;
	} else {
		/* since target time is greater than delay we know that we have room for count down */
		target_time -= us;
	}
	/* us is now the start of loop Value so if we miss the target time being lower we can check if timer has reset */
	us = SysTick->VAL;
	while((target_time < SysTick->VAL) || (SysTick->VAL > us)) {
		;
	}
}

unsigned long
clock_seconds(void)
{
  return secs;
}
