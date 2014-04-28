/*
 * rf233-dev.h
 *
 *  Created on: Apr 4, 2014
 *      Author: poppe
 */

#ifndef RF233_DEV_H_
#define RF233_DEV_H_

uint8_t
rf233_register_read(uint8_t address);

void
rf233_register_write(uint8_t address, uint8_t value);

void
rf233_subregister_write(uint8_t address, uint8_t mask, uint8_t position,
                            uint8_t value);
uint8_t
rf233_subregister_read(uint8_t address, uint8_t mask, uint8_t position);

void
rf233_frame_read(rf233_buffer_t *rx_frame);

void
rf233_frame_write(uint8_t *write_buffer, uint8_t length);
#endif /* RF233_DEV_H_ */
