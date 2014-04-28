/*
 * rf233.h
 *
 *  Created on: Apr 2, 2014
 *      Author: poppe
 */

#ifndef RF233_H_
#define RF233_H_

#define RF233_VERSION				0x01
#define RF233_MANUFACTURER_ID		0x1F
#define RF233_PART_NUM				0x0B

#define CRC_BAD			-1
#define CRC_OK			0

#define RF233_MAX_FRAME_LENGTH 		127
#define RF233_MIN_FRAME_LENGTH    		3

#define CHECKSUM_LEN 2
#define ED_LEN	1
#define STATUS_LEN 1

#define AUX_LEN (CHECKSUM_LEN + ED_LEN + STATUS_LEN)

#define RF233_STATE()	(rf233_subregister_read(SR_TRX_STATUS))

typedef enum{
    RADIO_SUCCESS,  /**< The requested service was performed successfully. */
    RADIO_UNSUPPORTED_DEVICE,         /**< The connected device is not an Atmel AT86RF230. */
    RADIO_INVALID_ARGUMENT,           /**< One or more of the supplied function arguments are invalid. */
    RADIO_TIMED_OUT,                  /**< The requested service timed out. */
    RADIO_WRONG_STATE,                /**< The end-user tried to do an invalid state transition. */
    RADIO_BUSY_STATE,                 /**< The radio transceiver is busy receiving or transmitting. */
    RADIO_STATE_TRANSITION_FAILED,    /**< The requested state transition could not be completed. */
    RADIO_CCA_IDLE,                   /**< Channel is clear, available to transmit a new frame. */
    RADIO_CCA_BUSY,                   /**< Channel busy. */
    RADIO_TRX_BUSY,                   /**< Transceiver is busy receiving or transmitting data. */
    RADIO_BAT_LOW,                    /**< Measured battery voltage is lower than voltage threshold. */
    RADIO_BAT_OK,                     /**< Measured battery voltage is above the voltage threshold. */
    RADIO_CRC_FAILED,                 /**< The CRC failed for the actual frame. */
    RADIO_CHANNEL_ACCESS_FAILURE,     /**< The channel access failed during the auto mode. */
    RADIO_NO_ACK,                     /**< No acknowledge frame was received. */
}radio_status_t;

typedef struct {
	uint8_t data[RF233_MAX_FRAME_LENGTH + CHECKSUM_LEN];
	uint8_t flags;
	uint8_t len;
	uint8_t lqi;
	uint8_t crc;
	uint8_t ed;
}rf233_buffer_t;

extern uint8_t RF233_receive_on;

#include "at86rf233_registermap.h"
#include "rf233-arch.h"
#include "rf233-dev.h"
#include "rf233-conf.h"
#include "rf233-commands.h"

#endif /* RF233_H_ */
