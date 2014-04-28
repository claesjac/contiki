#include "contiki.h"
#include "cpu.h"

uint8_t nested_irq = 0;

__attribute__(( naked )) uint32_t ulPortSetInterruptMask( void )
{
	__asm volatile														\
	(																	\
		"	mrs r0, basepri											\n" \
		"	mov r1, %0												\n"	\
		"	msr basepri, r1											\n" \
		"	bx lr													\n" \
		:: "i" ( 0x07 ) : "r0", "r1"	\
	);

	/* This return will not be reached but is necessary to prevent compiler
	warnings. */
	return 0;
}
/*-----------------------------------------------------------*/

__attribute__(( naked )) void vPortClearInterruptMask( uint32_t ulNewMaskValue )
{
	__asm volatile													\
	(																\
		"	msr basepri, r0										\n"	\
		"	bx lr												\n" \
		:::"r0"														\
	);

	/* Just to avoid compiler warnings. */
	( void ) ulNewMaskValue;
}

/*-----------------------------------------------------------*/

void cpu_enter_critical( void )
{
	ulPortSetInterruptMask();
	nested_irq++;
	__asm volatile( "dsb" );
	__asm volatile( "isb" );
}

void cpu_exit_critical( void )
{
//	configASSERT( uxCriticalNesting ); This might be good in the future for trouble shooting
	nested_irq--;
	if( nested_irq == 0 )
	{
		vPortClearInterruptMask(0);
	}
}


unsigned long __attribute__((naked))
cpu_cpsie(void)
{
  unsigned long ret;

  /* Read PRIMASK and enable interrupts */
  __asm("    mrs     r0, PRIMASK\n"
        "    cpsie   i\n"
        "    bx      lr\n"
        : "=r" (ret));

  /* The inline asm returns, we never reach here.
   * We add a return statement to keep the compiler happy */
  return ret;
}
/*---------------------------------------------------------------------------*/
unsigned long __attribute__((naked))
cpu_cpsid(void)
{
  unsigned long ret;

  /* Read PRIMASK and disable interrupts */
  __asm("    mrs     r0, PRIMASK\n"
        "    cpsid   i\n"
        "    bx      lr\n"
        : "=r" (ret));

  /* The inline asm returns, we never reach here.
   * We add a return statement to keep the compiler happy */
  return ret;
}
/*---------------------------------------------------------------------------*/

/** @} */
