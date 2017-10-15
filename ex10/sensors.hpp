#ifndef SENSORS_H
#define SENSORS_H

#include "ev3api.h"

struct SensorData {
	int touch_left;
	int touch_right;

	colorid_t color;

	int16_t ultrasonic;
};

struct Sensors {
	sensor_port_t TLEFT_P;
	sensor_port_t COLOR_P;
	sensor_port_t ULTRA_P;
	sensor_port_t TRIGHT_P;

	motor_port_t LEFT_P;
	motor_port_t RIGHT_P;
};

#endif
