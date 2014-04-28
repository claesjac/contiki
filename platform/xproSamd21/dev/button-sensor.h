
#ifndef BUTTON_SENSOR_H_
#define BUTTON_SENSOR_H_

#include "lib/sensors.h"
#include "board.h"

#define BUTTON_SENSOR "Button"

#define button_sensor button_select_sensor
extern const struct sensors_sensor button_select_sensor;
extern const struct sensors_sensor button_left_sensor;
extern const struct sensors_sensor button_right_sensor;
extern const struct sensors_sensor button_up_sensor;
extern const struct sensors_sensor button_down_sensor;
/*---------------------------------------------------------------------------*/
#endif /* BUTTON_SENSOR_H_ */

/** \brief Common initialiser for all SmartRF Buttons */
void button_sensor_init();

/**
 * @}
 * @}
 */
