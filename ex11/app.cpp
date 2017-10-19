#include "app.hpp"

#include <iterator>
#include <list>

struct SharedMemory {
	// IF NEEDED, this struct will store variables that must be shared between
	// tasks
};

// Holds if the collors are seen
struct Colors {
	uint8_t yellow;
	uint8_t blue;
	uint8_t red;
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
		virtual ID taskID() = 0;
		virtual Control takeControl(SensorData&) = 0;
		virtual void perform(SensorData&, Sensors&) = 0;
		virtual void printName() = 0;
};

class WalkAction : public RobotAction {
	public:
		ID taskID() {
			return WALK_TASK;
		}

		Control takeControl(SensorData& sensor_data) {
			return PASS;
		}

		void perform(SensorData& sensor_data, Sensors& sensors) {
			ev3_motor_set_power(sensors.LEFT_P, 20);
			ev3_motor_set_power(sensors.RIGHT_P, 20);
			sleep(50);
		}

		void printName() {
			cycle_print((char*)"Walk");
		}
};

class ColorAction : public RobotAction {
	public:
		ID taskID() {
			return COLOR_TASK;
		}

		Control takeControl(SensorData& sensor_data) {
			if(sensor_data.color != COLOR_WHITE)
				return PASS; // We should still walk after detecting a color
			return SKIP;
		}

		void perform(SensorData& sensor_data, Sensors& sensors) {
			// TODO: Send color to MASTER
		}

		void printName() {
			cycle_print((char*)"Color");
		}
};

class AvoidAction : public RobotAction {
	public:
		ID taskID() {
			return AVOID_TASK;
		}

		Control takeControl(SensorData& sensor_data) {
			if(sensor_data.color == COLOR_BLACK
					|| sensor_data.ultrasonic <= 30
					|| sensor_data.touch_left == 1
					|| sensor_data.touch_right == 1)
				return BLOCK;
			return SKIP;
		}

		void perform(SensorData& sensor_data, Sensors& sensors) {
			ev3_motor_set_power(sensors.LEFT_P, -20);
			ev3_motor_set_power(sensors.RIGHT_P, -40);
			sleep(1000);
			ev3_motor_stop(sensors.LEFT_P, true);
			ev3_motor_stop(sensors.RIGHT_P, true);
		}

		void printName() {
			cycle_print((char*)"Avoid");
		}
};

class Arbitrator {
	public:
		Arbitrator(SensorData&, Sensors&);
		void start();
		void apply(RobotAction*);

	private:
		void assess_actions(RobotAction::Control& control);
		std::list<RobotAction*> actions;
		SensorData sensor_data;
		Sensors sensors;
		void update_sensor_data();
};

Arbitrator::Arbitrator(SensorData& sensor_data, Sensors& sensors) {
	this->sensor_data = sensor_data;
	this->sensors = sensors;
}

void Arbitrator::start() {
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

void Arbitrator::assess_actions(RobotAction::Control& control) {
	for (RobotAction* action : actions) {
		action->printName();
		control = action->takeControl(sensor_data);

		switch (control) {
			case RobotAction::SKIP:
				// Do nothing
				break;
			case RobotAction::BLOCK:
				// Take action and stop loop
				wup_tsk(action->taskID());
				return;
			case RobotAction::PASS:
				// Take action and continue to the next potential action
				wup_tsk(action->taskID());
				break;
		}
		sleep(50);
	}
}

void Arbitrator::apply(RobotAction* mode) {
	actions.push_back(mode);
}

void Arbitrator::update_sensor_data() {
	sensor_data.touch_left = ev3_touch_sensor_is_pressed(sensors.TLEFT_P);
	sensor_data.touch_right = ev3_touch_sensor_is_pressed(sensors.TRIGHT_P);
	sensor_data.color = ev3_color_sensor_get_color(sensors.COLOR_P);
	sensor_data.ultrasonic = ev3_ultrasonic_sensor_get_distance(sensors.ULTRA_P);
}
int32_t FONT_WIDTH, FONT_HEIGHT, NLINES;
uint8_t slave_address[6] = { 0x00, 0x17, 0xE9, 0xB4, 0xC7, 0x4E };
const char* pin = "0000";
static FILE* bt_con;
bool_t is_master = true;

int line = 0;

SharedMemory shared_memory;
SensorData sensor_data;
Sensors sensors;
Colors colors;

bool_t isConnected() {
	T_SERIAL_RPOR rpor;
	ER ercd = serial_ref_por(SIO_PORT_SPP_MASTER_TEST, &rpor);
	return ercd == E_OK;
}

void btConnect() {
	while(true) {
		if (is_master) {
			bt_con = fdopen(SIO_PORT_SPP_MASTER_TEST_FILENO, "a+");
			if (bt_con != NULL) {
				setbuf(bt_con, NULL);
				while (!isConnected()) {
					cycle_print((char*)"Connecting...");
					spp_master_test_connect(slave_address, pin);
					sleep(1000);
				}
				break;
			}
		} else {
			while (!ev3_bluetooth_is_connected()) {
				cycle_print((char*)"Waiting for connection...");
				sleep(1000);
			}
			bt_con = ev3_serial_open_file(EV3_SERIAL_BT);
			break;
		}
		sleep(1000);
	}
	cycle_print((char*)"Connected.");
	act_tsk(BT_RECV_TASK);
}

void set_font(lcdfont_t font) {
	ev3_lcd_set_font(font);
	ev3_font_get_size(font, &FONT_WIDTH, &FONT_HEIGHT);
}

void cycle_print(char* message) {
	int printLine = ++line % NLINES;
	if (line >= NLINES)
		ev3_lcd_clear_line_range(printLine, printLine + 1);
	ev3_print(printLine, message);
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

	colors = {
		.yellow = 0,
		.blue = 0,
		.red = 0
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

	btConnect();
}

AvoidAction aa;
ColorAction ca;
WalkAction wa;

void main_task(intptr_t unused) {
	init();

	Arbitrator arbitrator(sensor_data, sensors);

	// Start the tasks, will sleep imidiatly
	act_tsk(aa.taskID());
	act_tsk(ca.taskID());
	act_tsk(wa.taskID());

	arbitrator.apply(&aa);
	arbitrator.apply(&ca);
	arbitrator.apply(&wa);

	arbitrator.start();
	return;
}

void bt_recv_task(intptr_t unused) {
	char recv;
	while ((recv = fgetc(bt_con))) {
		switch(recv) {
			case 'y':
				colors.yellow = 1;
				break;
			case 'b':
				colors.blue = 1;
				break;
			case 'r':
				colors.red = 1;
				break;
			default:
				break;
		}
		sleep(500);
	}
}

void avoid_task(intptr_t){
	while(1) {
		slp_tsk();
		aa.perform(sensor_data, sensors);
	}
}

void color_task(intptr_t){
	while(1) {
		slp_tsk();
		ca.perform(sensor_data, sensors);
	}
}

void walk_task(intptr_t){
	while(1) {
		slp_tsk();
		wa.perform(sensor_data, sensors);
	}
}
