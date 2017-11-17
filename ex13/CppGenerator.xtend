package org.vanderveen.ev3rt.generator

import org.vanderveen.ev3rt.behaviourDSL.Mission

class CppGenerator {
	def static toCpp(Mission mission)'''
		#include "app.hpp"

		#include <iterator>
		#include <list>

		struct SharedMemory {
			// IF NEEDED, this struct will store variables that must be shared between
			// tasks
		};

		struct SensorData {
			int touch_left;
			int touch_right;

			colorid_t color;

			int16_t ultrasonic;
		};

		// Should not be called "Sensors"
		struct Sensors {
			sensor_port_t TLEFT_P;
			sensor_port_t COLOR_P;
			sensor_port_t ULTRA_P;
			sensor_port_t TRIGHT_P;

			motor_port_t LEFT_P;
			motor_port_t RIGHT_P;
		};

		class RobotAction {
			public:
				// Be careful with PASS! Sensor Data is not updated for the next action
				enum Control { SKIP, BLOCK, PASS };
				virtual Control takeControl(SensorData&) = 0;
				virtual void perform(SensorData&, Sensors&) = 0;
				virtual void printName() = 0;
		};

		class WalkAction : public RobotAction {
			public:
				Control takeControl(SensorData& sensor_data) {
					return PASS;
				}

				void perform(SensorData& sensor_data, Sensors& sensors) {
					ev3_motor_set_power(sensors.LEFT_P, 20);
					ev3_motor_set_power(sensors.RIGHT_P, 20);
					sleep(50);
				}

				void printName() {
					ev3_print(0, "Walk");
				}
		};

		class Arbitrator {
			public:
				Arbitrator(SensorData& sensor_data, Sensors& sensors) {
					this->sensor_data = sensor_data;
					this->sensors = sensors;
				}
				void start() {
					bool running = true;

					RobotAction::Control control;

					while(running) {

						// Read current values from sensors
						// Actions that need access to the data, whould have been passed
						// a reference to it in their constructor
						update_sensor_data();

						assess_actions(control);

					}
					return;
				}

				void apply(RobotAction* mode) {
					actions.push_back(mode);
					return;
				}

			private:
				std::list<RobotAction*> actions;
				SensorData sensor_data;
				Sensors sensors;

				void assess_actions(RobotAction::Control& control) {
					for (RobotAction* action : actions) {
						action->printName();
						control = action->takeControl(sensor_data);

						switch (control) {
							case RobotAction::SKIP:
								// Do nothing
								break;
							case RobotAction::BLOCK:
								// Take action and stop loop
								action->perform(sensor_data, sensors);
								return;
						}
					}
				}
				void update_sensor_data() {
					sensor_data.touch_left = ev3_touch_sensor_is_pressed(sensors.TLEFT_P);
					sensor_data.touch_right = ev3_touch_sensor_is_pressed(sensors.TRIGHT_P);
					sensor_data.color = ev3_color_sensor_get_color(sensors.COLOR_P);
					sensor_data.ultrasonic = ev3_ultrasonic_sensor_get_distance(sensors.ULTRA_P);
					return;
				}
		};


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
			WalkAction wa;

			arbitrator.apply(&wa);

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
	'''
}
