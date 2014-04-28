/*
 * rf233-arch.c
 *
 *  Created on: Apr 2, 2014
 *      Author: poppe
 */

#include "contiki.h"

#include "rf233.h"
#include "pmc.h"

void
rf233_arch_init(void)
{
/* Enable the SPI Clock */

	RF233_SPI_TRANSFER_CLOSE();

	SPI->SPI_CR |= SPI_CR_SPIDIS;
	SPI->SPI_CR |= SPI_CR_SWRST;

	SPI->SPI_MR = SPI_MR_MSTR;

	SPI->SPI_MR &= (~SPI_MR_DLYBCS_Msk);
	SPI->SPI_MR |= SPI_MR_DLYBCS( SPI_DLYBCS );
	SPI->SPI_MR |= SPI_MR_MODFDIS;
	SPI->SPI_MR &= ~SPI_MR_LLB;
	SPI->SPI_MR |= SPI_MR_PCS(0xff);

	SPI->SPI_MR &= (~SPI_MR_PCS_Msk);
	SPI->SPI_MR |= SPI_MR_PCS(~(1 << 0));

	//SPI->SPI_MR |= SPI_MR_WDRBT;

	SPI->SPI_CSR[0] &= ~(SPI_CSR_DLYBS_Msk | SPI_CSR_DLYBCT_Msk | SPI_CSR_SCBR_Msk);
	 SPI->SPI_CSR[0] |= SPI_CSR_SCBR(SPI_CLK_RATIO);
	SPI->SPI_CSR[0] |= (SPI_CSR_DLYBS(SPI_DLYBS) | SPI_CSR_DLYBCT(SPI_DLYBCT) );

	/* Set Phase and Polarity */
	SPI_CLK_POLARITY ? (SPI->SPI_CSR[0] |= SPI_CSR_NCPHA) : (SPI->SPI_CSR[0] &= (~SPI_CSR_NCPHA));
	SPI_CLK_PHASE ? (SPI->SPI_CSR[0] |= SPI_CSR_CPOL) : (SPI->SPI_CSR[0] &= (~SPI_CSR_CPOL));

	/* Bit Quanity Sent */
	SPI->SPI_CSR[0] &= (~SPI_CSR_BITS_Msk);
	SPI->SPI_CSR[0] |= SPI_CSR_BITS_8_BIT;
	/* CS Behavior */
	//SPI->SPI_CSR[0] |= SPI_CSR_CSAAT;
	//->SPI_CSR[0] |= SPI_CSR_CSNAAT;
	/* Keep a fixed Periphial Select */
	SPI->SPI_MR |= (SPI_MR_PS);
	SPI->SPI_MR &= (~SPI_MR_PCSDEC);
	SPI->SPI_CR |= SPI_CR_SPIEN;



}
void __attribute__((__interrupt__))
SPI_Handler(void)
{
	while(1){;}
}

uint8_t rf233_arch_write(uint8_t data)
{
	while(!(SPI->SPI_SR & SPI_SR_TDRE)){;}
	SPI->SPI_TDR = (( SPI_TDR_PCS(~(1 << 0)) ) | SPI_TDR_TD(data));
	/* endless loop until SPI complete */

	while(!(SPI->SPI_SR & SPI_SR_RDRF)){;}
//	while(SPI->SPI_SR && SPI_SR_TXEMPTY != 1){;}
	data = SPI->SPI_RDR;

	return data;
}

