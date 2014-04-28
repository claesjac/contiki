
#include "contiki.h"

#include <debug-uart.h>

#include <string.h>

#include "cpu.h"
//#include "pmc.h"
#include "halGpio.h"
#include "ringbuf.h"

#ifndef DBG_XMIT_BUFFER_LEN
#define DBG_XMIT_BUFFER_LEN 256
#endif
#ifndef DBG_RECV_BUFFER_LEN
#define DBG_RECV_BUFFER_LEN 128
#endif

static unsigned char dbg_xmit_buffer[DBG_XMIT_BUFFER_LEN];
static unsigned char dbg_recv_buffer[DBG_RECV_BUFFER_LEN];
static struct ringbuf dbg_ringbuf;

static int dbg_recv_handler_func();
static uint32_t send_active;

HAL_GPIO_PIN(UART_TXD, A, 4);
HAL_GPIO_PIN(UART_RXD, A, 5);

void
dbg_setup_uart()
{

	ringbuf_init(&dbg_ringbuf, dbg_xmit_buffer, DBG_XMIT_BUFFER_LEN);

	/*(uint64_t)65536 * (48000000 - 16 * 115200) / 48000000*/
	 // uint16_t brr = 63019;
//	uint16_t brr = 55469; // baud: 9600 mck: 1M
//	uint16_t brr = 45403;// baud: 19200 mck: 1M
	uint16_t brr = 5138;// baud: 57600 mck: 1M

	  HAL_GPIO_UART_TXD_out();
	  HAL_GPIO_UART_TXD_pmuxen();
	  HAL_GPIO_UART_RXD_in();
	  HAL_GPIO_UART_RXD_pmuxen();

	  PORT->Group[0].PMUX[2].bit.PMUXE = PORT_PMUX_PMUXE_D_Val; // TX
	  PORT->Group[0].PMUX[2].bit.PMUXO = PORT_PMUX_PMUXO_D_Val; // RX

	  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

	  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
	      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

	  while (SERCOM0->USART.SYNCBUSY.reg);
	  SERCOM0->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN |
	      SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);

	  SERCOM0->USART.BAUD.reg = SERCOM_USART_BAUD_BAUD(brr);

	  while (SERCOM0->USART.SYNCBUSY.reg);
	  SERCOM0->USART.CTRLA.reg =
	      SERCOM_USART_CTRLA_ENABLE | SERCOM_USART_CTRLA_DORD |
	      SERCOM_USART_CTRLA_MODE(SERCOM_USART_CTRLA_MODE_USART_INT_CLK_Val) |
	      SERCOM_USART_CTRLA_RXPO(1/*PAD1*/) | SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);


	  NVIC_EnableIRQ(SERCOM0_IRQn);

	  SERCOM0->USART.INTENSET.reg = SERCOM_USART_INTENSET_TXC;

	  send_active = 0;
}

static int (*input_func)(unsigned char c) = NULL;

void SERCOM0_Handler(void)
{
	dbg_recv_handler_func();
}

static int dbg_recv_handler_func()
{
	uint8_t *c8;

	uint16_t status = SERCOM0->USART.STATUS.reg;

	 if (SERCOM0->USART.INTFLAG.bit.TXC == 1) {
		 if(ringbuf_elements(&dbg_ringbuf) > 0) {
			 SERCOM0->USART.DATA.reg = (ringbuf_get(&dbg_ringbuf));
			 return 1;
		 } else {
			 send_active = 0;
			 SERCOM0->USART.INTFLAG.bit.TXC = 1;
		 }

	 }

	 if(SERCOM0->USART.INTFLAG.bit.RXC == 1) {

		 uint16_t c = (unsigned char)SERCOM0->USART.DATA.bit.DATA;
		 c8 = (uint8_t *)&c;

		 /* This is for Echo only */
		 dbg_putchar(*c8);

		 if (input_func != NULL) {
			 input_func(*c8);
		 }
		 return 1;
	 }

	  return 0;
}

void
dbg_set_input_handler(int (*handler)(const char c))
{
	  input_func = handler;

	  SERCOM0->USART.INTENSET.bit.RXC = 1;
}
static volatile unsigned char mutex = 0;

unsigned int
dbg_send_bytes(const unsigned char *seq, unsigned int len)
{
	unsigned int x;
	  //INTERRUPTS_DISABLE();
	for(x=0; len; len--, x++) {
		if(ringbuf_put(&dbg_ringbuf, *seq++) == 0){
			return x;
		}
	}
	  //INTERRUPTS_ENABLE();
	if(send_active == 0)
	{
		send_active = 1;
		SERCOM0->USART.DATA.reg = SERCOM_SPI_DATA_DATA(ringbuf_get(&dbg_ringbuf));
	}
	  return x;
}
static unsigned char dbg_write_overrun = 0;

void
dbg_putchar(const char ch)
{
	  if (dbg_write_overrun) {
	    if (dbg_send_bytes((const unsigned char*)"^",1) != 1) return;
	  }
	  dbg_write_overrun = 0;
	  if (dbg_send_bytes((const unsigned char*)&ch,1) != 1) {
	    dbg_write_overrun = 1;
	  }
}

void
dbg_blocking_putchar(const char ch)
{
	  if (dbg_write_overrun) {
	    while (dbg_send_bytes((const unsigned char*)"^",1) != 1);
	  }
	  dbg_write_overrun = 0;
	  while (dbg_send_bytes((const unsigned char*)&ch,1) != 1);
}

void
dbg_drain()
{
	//while(!(UART0->UART_SR & UART_SR_TXBUFE));
}


