
#include "contiki.h"
#include "board.h"
//#include "gpio.h"
#include "dev/button-sensor.h"
#include "sys/timer.h"

#include <stdint.h>
#include <string.h>

#define BUTTON_SELECT_PORT_BASE  GPIO_PORT_TO_BASE(BUTTON_PORT)
#define BUTTON_SELECT_PIN_MASK   GPIO_PIN_MASK(BUTTON_PIN)

/*---------------------------------------------------------------------------*/
static struct timer debouncetimer;
/*---------------------------------------------------------------------------*/
/**
 * \brief Common initialiser for all buttons
 * \param port_base GPIO port's register offset
 * \param pin_mask Pin mask corresponding to the button's pin
 */
static void
config(void *port_base, uint32_t pin_mask)
{

}
/*---------------------------------------------------------------------------*/
/**
 * \brief Callback registered with the GPIO module. Gets fired with a button
 * port/pin generates an interrupt
 * \param port The port number that generated the interrupt
 * \param pin The pin number that generated the interrupt. This is the pin
 * absolute number (i.e. 0, 1, ..., 7), not a mask
 */
static void
btn_callback(uint8_t port, uint8_t pin)
{

}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the select button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_select(int type, int value)
{

}

/*---------------------------------------------------------------------------*/
void
button_sensor_init()
{

}
/*---------------------------------------------------------------------------*/

SENSORS_SENSOR(button_select_sensor, BUTTON_SENSOR, NULL, config_select, NULL);


/** @} */
