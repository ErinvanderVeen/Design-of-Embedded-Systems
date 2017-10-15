#include "arbitrator.hpp"

Arbitrator::Arbitrator(SensorData& sensor_data, Sensors& sensors) {
	this->sensor_data = sensor_data;
	this->sensor = sensor;
}

void Arbitrator::start() {
	bool running = true;

	RobotAction::Control control;
	while(running) {

		// Read current values from sensors
		// Actions that need access to the data, whould have been passed
		// a reference to it in their constructor
		update_sensor_data();

		for (RobotAction* action : actions) {
			control = action->takeControl(sensor_data);

			switch (control) {
				case RobotAction::SKIP:
					// Do nothing
					break;
				case RobotAction::BLOCK:
					// Take action and stop loop
					action->perform(sensor_data);
					return;
				case RobotAction::PASS:
					// Take action and continue to the next potential action
					action->perform(sensor_data);
					break;
			}
		}
	}
	return;
}

void Arbitrator::apply(RobotAction* mode) {
	modes.push_back(mode);
	return;
}

void Arbitrator::update_sensor_data() {
	sensor_data.touch_left = ev3_touch_sensor_is_pressed(TLEFT_P);
	sensor_data.touch_right = ev3_touch_sensor_is_pressed(TRIGHT_P);
	sensor_data.color = ev3_color_sensor_get_color(COLOR_P);
	sensor_data.ultrasonic = ev3_ultrasonic_sensor_get_distance(ULTRA_P);
	return;
}
