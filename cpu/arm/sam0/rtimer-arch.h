/**
 * \file
 *         Header file for the STM32F103-specific rtimer code
 * \author
 *         Simon Berg <ksb@users.sourceforge.net>
 */

#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include "sys/rtimer.h"



#define RTIMER_ARCH_PRESCALER  		256
#define RTIMER_INTERVAL 			1000  /* length of time (ms) it takes to overflow timer */
#define RTIMER_ARCH_TOP        (((1000000ul/1000ul) / RTIMER_ARCH_PRESCALER) * RTIMER_INTERVAL)


/* One tick: 200 us. Using this value we will advance about 5.53
   sec after a day. */
#define RTIMER_ARCH_SECOND            3906


void rtimer_arch_set(rtimer_clock_t t);

rtimer_clock_t rtimer_arch_now();

#endif /* RTIMER_ARCH_H_ */
