/*
 * rf233-commands.c
 *
 *  Created on: Apr 6, 2014
 *      Author: poppe
 */

#include "contiki.h"
#include "stdio.h"

#include "rf233.h"




#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/*---------------------------------------------------------------------------*/
void
rf233_set_channel(uint8_t c)
{
 /* Wait for any transmission to end. */
  PRINTF("rf233: Set Channel %u\n",c);
 // rf233_waitidle();
  //channel=c;
  rf233_subregister_write(SR_CHANNEL, c);
}

uint8_t
rf233_get_channel(void)
{
	//  rf233_waitidle();
	  //channel=c;
	  return (rf233_subregister_read(SR_CHANNEL));
}

/*---------------------------------------------------------------------------*/
void
rf233_set_txpower(uint8_t power)
{
  set_txpower(power);
}
/*---------------------------------------------------------------------------*/
uint8_t
rf233_get_txpower(void)
{
	uint8_t power = -1;
	if (rf233_arch_slptr_state()) {
		PRINTF("rf233_get_txpower:Sleeping");
	} else {
		power = rf233_subregister_read(SR_TX_PWR);
	}
	return power;
}

/*---------------------------------------------------------------------------*/
void
set_txpower(uint8_t power)
{
//  if (power > TX_PWR_17_2DBM){
//    power=TX_PWR_17_2DBM;
//  }
  if (rf233_arch_slptr_state()) {
    PRINTF("rf233_set_txpower:Sleeping");  //happens with cxmac
  } else {
    rf233_subregister_write(SR_TX_PWR, power);
  }
}
/*---------------------------------------------------------------------------*/
char
rf233_isidle(void)
{
  uint8_t radio_state;
  /* Contikimac can turn the radio off during an interrupt, so we always check
   * slptr before doing the SPI transfer. The caller must also make this test
   * if it could otherwise hang waiting for idle! */
  if (rf233_arch_slptr_state()) {
	  return 1;
  }
  else {
    radio_state = rf233_subregister_read(SR_TRX_STATUS);
    if (radio_state != BUSY_TX_ARET &&
      radio_state != BUSY_RX_AACK &&
      radio_state != STATE_TRANSITION &&
      radio_state != BUSY_RX &&
      radio_state != BUSY_TX) {
      return(1);
    }
    else {
      return(0);
    }
  }
}
/*---------------------------------------------------------------------------*/
void
rf233_set_pan_addr(unsigned pan,
                    unsigned addr,
                    const uint8_t ieee_addr[8])
//rf230_set_pan_addr(uint16_t pan,uint16_t addr,uint8_t *ieee_addr)
{
  PRINTF("rf233: PAN=%x Short Addr=%x\n",pan,addr);

  uint8_t abyte;
  abyte = pan & 0xFF;
  rf233_register_write(RG_PAN_ID_0,abyte);
  abyte = (pan >> 8*1) & 0xFF;
  rf233_register_write(RG_PAN_ID_1, abyte);

  abyte = addr & 0xFF;
  rf233_register_write(RG_SHORT_ADDR_0, abyte);
  abyte = (addr >> 8*1) & 0xFF;
  rf233_register_write(RG_SHORT_ADDR_1, abyte);

  if (ieee_addr != NULL) {
    PRINTF("MAC=%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_7, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_6, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_5, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_4, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_3, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_2, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_1, *ieee_addr++);
    PRINTF(":%x",*ieee_addr);
    rf233_register_write(RG_IEEE_ADDR_0, *ieee_addr);
    PRINTF("\n");
  }
}
/*---------------------------------------------------------------------------*/
void
rf233_waitidle(void)
{
  /* TX_ARET with multiple csma retries can take a very long time to finish */
  while (1) {
    if (rf233_arch_slptr_state()) return;
    if (rf233_isidle()) break;
  }
}

/*---------------------------------------------------------------------------*/
uint16_t
rf233_get_panid(void)
{
	uint16_t panid;

    panid = rf233_register_read(RG_PAN_ID_0);
    panid |= ((rf233_register_read(RG_PAN_ID_1)) << 8);

    return panid;
}
