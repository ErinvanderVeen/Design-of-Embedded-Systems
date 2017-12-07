package org.vanderveen.ev3rt.generator

import org.vanderveen.ev3rt.behaviourDSL.Mission
import org.vanderveen.ev3rt.behaviourDSL.Condition
import org.vanderveen.ev3rt.behaviourDSL.Behaviour
import org.vanderveen.ev3rt.behaviourDSL.Operator
import org.vanderveen.ev3rt.behaviourDSL.Sensor
import org.vanderveen.ev3rt.behaviourDSL.Value
import org.vanderveen.ev3rt.behaviourDSL.PressedState
import org.vanderveen.ev3rt.behaviourDSL.Color
import org.vanderveen.ev3rt.behaviourDSL.Distance
import org.vanderveen.ev3rt.behaviourDSL.TouchValue
import org.vanderveen.ev3rt.behaviourDSL.ColorValue
import org.vanderveen.ev3rt.behaviourDSL.Move
import org.vanderveen.ev3rt.behaviourDSL.Turn
import org.vanderveen.ev3rt.behaviourDSL.Direction

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
			// MASTER
			colorid_t color_left;
			int16_t ultra_back;
			int16_t gyro_angle;
			int16_t gyro_rate;
			colorid_t color_right;

			// SLAVE
			int touch_left;
			colorid_t color_center;
			int16_t ultra_front;
			int touch_right;
		};

		// Should not be called "Sensors"
		struct Sensors {
			sensor_port_t COLORL_P;
			sensor_port_t ULTRAB_P;
			sensor_port_t GYRO_P;
			sensor_port_t COLORR_P;

			motor_port_t LEFT_P;

			motor_port_t ARM_P;
			motor_port_t RIGHT_P;
		};

		SharedMemory shared_memory;
		SensorData sensor_data;
		Sensors sensors;

		uint8_t slave_address[6] = { 0x00, 0x17, 0xE9, 0xB2, 0x6C, 0x86 };
		const char* pin = "0000";
		static FILE *bt_con;

		class RobotAction {
			public:
				// Be careful with PASS! Sensor Data is not updated for the next action
				enum Control { SKIP, BLOCK, PASS };
				virtual Control takeControl() = 0;
				virtual void perform() = 0;
				virtual void printName() = 0;
				
		};

		RobotAction* current_action;

		«FOR behavior : mission.behaviours»
				«fromBehavior(behavior)»;
		«ENDFOR»

		class Arbitrator {
			public:
				Arbitrator() {}

				void start() {
					bool running = true;

					RobotAction::Control control;

					while(running) {

						// Read current values from sensors
						// Actions that need access to the data, whould have been passed
						// a reference to it in their constructor
						update_sensor_data();

						assess_actions(control);

						// Sleep to allow execution of behavior
						tslp_tsk(100);
					}
					return;
				}

				void apply(RobotAction* mode) {
					actions.push_back(mode);
					return;
				}

			private:
				std::list<RobotAction*> actions;
				void restart_behavior_task() {
					// Kill and restart task
					ter_tsk(BEHAVIOR_TASK);
					act_tsk(BEHAVIOR_TASK);
				}

				void assess_actions(RobotAction::Control& control) {
					for (RobotAction* action : actions) {
						action->printName();
						control = action->takeControl();

						switch (control) {
							case RobotAction::SKIP:
								// Do nothing
								break;
							case RobotAction::BLOCK:
								current_action = action;
								restart_behavior_task();
								return;
							case RobotAction::PASS:
								current_action = action;
								restart_behavior_task();
								return;
								// TODO: SLEEP, BREAK
						}
					}
				}
				void update_sensor_data() {
					sensor_data.color_left = ev3_color_sensor_get_color(sensors.COLORL_P);
					sensor_data.ultra_back = ev3_ultrasonic_sensor_get_distance(sensors.ULTRAB_P);
					sensor_data.gyro_angle = ev3_gyro_sensor_get_angle(sensors.GYRO_P);
					sensor_data.gyro_rate = ev3_gyro_sensor_get_rate(sensors.GYRO_P);
					sensor_data.color_right = ev3_color_sensor_get_color(sensors.COLORR_P);

					update_bt_sensors();

					return;
				}

				void update_bt_sensors() {
					fscanf(bt_con, "tl%d\n", &sensor_data.touch_left);
					fscanf(bt_con, "tr%d\n", &sensor_data.touch_right);
					fscanf(bt_con, "cm%d\n", &sensor_data.color_center);
					fscanf(bt_con, "uf%d\n", &sensor_data.color_front);
				}
		};

		int32_t FONT_WIDTH, FONT_HEIGHT;

		void set_font(lcdfont_t font) {
			ev3_lcd_set_font(font);
			ev3_font_get_size(font, &FONT_WIDTH, &FONT_HEIGHT);
		}

		bool_t isConnected() {
			T_SERIAL_RPOR rpor;
			ER ercd = serial_ref_por(SIO_PORT_SPP_MASTER_TEST, &rpor);
			return ercd == E_OK;
		}

		void btConnect() {
			while(true) {
				bt_con = fdopen(SIO_PORT_SPP_MASTER_TEST_FILENO, "a+");
				//open file for update(read & write), append mode
				if (bt_con != NULL) {
					setbuf(bt_con, NULL);
					while (!isConnected()) {
						cycle_print((char*)"Connecting...");
						spp_master_test_connect(slave_address, pin);
						sleep(1000);
					}
					break;
				}
				sleep(1000);
			}
		}

		void init() {
			btConnect();

			sensors = {
				.COLORL_P = EV_PORT_1;
				.ULTRAB_P = EV3_PORT_2;
				.GYRO_P   = EV3_PORT_3;
				.COLORR_P = EV3_PORT_4;

				.LEFT_P  = EV3_PORT_A;

				.ARM_P   = EV3_PORT_C;
				.RIGHT_P = EV3_PORT_D;
			};

			set_font(EV3_FONT_MEDIUM);

			//	Sensor init
			ev3_sensor_config(sensors.COLORL_P, COLOR_SENSOR);
			ev3_sensor_config(sensors.ULTRAB_P, ULTRASONIC_SENSOR);
			ev3_sensor_config(sensors.GYRO_P, GYRO_SENSOR);
			ev3_sensor_config(sensors.COLORR_P, COLOR_SENSOR);

			//	Motor init
			ev3_motor_config(sensors.LEFT_P, LARGE_MOTOR);

			ev3_motor_config(sensors.ARM_P, MEDIUM_MOTOR);
			ev3_motor_config(sensors.RIGHT_P, LARGE_MOTOR);
		}

		void arbitrator_task(intptr_t unused) {
			init();

			Arbitrator* arbitrator = new Arbitrator;

			// TODO: Create RobotActions and apply to arbitrator
			«FOR behavior : mission.behaviours»
				arbitrator->apply(new «behavior.name»());
			«ENDFOR»


			arbitrator->start();
		}

		void behavior_task(intptr_t unused) {
			current_action->perform();
		}

	'''
	
	def static fromBehavior(Behaviour behaviour)'''
		class «behaviour.name» : public RobotAction {
					public:
						Control takeControl() {
							«IF !behaviour.conditions.isEmpty»
							if(«FOR condition : behaviour.conditions SEPARATOR " && "»
								«fromCondition(condition)»
							«ENDFOR»)
							«ENDIF»
								return BLOCK;
							return SKIP;
						}
						
						void perform() {
							«FOR action : behaviour.actions»
								«fromAction(action)»
							«ENDFOR»
						}
						
						void printName() {
							ev3_print(0, "«behaviour.name»");
						}
				}'''
	
	def static dispatch fromAction(Move move)'''
			ev3_motor_set_power(sensors.LEFT_P, «IF move.direction == Direction::FORWARD»20«ENDIF»«IF move.direction == Direction::BACKWARD»-20«ENDIF»);
			ev3_motor_set_power(sensors.RIGHT_P, «IF move.direction == Direction::FORWARD»20«ENDIF»«IF move.direction == Direction::BACKWARD»-20«ENDIF»);
			sleep(«move.duration.time»);
	'''
	
	def static dispatch fromAction(Turn turn) '''
			ev3_motor_set_power(sensors.LEFT_P, «IF turn.direction == Direction::LEFT»-20«ENDIF»«IF turn.direction == Direction::RIGHT»20«ENDIF»);
			ev3_motor_set_power(sensors.RIGHT_P, «IF turn.direction == Direction::LEFT»20«ENDIF»«IF turn.direction == Direction::RIGHT»-20«ENDIF»);
			sleep(«turn.duration.time»);
	'''
	
	def static fromCondition(Condition condition)'''
		«fromSensor(condition.sensor)» «fromOperator(condition.operator)» «fromValue(condition.value)»
	'''
	
	def static fromSensor(Sensor sensor) {
		switch(sensor) {
			case Sensor::TOUCH_L: return '''sensor_data.touch_left'''
			case Sensor::TOUCH_R: return '''sensor_data.touch_right'''
			case Sensor::COLOR: return '''sensor_data.color'''
			case Sensor::SONIC: return '''sensor_data.ultrasonic'''
			default: throw new UnsupportedOperationException("Unsupported Sensor")
		}		
	}
	
	def static fromOperator(Operator operator) {
		switch(operator) {
			case Operator::EQ: return '''=='''	
			case Operator::NEQ: return '''!='''
			case Operator::GT: return '''>'''
			case Operator::LT: return '''<'''
			case Operator::GEQ: return '''>='''
			case Operator::LEQ: return '''<='''
			default: throw new UnsupportedOperationException("Unsupported Operator")
		}
	}
	
	def static dispatch fromValue(TouchValue state) {
		switch(state.pressed) {
			case PressedState::PRESSED:
				return '''1'''
			case PressedState::UNPRESSED:
				return '''0'''
			default:
				throw new UnsupportedOperationException("Unsupported Pressed State")
		}
	}
	
	def static dispatch fromValue(ColorValue color) {
		switch(color.color) {
			case BLACK:
				return '''COLOR_BLACK'''
			case BLUE:
				return '''COLOR_BLUE'''
			case RED:
				return '''COLOR_RED'''
			case WHITE:
				return '''COLOR_WHITE'''
			case YELLOW:
				return '''COLOR_YELLOW'''
			default:
				throw new UnsupportedOperationException("Unsupported Color")
			
		}
	}
	
	def static dispatch fromValue(Distance dist)'''
		«dist.cm»
	'''
	
}
