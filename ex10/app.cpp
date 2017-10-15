#include "app.hpp"

int32_t FONT_WIDTH, FONT_HEIGHT;

SharedMemory shared_memory;
SensorData sensor_data;
Sensors sensors;

void set_font(lcdfont_t font) {
	ev3_lcd_set_font(font);
	ev3_font_get_size(font, &FONT_WIDTH, &FONT_HEIGHT);
}

void main_task(intptr_t unused) {
	init();

	Arbitrator arbitrator(sensor_data, sensors);

	// TODO: Create RobotActions and apply to arbitrator

	arbitrator.start();
	return;
}

void init() {
	sensors = {
		.TLEFT_P = EV3_PORT_1,
		.COLOR_P = EV3_PORT_2,
		.ULTRA_P = EV3_PORT_3,
		.TRIGHT_P = EV3_PORT_4,

		.LEFT_P = EV3_PORT_A,
		.RIGHT_P = EV3_PORT_D
	};

	set_font(EV3_FONT_MEDIUM);
//	Motor init
	ev3_motor_config(sensors.LEFT_P, LARGE_MOTOR);
	ev3_motor_config(sensors.RIGHT_P, LARGE_MOTOR);
//	Sensor init
	ev3_sensor_config(sensors.ULTRA_P, ULTRASONIC_SENSOR);
	ev3_sensor_config(sensors.COLOR_P, COLOR_SENSOR);
	ev3_sensor_config(sensors.TLEFT_P, TOUCH_SENSOR);
	ev3_sensor_config(sensors.TRIGHT_P, TOUCH_SENSOR);
}
