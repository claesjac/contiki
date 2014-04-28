/*
 * watchdog.h
 *
 *  Created on: Mar 27, 2014
 *      Author: poppe
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_


#define WDT_KEY_WORD (0xA5u << 24)
#define WDT_SLCK_DIV      128	/* This is the required slow clock divider by the ATMEL*/
#define WDT_MAX_VALUE     4095  /* This is the max a 12-bit register can be */

void
watchdog_init(void);

void
watchdog_start(void);

void
watchdog_periodic(void);

void
watchdog_stop(void);

void
watchdog_reboot(void);

#endif /* WATCHDOG_H_ */
