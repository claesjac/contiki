/*
 * rf233.c
 *
 *  Created on: Apr 2, 2014
 *      Author: poppe
 */
#include "contiki.h"

#include "stdint.h"
#include "stdio.h"
#include "radio.h"
#include "rf233.h"
#include "cpu.h"
#include "packetbuf.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PUTS(s) puts(s)
#else
#define PRINTF(...)
#define PUTS(s)
#endif


/* Double check that max len includes checksum */
static rf233_buffer_t buffer;
uint8_t RF233_receive_on;

uint8_t volatile rf233_pending=0;


int
rf233_init(void)
{
  /* Wait in case VCC just applied */
  clock_delay(TIME_TO_ENTER_P_ON);

  /* Initialize Hardware Abstraction Layer */
  rf233_arch_init();


  /* Do full rf230 Reset */
  rf233_arch_rst_low();
  rf233_arch_slptr_low();

  /* On powerup a TIME_RESET delay is needed here, however on some other MCU reset
   * (JTAG, WDT, Brownout) the radio may be sleeping. It can enter an uncertain
   * state (sending wrong hardware FCS for example) unless the full wakeup delay
   * is done.
   * Wake time depends on board capacitance; use 2x the nominal delay for safety.
   * See www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=78725
   */
  clock_delay(2*TIME_SLEEP_TO_TRX_OFF);
  //delay_us(TIME_RESET); /* Old impl. */
  rf233_arch_rst_high();
  clock_delay(4*TIME_P_ON_TO_TRX_OFF);
  /* Force transition to TRX_OFF */
  rf233_subregister_write(SR_TRX_CMD, CMD_FORCE_TRX_OFF);
  clock_delay(4*TIME_P_ON_TO_TRX_OFF);

  /* Verify that it is a supported version */
  /* Note gcc optimizes this away if DEBUG is not set! */
  //RF233 - version 1, ID 31
  uint8_t tpart =  rf233_register_read(RG_PART_NUM);
  uint8_t tmanu = rf233_register_read(RG_MAN_ID_0);
  uint8_t tvers = rf233_register_read(RG_VERSION_NUM);

  if (tpart != RF233_PART_NUM) {
	  PRINTF("rf233: Unsupported part num %u\n", tpart);
  }
  if (tvers != RF233_VERSION) {
    PRINTF("rf233: Unsupported version %u\n",tvers);
  }
  if (tmanu != RF233_MANUFACTURER_ID) {
    PRINTF("rf233: Unsupported manufacturer ID %u\n",tmanu);
  }

  PRINTF("rf233: Version %u, ID %u\n",tvers, tmanu);


 // rf230_warm_reset();

 /* Start the packet receive process */
//  process_start(&rf230_process, NULL);

 /* Leave radio in on state (?)*/
//  radio_on();

  return 1;
}
static uint8_t
rf233_set_state(uint8_t new_state)
{
	uint8_t current_state = RF233_STATE();

	/* check if new state is a valid to transition */
	if((new_state != TRX_OFF) && (new_state != RX_ON) && (new_state != PLL_ON) \
			&& (new_state != RX_AACK_ON) && (new_state != TX_ARET_ON))
	{
		return -1; //TODO: Give reason
	}
	if (new_state == current_state)
	{
		return 0;
	}

	switch(new_state) {
		case TRX_OFF:
			 /* make sure we are not in sleep */

			// check if transceiver is in a state that will transfer to TRX_OFF
			if( (current_state == RX_ON) || (current_state == PLL_ON) || (current_state == RX_AACK_ON) || (current_state == TX_ARET_ON)) {
				rf233_subregister_write(SR_TRX_CMD, CMD_TRX_OFF);
				clock_delay(20);
			}
#if 0
			else if(force) { //if force is allowed force the transfer

				rf233_register_write(SR_TRX_CMD, CMD_FORCE_TRX_OFF);
				clock_delay(20);
			}
#endif
		break;

		case RX_ON:
			rf233_subregister_write(SR_TRX_CMD, CMD_RX_ON);
			if (SLEEP == TRX_OFF) {
				clock_delay(180);
			}else if((current_state == RX_AACK_ON) || (current_state == RX_ON) || \
					(current_state == PLL_ON) || (current_state == TX_ARET_ON)) {
				clock_delay(10);
			}
		case PLL_ON:
			if (current_state == TRX_OFF) {
				rf233_subregister_write(SR_TRX_CMD, CMD_PLL_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);
			}
		break;

		case RX_AACK_ON:
			if (current_state == TRX_OFF) {
				rf233_subregister_write(SR_TRX_CMD, CMD_RX_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);
				current_state = RF233_STATE();
			}

			if ((current_state == RX_ON) || (current_state == PLL_ON)) {
				rf233_subregister_write(SR_TRX_CMD, CMD_RX_AACK_ON);
			//	clock_delay(TIME_STATE_TRANSITION_PLL_ACTIVE);
			} else if (current_state == TX_ARET_ON) {
				rf233_subregister_write(SR_TRX_CMD, CMD_RX_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);

				rf233_subregister_write(SR_TRX_CMD, CMD_RX_AACK_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);
			}
		break;

		case TX_ARET_ON:

			if (current_state == TRX_OFF) {

				rf233_subregister_write(SR_TRX_CMD, CMD_RX_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);
				current_state = RF233_STATE();
			}


			if ((current_state == RX_ON) || (current_state == PLL_ON)) {

				rf233_subregister_write(SR_TRX_CMD, CMD_TX_ARET_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);
			}
			else if (current_state == RX_AACK_ON) {

				rf233_subregister_write(SR_TRX_CMD, CMD_RX_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);

				rf233_subregister_write(SR_TRX_CMD, CMD_TX_ARET_ON);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);
			}
		break;

		case SLEEP:

			rf233_subregister_write(SR_TRX_CMD, CMD_FORCE_TRX_OFF);
				clock_delay(TIME_TRX_OFF_TO_PLL_ACTIVE);

				rf233_arch_slptr_high();
		break;

		default:

			return -1;

	}
	/* Verify transission */
  	current_state = RF233_STATE();
	if(new_state == current_state) {
		return 0;
	}

	else {
		PUTS("RF233 did not change to the proper state\n");
		return -1;
	}
}

void
rf230_set_promiscuous_mode(uint8_t isPromiscuous) {
#if RF230_CONF_AUTOACK
    is_promiscuous = isPromiscuous;
/* TODO: Figure out when to pass promisc state to 802.15.4 */
//    radio_set_trx_state(is_promiscuous?RX_ON:RX_AACK_ON);
#endif
}
/*---------------------------------------------------------------------------*/
static int
rf233_prepare(const void *payload, unsigned short payload_len);
/*---------------------------------------------------------------------------*/
static int
rf233_transmit(unsigned short payload_len);
/*---------------------------------------------------------------------------*/
static int
rf233_send(const void *payload, unsigned short payload_len);
/*---------------------------------------------------------------------------*/
static int
rf233_read(void *buf, unsigned short bufsize);
/*---------------------------------------------------------------------------*/
static int
rf233_cca(void);
/*---------------------------------------------------------------------------*/
int
rf233_receiving_packet(void);
/*---------------------------------------------------------------------------*/
static int
rf233_pending_packet(void);
/*---------------------------------------------------------------------------*/
static int
rf233_on(void);
/*---------------------------------------------------------------------------*/
static int
rf233_off(void);
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static int
rf233_prepare(const void *payload, unsigned short payload_len)
{
	  int ret = 0;
	  uint8_t total_len,*pbuf;


   	  PRINTF("rf230: sending %d bytes\n", payload_len);

//	  RIMESTATS_ADD(tx);

	  /* Copy payload to RAM buffer */
	  total_len = payload_len + AUX_LEN;
	  if (total_len > RF233_MAX_FRAME_LENGTH){
	    PRINTF("rf233_prepare: packet too large (%d, max: %d)\n",total_len,RF233_MAX_FRAME_LENGTH);
	    return -1;
	  }
	  pbuf = &buffer.data[0];
	  memcpy(pbuf,payload,payload_len);
	  pbuf+=payload_len;

	/*------------------------------------------------------------*/

	#ifdef RF233_HOOK_TX_PACKET
	  RF230BB_HOOK_TX_PACKET(buffer,total_len);
	#endif


	  return ret;
}

/*---------------------------------------------------------------------------*/
static int
rf233_transmit(unsigned short payload_len)
{
	  int txpower;
	  uint8_t total_len;
	  uint8_t tx_result;

	  /* If radio is sleeping we have to turn it on first */
	  /* This automatically does the PLL calibrations */
	  if (rf233_arch_slptr_state()) {
		  rf233_arch_slptr_low();
	    clock_delay(2*TIME_SLEEP_TO_TRX_OFF); //extra delay (2x) depends on board capacitance


	  } else {

	  }

	  /* Prepare to transmit */
	#if RF233_CONF_FRAME_RETRIES
	  rf233_set_state(TX_ARET_ON);
	#else
	  rf233_set_state(PLL_ON);
	#endif

	  txpower = 0;

	  if(packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER) > 0) {
	    /* Remember the current transmission power */
	    txpower = rf233_get_txpower();
	    /* Set the specified transmission power */
	    set_txpower(packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER) - 1);
	  }

	  total_len = payload_len + AUX_LEN;

	  ENERGEST_ON(ENERGEST_TYPE_TRANSMIT);

	/* No interrupts across frame download! */
	  HAL_ENTER_CRITICAL_REGION();

	  /* Toggle the SLP_TR pin to initiate the frame transmission, then transfer
	   * the frame. We have about 16 us + the on-air transmission time of 40 bits
	   * (for the synchronization header) before the transceiver sends the PHR. */
	  rf233_arch_slptr_high();
	  rf233_arch_slptr_low();
	  rf233_frame_write(buffer.data, total_len);

	  HAL_LEAVE_CRITICAL_REGION();
	  PRINTF("rf233_transmit: %d\n", (int)total_len);



	 /* We wait until transmission has ended so that we get an
	     accurate measurement of the transmission time.*/
	  rf233_waitidle();

	 /* Get the transmission result */
	#if RF233_CONF_FRAME_RETRIES
	  tx_result = rf233_subregister_read(SR_TRAC_STATUS);
	#else
	  tx_result=RADIO_TX_OK;
	#endif



	 /* Restore the transmission power */
	 if(packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER) > 0) {
	    set_txpower(txpower & 0xff);
	  }

	  ENERGEST_OFF(ENERGEST_TYPE_TRANSMIT);
	  if(RF233_receive_on) {
	    ENERGEST_ON(ENERGEST_TYPE_LISTEN);
	    rf233_on();
	  } else {
	#if RADIOALWAYSON
	    /* Enable reception */
	    rf233_on();
	#else
	    rf233_off();
	    PRINTF("rf233_transmit: turning radio off\n");
	#endif
	  }

	  if (tx_result==1) {        //success, data pending from addressee
	    tx_result=RADIO_TX_OK;           //handle as ordinary success
	  }

	  if (tx_result==RADIO_TX_OK) {
//	    RIMESTATS_ADD(lltx);
	//    if(packetbuf_attr(PACKETBUF_ATTR_RELIABLE))
	  //    RIMESTATS_ADD(ackrx);		//ack was requested and received
	  } else if (tx_result==3) {        //CSMA channel access failure
//	    RIMESTATS_ADD(contentiondrop);
	    PRINTF("rf230_transmit: Transmission never started\n");
	    tx_result = RADIO_TX_COLLISION;
	  } else if (tx_result==5) {        //Expected ACK, none received
	    tx_result = RADIO_TX_NOACK;
	    PRINTF("rf230_transmit: ACK not received\n");
//	    RIMESTATS_ADD(badackrx);		//ack was requested but not received
	  } else if (tx_result==7) {        //Invalid (Can't happen since waited for idle above?)
	    tx_result = RADIO_TX_ERR;
	  }

	  return tx_result;
}
static int
rf233_send(const void *payload, unsigned short payload_len) {
	uint8_t ret = 0;

#ifdef RF233_HOOK_IS_SEND_ENABLED
if(!RF233_HOOK_IS_SEND_ENABLED()) {
	goto bail;
}
#endif

	ret = rf233_prepare(payload, payload_len);
	if(ret != 0) {
		PRINTF("rf233_send: Unable to send, prep failed (%d)\n", ret);
		return -1;
	}

	ret = rf233_transmit(payload_len);


	#if RADIOSTATS
	if (ret != 0) RF230_sendfail++;
	#endif

	return ret;
}
/*---------------------------------------------------------------------------*/
static int
rf233_off(void)
{
/* Don't do anything if we are already turned off. */
//if (hal_get_slptr()) DEBUGFLOW('6');

/* If we are currently receiving a packet, we still call radio_off(),
 as that routine waits until Rx is complete (packet uploaded in ISR
 so no worries about losing it). The transmit routine may also turn
+     the radio off on a return to sleep. rf230_isidle checks for that. */
	if (!rf233_isidle()) {
	PRINTF("rf233_off: busy receiving\r\n");
	return 1;
	}

	rf233_arch_slptr_high();
return 0;
}
/*---------------------------------------------------------------------------*/
static int
rf233_on(void)
{
if(RF233_receive_on) {
//if (hal_get_slptr()) DEBUGFLOW('Q');//Cooja TODO: shows sleeping occasionally
return 1;
}

rf233_arch_slptr_low();;
return 1;
}

//TODO: Check RIMESTAT and what I could add from this section about the packet size
static int
rf233_read(void *buf, unsigned short bufsize) {
	 uint8_t len,*framep;

	  /* The length includes the twp-byte checksum but not the LQI byte */
	  len = buffer.len;
	  if (len==0) {
	    return 0;
	  }

	  if(bufsize < buffer.len) {
		  return 0;
	  }

	 /* Transfer the frame, stripping the footer, but copying the checksum */
	  framep = &buffer.data[0];
	  memcpy(buf,framep,len-AUX_LEN);
//	  rf230_last_correlation = rxframe[rxframe_head].lqi;

	  /* Clear the length field to allow buffering of the next packet */
	  buffer.len=0;

	  rf233_pending = 0;

//	RIMESTATS_ADD(rx);

	#ifdef RF233_HOOK_RX_PACKET
	  RF233_HOOK_RX_PACKET(buf,len);
	#endif

	  /* Here return just the data length. The checksum is however still in the buffer for packet sniffing */
	  return len - AUX_LEN;
}
static int
rf233_cca(void) {
	  uint8_t cca=0;
	  uint8_t radio_was_off = 0;

	  /* Turn radio on if necessary. If radio is currently busy return busy channel */
	  /* This may happen when testing radio duty cycling with RADIOALWAYSON,
	   * or because a packet just started. */
	  if(RF233_receive_on) {
	    if (rf233_arch_slptr_state()) {  //should not be sleeping!
		  goto busyexit;
		} else {
	      if (!rf233_isidle()) {
	        goto busyexit;
	      }
		}
	  } else {
	    radio_was_off = 1;
	    rf233_on();
	  }

	  ENERGEST_ON(ENERGEST_TYPE_LED_YELLOW);

	  HAL_ENTER_CRITICAL_REGION();

	  rf233_waitidle();

	  rf233_subregister_write(SR_CCA_REQUEST,1);

	  clock_delay(TIME_CCA);

	  while ((cca & 0x80) == 0 ) {
	    if (rf233_arch_slptr_state() == 0) {
	        break;
	    }
	    cca=rf233_register_read(RG_TRX_STATUS);
	  }
	  HAL_LEAVE_CRITICAL_REGION();


	  ENERGEST_OFF(ENERGEST_TYPE_LED_YELLOW);
	  if(radio_was_off) {
	    rf233_off();
	  }

	   if (cca & 0x40) {
		 return 1;
	   } else {
	 busyexit:
		 return 0;
	   }
}
int
rf233_receiving_packet(void){

	uint8_t radio_state;
	  if (rf233_arch_slptr_state()) {
	  } else {
	    radio_state = rf233_subregister_read(SR_TRX_STATUS);
	    if ((radio_state==BUSY_RX) || (radio_state==BUSY_RX_AACK)) {
	      return 1;
	    }
	  }
	  return 0;
}
static int
rf233_pending_packet(void){
#if RF230_INSERTACK
    if(ack_pending == 1) return 1;
#endif
  return rf233_pending;
}


const struct radio_driver rf233_driver =
  {
    rf233_init,
    rf233_prepare,
    rf233_transmit,
    rf233_send,
    rf233_read,
    rf233_cca,
    rf233_receiving_packet,
    rf233_pending_packet,
    rf233_on,
    rf233_off,
  };
