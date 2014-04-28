#include "contiki.h"

#include "cpu.h"
#include "watchdog.h"

/*---------------------------------------------------------------------------*/
/** \brief Initialisation function for the WDT. Currently simply explicitly
 * sets the WDT interval to max interval */
void
watchdog_init(void)
{

	PM->APBAMASK.reg |= PM_APBAMASK_WDT;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(WDT_GCLK_ID) |
			GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4);

	WDT->CONFIG.reg = WDT_CONFIG_PER(0xa);
}

/*---------------------------------------------------------------------------*/
/** \brief Starts the WDT in watchdog mode, maximum interval */
void
watchdog_start(void)
{
	while (WDT->STATUS.bit.SYNCBUSY == 1) { ; }
	WDT->CTRL.reg |= WDT_CTRL_WEN;
}
/*---------------------------------------------------------------------------*/
/** \brief Writes the WDT clear sequence. This function assumes that we are
 * in watchdog mode and that interval bits (bits [1:0]) are 00 */
void
watchdog_periodic(void)
{

	while (WDT->STATUS.bit.SYNCBUSY == 1) { ; }

	/* Disable the Watchdog module */
	WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;

}
/*---------------------------------------------------------------------------*/
/** \brief In watchdog mode, the WDT can not be stopped. This function is
 * defined here to satisfy API requirements.
 */
void
watchdog_stop(void)
{
	while (WDT->STATUS.bit.SYNCBUSY == 1) { ; }

	WDT->CTRL.reg &= ~WDT_CTRL_ENABLE;
  return;
}
/*---------------------------------------------------------------------------*/
/** \brief Keeps control until the WDT throws a reset signal */
void
watchdog_reboot(void)
{
  INTERRUPTS_DISABLE();
  while(1);
}

void
WDT_Handler(void) {
	while(1);
}
/**
 * @}
 * @}
 */
