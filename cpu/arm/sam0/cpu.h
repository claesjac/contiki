
#ifndef CPU_H_
#define CPU_H_

/** \brief Disables all CPU interrupts */
unsigned long cpu_cpsid(void);

/** \brief Enables all CPU interrupts */
unsigned long cpu_cpsie(void);

/** \brief Enables all CPU interrupts */
#define INTERRUPTS_ENTER_CRITICAL()  cpu_enter_critical()
//#define INTERRUPTS_ENABLE()  cpu_enter_critical()
#define INTERRUPTS_ENABLE()  cpu_cpsie()
/** \brief Disables all CPU interrupts. */
//#define INTERRUPTS_DISABLE() cpu_exit_critical()
#define INTERRUPTS_DISABLE() cpu_cpsid()
#endif /* CPU_H_ */

/**
 * @}
 * @}
 */
