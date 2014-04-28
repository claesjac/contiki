/*
 * rf233-arch.c
 *
 *  Created on: Apr 2, 2014
 *      Author: poppe
 */

#include "contiki.h"

#include "rf233.h"
#include "halGpio.h"

void
rf233_arch_init(void)
{
/* Enable the SPI Clock */

	RF233_SPI_TRANSFER_CLOSE();

	  // Configure IO pins
	  HAL_GPIO_PHY_SLP_TR_out();
	  HAL_GPIO_PHY_SLP_TR_clr();
	  HAL_GPIO_PHY_RST_out();
	  HAL_GPIO_PHY_IRQ_in();
	  HAL_GPIO_PHY_IRQ_pmuxen();
	  HAL_GPIO_PHY_CS_out();
	  HAL_GPIO_PHY_MISO_in();
	  HAL_GPIO_PHY_MISO_pmuxen();
	  HAL_GPIO_PHY_MOSI_out();
	  HAL_GPIO_PHY_MOSI_pmuxen();
	  HAL_GPIO_PHY_SCK_out();
	  HAL_GPIO_PHY_SCK_pmuxen();

	  // Configure SPI
	  PORT->Group[HAL_GPIO_PORTC].PMUX[9].bit.PMUXE = PORT_PMUX_PMUXE_F_Val;  // SCK
	  PORT->Group[HAL_GPIO_PORTC].PMUX[9].bit.PMUXO = PORT_PMUX_PMUXO_F_Val;  // MISO
	  PORT->Group[HAL_GPIO_PORTB].PMUX[15].bit.PMUXE = PORT_PMUX_PMUXE_F_Val; // MOSI

	  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM4;

	  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM4_GCLK_ID_CORE) |
	      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

	  SERCOM4->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
	  while (SERCOM4->SPI.SYNCBUSY.reg);

	  SERCOM4->SPI.BAUD.reg = 0; /* MCK: 1M baud: 500 kHz */

	  SERCOM4->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_ENABLE |
	      SERCOM_SPI_CTRLA_MODE(SERCOM_SPI_CTRLA_MODE_SPI_MASTER_Val) |
	      SERCOM_SPI_CTRLA_DIPO(0) | SERCOM_SPI_CTRLA_DOPO(1);
	  while (SERCOM4->SPI.SYNCBUSY.reg);




}
void __attribute__((__interrupt__))
SERCOM4_Handler(void)
{
	while(1){;}
}

uint8_t rf233_arch_write(uint8_t data)
{
	SERCOM4->SPI.DATA.reg = data;
	while (!SERCOM4->SPI.INTFLAG.bit.RXC);
	data = SERCOM4->SPI.DATA.reg;
	return data;
}

