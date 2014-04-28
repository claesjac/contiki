#include "contiki.h"

#include "sys/energest.h"
#include "sys/rtimer.h"
#include "cpu.h"



#include <stdint.h>
/*---------------------------------------------------------------------------*/
static volatile rtimer_clock_t next_trigger;
static volatile uint32_t secs;
/*---------------------------------------------------------------------------*/
/**
 * \brief We don't need to explicitly initialise anything but this
 *        routine is required by the API.
 *
 *        The Sleep Timer starts ticking automatically as soon as the device
 *        turns on. We don't need to turn on interrupts before the first call
 *        to rtimer_arch_schedule()
 */
void
rtimer_arch_init(void)
{
	secs = 0;

	 PM->APBCMASK.reg |= PM_APBCMASK_TC4;

	  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC4_GCLK_ID) |
	      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

	  TC4->COUNT16.CTRLA.reg =
	      TC_CTRLA_MODE(TC_CTRLA_MODE_COUNT16_Val) |
	      TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_MFRQ_Val) |
	      TC_CTRLA_PRESCALER(TC_CTRLA_PRESCALER_DIV256) |
	      TC_CTRLA_PRESCSYNC(TC_CTRLA_PRESCSYNC_PRESC_Val);

	  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
	  TC4->COUNT16.COUNT.reg = 0;

	  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
	  TC4->COUNT16.CC[0].reg = RTIMER_ARCH_TOP;

	  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
	  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;

	  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
	  TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC0;

	  NVIC_EnableIRQ(TC4_IRQn);

}
/*---------------------------------------------------------------------------*/
/**
 * \brief Schedules an rtimer task to be triggered at time t
 * \param t The time when the task will need executed. This is an absolute
 *          time, in other words the task will be executed AT time \e t,
 *          not IN \e t ticks
 */
void
rtimer_arch_schedule(rtimer_clock_t t)
{
	  rtimer_clock_t now;

	  INTERRUPTS_DISABLE();

	  now = rtimer_arch_now();
	  /* I don't know if I need to keep this additional time for short schedules */

	  if((int32_t)(t - now) < 7) {
	    t = now + 7;
	  }
	  if (t > RTIMER_ARCH_TOP) {
		  t -= RTIMER_ARCH_TOP;
	  }
	  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
	  TC4->COUNT16.CC[0].reg = t;


	  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
	  TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC1;

	  INTERRUPTS_ENABLE();

	  /* Store the value. The LPM module will query us for it */
	  next_trigger = t;

}
/*---------------------------------------------------------------------------*/
rtimer_clock_t
rtimer_arch_next_trigger()
{
  return next_trigger;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Returns the current real-time clock time
 * \return The current rtimer time in ticks
 */
rtimer_clock_t
rtimer_arch_now()
{
  rtimer_clock_t rv;

  rv = TC4->COUNT16.COUNT.reg;


  return rv;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief The rtimer ISR
 *
 *        Interrupts are only turned on when we have an rtimer task to schedule
 *        Once the interrupt fires, the task is called and then interrupts no
 *        longer get acknowledged until the next task needs scheduled.
 */
void
TC4_Handler()
{

  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if(TC4->COUNT16.INTFLAG.bit.MC0) {
	  secs++;
	  TC4->COUNT16.INTFLAG.bit.MC0 = 1;
  }

  if(TC4->COUNT16.INTFLAG.bit.MC1) {
	  next_trigger = 0;

	  TC4->COUNT16.INTFLAG.bit.MC1 = 1;
	  rtimer_run_next();

	  if(next_trigger == NULL) {
		  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
			  TC4->COUNT16.INTENCLR.reg = TC_INTENCLR_MC1;

	  }
  }
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** @} */
