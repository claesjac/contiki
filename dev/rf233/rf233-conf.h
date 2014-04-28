/*
 * rf233-conf.h
 *
 *  Created on: Apr 7, 2014
 *      Author: poppe
 */

#ifndef RF233_CONF_H_
#define RF233_CONF_H_


#define RF233_CONF_FRAME_RETRIES	3


//#define RF233_HOOK_TX_PACKET
//#define RF233_HOOK_IS_SEND_ENABLED
//#define RF233_HOOK_RX_PACKET

#define RADIOALWAYSON	1


#ifdef IEEE802154_CONF_CHANNEL
#define IEEE802154_CHANNEL           IEEE802154_CONF_CHANNEL
#else
#define IEEE802154_CHANNEL           11
#endif


#endif /* RF233_CONF_H_ */
