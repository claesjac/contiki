/*
 * rf233-arch.h
 *
 *  Created on: Apr 2, 2014
 *      Author: poppe
 */

#ifndef RF233_ARCH_H_
#define RF233_ARCH_H_

#include "contiki.h"
#include "dev/halGpio.h"

HAL_GPIO_PIN(PHY_RST,    B, 15);
HAL_GPIO_PIN(PHY_IRQ,    B, 0);
HAL_GPIO_PIN(PHY_SLP_TR, A, 20);
HAL_GPIO_PIN(PHY_CS,     B, 31);
HAL_GPIO_PIN(PHY_MISO,   C, 19);
HAL_GPIO_PIN(PHY_MOSI,   B, 30);
HAL_GPIO_PIN(PHY_SCK,    C, 18);

#define rf233_arch_rst_high()		HAL_GPIO_PHY_RST_set()
#define rf233_arch_rst_low()		HAL_GPIO_PHY_RST_clr()
#define rf233_arch_rst_state()		HAL_GPIO_PHY_RST_state()
#define rf233_arch_slptr_high()		HAL_GPIO_PHY_SLP_TR_set()
#define rf233_arch_slptr_low()		HAL_GPIO_PHY_SLP_TR_clr()
#define rf233_arch_slptr_state()	HAL_GPIO_PHY_SLP_TR_state()

#define SPI_CLK_POLARITY 0
#define SPI_CLK_PHASE    1
#define SPI_CLK_RATIO	120 /* Ratio from MCK to spi clk rate */

#define SPI_DLYBS        0x06 //Delay before clock
#define SPI_DLYBCT       0x04 //Delay between transfers
#define SPI_DLYBCS		 0x0f //Delay between CS
#if 1
#define RF233_SPI_TRANSFER_OPEN()	HAL_GPIO_PHY_CS_clr()
#define RF233_SPI_TRANSFER_CLOSE()	HAL_GPIO_PHY_CS_set()
#else
#define RF233_SPI_TRANSFER_OPEN()	 	do{ \
											SPI->SPI_MR &= (~SPI_MR_PCS_Msk); \
											SPI->SPI_MR |= SPI_MR_PCS(~(1 << 0)); \
										} while(0)

#define RF233_SPI_TRANSFER_CLOSE()	 	do { \
											SPI->SPI_MR &= (~SPI_MR_PCS_Msk); \
											SPI->SPI_MR |= SPI_MR_PCS(~(1 << 1));\
										}while(0)
#endif
#define HAL_SPI_TRANSFER


#define HAL_ENTER_CRITICAL_REGION()	INTERRUPTS_DISABLE()
#define HAL_LEAVE_CRITICAL_REGION()	INTERRUPTS_ENABLE()
void rf233_arch_init(void);
uint8_t rf233_arch_write(uint8_t data);
#define HAL_SPI_TRANSFER_FAST()





#endif /* RF233_ARCH_H_ */
