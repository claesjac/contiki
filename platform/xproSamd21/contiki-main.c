/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \addtogroup platform
 * @{
 *
 * \defgroup cc2538 The cc2538 Development Kit platform
 *
 * The cc2538DK is the new platform by Texas Instruments, based on the
 * cc2530 SoC with an ARM Cortex-M3 core.
 * @{
 *
 * \file
 *   Main module for the cc2538dk platform
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"

#include "lib/random.h"
#include "net/netstack.h"
#include "net/queuebuf.h"
#include "net/ip/tcpip.h"
#include "net/ip/uip.h"
#include "net/mac/frame802154.h"
#include "dev/serial-line.h"
#include "cpu.h"
//#include "reg.h"
#include "ieee-addr.h"
//#include "rf233.h"
#include "debug-uart.h"
#include "sensors.h"
#include "watchdog.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
/*---------------------------------------------------------------------------*/
#if STARTUP_CONF_VERBOSE
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define PUTS(s) puts(s)


static uint8_t src_addr[8];



void
board_init(void);
/*---------------------------------------------------------------------------*/


#if 0
static void
fade(unsigned char l)
{
  volatile int i;
  int k, j;
  for(k = 0; k < 800; ++k) {
    j = k > 400 ? 800 - k : k;

    leds_on(l);
    for(i = 0; i < j; ++i) {
      asm("nop");
    }
    leds_off(l);
    for(i = 0; i < 400 - j; ++i) {
      asm("nop");
    }
  }
}
#endif

/*---------------------------------------------------------------------------*/
static void
set_rime_addr()
{
	ieee_addr_cpy_to(&linkaddr_node_addr.u8[0], LINKADDR_SIZE);

#if STARTUP_CONF_VERBOSE
  {
    int i;
    printf("Rime configured with address ");
    for(i = 0; i < LINKADDR_SIZE - 1; i++) {
      printf("%02x:", linkaddr_node_addr.u8[i]);
    }
    printf("%02x\n", linkaddr_node_addr.u8[i]);
  }
#endif

}
/*---------------------------------------------------------------------------*/
/**
 * \brief Main routine for the cc2538dk platform
 */
int
main(void) {
 clock_init();
 board_init();
 rtimer_init();
 //gpio_init();



  process_init();

  watchdog_init();


  dbg_setup_uart();
  dbg_set_input_handler(&serial_line_input_byte);

  serial_line_init();
 INTERRUPTS_ENABLE();

  PUTS(CONTIKI_VERSION_STRING);
  PUTS(BOARD_STRING);

  PRINTF(" Net: ");
  PRINTF("%s\n", NETSTACK_NETWORK.name);
  PRINTF(" MAC: ");
  PRINTF("%s\n", NETSTACK_MAC.name);
  PRINTF(" RDC: ");
  PRINTF("%s\n", NETSTACK_RDC.name);

  /* Initialise the H/W RNG engine. */
  random_init(0);
/* This is temp this should be put in project */
//  serial_shell_init();
//  shell_rime_debug_init();
//  shell_power_init();
//  shell_text_init();
//  shell_time_init();
//  shell_rime_sniff_init();

/* end */
  process_start(&etimer_process, NULL);
  ctimer_init();

  set_rime_addr();
  netstack_init();
#if 0
  rf233_set_channel(IEEE802154_CHANNEL);
  rf233_set_pan_addr(IEEE802154_PANID, 0x3344, &linkaddr_node_addr.u8[0]);
  uint8_t c = rf233_get_channel();
  uint16_t pid = rf233_get_panid();
  PRINTF("CHANNEL: %u\n", c);
  PRINTF("PANID: 0x%04X\n", pid);
#endif

#if 0
#if UIP_CONF_IPV6
  memcpy(&uip_lladdr.addr, &linkaddr_node_addr, sizeof(uip_lladdr.addr));
  queuebuf_init();
  process_start(&tcpip_process, NULL);
#endif /* UIP_CONF_IPV6 */
#endif
  process_start(&sensors_process, NULL);

  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);

  autostart_start(autostart_processes);

  watchdog_start();
//  fade(LEDS_ORANGE);

  while(1) {
    uint8_t r;
    do {
      /* Reset watchdog and handle polls and events */
      watchdog_periodic();

      r = process_run();
    } while(r > 0);

    /* We have serviced all pending events. Enter a Low-Power mode. */
  //  lpm_enter();
  }
}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
