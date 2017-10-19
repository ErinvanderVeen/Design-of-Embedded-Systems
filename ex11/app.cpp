#include "app.hpp"

#include <iterator>
#include <list>

struct SharedMemory {
	uint8_t yellow;
	uint8_t blue;
	uint8_t red;
	FILE* bt_con;
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
		RobotAction(SharedMemory* sm, SensorData* sd, Sensors* s) {
			shared_memory = sm;
			sensor_data = sd;
			sensors = s;
		}
		// Be careful with PASS! Sensor Data is not updated for the next action
		enum Control { SKIP, BLOCK, PASS };
		virtual ID taskID() = 0;
		virtual Control takeControl() = 0;
		virtual void perform() = 0;
		virtual void printName() = 0;

		SharedMemory* shared_memory;
		SensorData* sensor_data;
		Sensors* sensors;
};

class WalkAction : public RobotAction {
	public:
	WalkAction(SharedMemory* sm, SensorData* sd, Sensors* s)
			: RobotAction(sm, sd, s) {}

		ID taskID() {
			return WALK_TASK;
		}

		Control takeControl() {
			return PASS;
		}

		void perform() {
			ev3_motor_set_power(RobotAction::sensors->LEFT_P, 20);
			ev3_motor_set_power(RobotAction::sensors->RIGHT_P, 20);
			wakeup(MAIN_TASK);
		}

		void printName() {
			//cycle_print((char*)"Walk");
		}
};

class ColorAction : public RobotAction {
	public:
		ColorAction(SharedMemory* sm, SensorData* sd, Sensors* s)
			: RobotAction(sm, sd, s) {}

		ID taskID() {
			return COLOR_TASK;
		}

		Control takeControl() {
			if(RobotAction::sensor_data->color != COLOR_WHITE
					&& RobotAction::sensor_data->color != prev_color) {
				prev_color = RobotAction::sensor_data->color;
				return PASS; // We should still walk after detecting a color
			}
			return SKIP;
		}

		void perform() {
			switch(RobotAction::sensor_data->color) {
				case COLOR_YELLOW:
					RobotAction::shared_memory->yellow = 1;
					cycle_print((char*)"Saw Yellow!");
					fprintf(RobotAction::shared_memory->bt_con, "y\n");
					break;
				case COLOR_BLUE:
					RobotAction::shared_memory->blue = 1;
					cycle_print((char*)"Saw Blue!");
					fprintf(RobotAction::shared_memory->bt_con, "b\n");
					break;
				case COLOR_RED:
					RobotAction::shared_memory->red = 1;
					cycle_print((char*)"Saw Red!");
					fprintf(RobotAction::shared_memory->bt_con, "r\n");
					break;
				default:
					break;
			}
			if (RobotAction::shared_memory->yellow == 1
					&& RobotAction::shared_memory->blue == 1
					&& RobotAction::shared_memory->red == 1)
				act_tsk(FIN_TASK);
			wakeup(MAIN_TASK);
		}

		void printName() {
			//cycle_print((char*)"Color");
		}

	private:
		colorid_t prev_color;

};

class AvoidAction : public RobotAction {
	public:
		AvoidAction(SharedMemory* sm, SensorData* sd, Sensors* s)
			: RobotAction(sm, sd, s) {}

		ID taskID() {
			return AVOID_TASK;
		}

		Control takeControl() {
			if(RobotAction::sensor_data->color == COLOR_BLACK
					|| RobotAction::sensor_data->ultrasonic <= 30
					|| RobotAction::sensor_data->touch_left == 1
					|| RobotAction::sensor_data->touch_right == 1)
				return BLOCK;
			return SKIP;
		}

		void perform() {
			ev3_motor_set_power(RobotAction::sensors->LEFT_P, -20);
			ev3_motor_set_power(RobotAction::sensors->RIGHT_P, -40);
			sleep(1000);
			ev3_motor_stop(RobotAction::sensors->LEFT_P, true);
			ev3_motor_stop(RobotAction::sensors->RIGHT_P, true);
			wakeup(MAIN_TASK);
		}

		void printName() {
			//cycle_print((char*)"Avoid");
		}
};

class Arbitrator {
	public:
		Arbitrator(SensorData*, Sensors*);
		void start();
		void apply(RobotAction*);

	private:
		void assess_actions(RobotAction::Control& control);
		std::list<RobotAction*> actions;
		SensorData* sensor_data;
		Sensors* sensors;
		void update_sensor_data();
};

Arbitrator::Arbitrator(SensorData* sensor_data, Sensors* sensors) {
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
		control = action->takeControl();

		switch (control) {
			case RobotAction::SKIP:
				// Do nothing
				break;
			case RobotAction::BLOCK:
				// Take action and stop loop
				wakeup(action->taskID());
				//cycle_print((char*)"WAKING UP A TASK");
				slp_tsk();
				return;
			case RobotAction::PASS:
				// Take action and continue to the next potential action
				wakeup(action->taskID());
				//cycle_print((char*)"WAKING UP A TASK");
				slp_tsk();
				break;
		}
	}
}

void Arbitrator::apply(RobotAction* mode) {
	actions.push_back(mode);
}

void Arbitrator::update_sensor_data() {
	sensor_data->touch_left = ev3_touch_sensor_is_pressed(sensors->TLEFT_P);
	sensor_data->touch_right = ev3_touch_sensor_is_pressed(sensors->TRIGHT_P);
	sensor_data->color = ev3_color_sensor_get_color(sensors->COLOR_P);
	sensor_data->ultrasonic = ev3_ultrasonic_sensor_get_distance(sensors->ULTRA_P);
}
int32_t FONT_WIDTH, FONT_HEIGHT, NLINES;
uint8_t slave_address[6] = { 0x00, 0x17, 0xE9, 0xB2, 0x56, 0x99 };
const char* pin = "0000";
bool_t  is_master = false;

static int line = 0;

AvoidAction* aa;
ColorAction* ca;
WalkAction* wa;

SharedMemory* shared_memory;
SensorData* sensor_data;
Sensors* sensors;

bool_t isConnected() {
	T_SERIAL_RPOR rpor;
	ER ercd = serial_ref_por(SIO_PORT_SPP_MASTER_TEST, &rpor);
	return ercd == E_OK;
}

void btConnect() {
	while(true) {
		if (is_master) {
			shared_memory->bt_con = fdopen(SIO_PORT_SPP_MASTER_TEST_FILENO, "a+");
			if (shared_memory->bt_con != NULL) {
				setbuf(shared_memory->bt_con, NULL);
				while (!isConnected()) {
					//cycle_print((char*)"Connecting...");
					spp_master_test_connect(slave_address, pin);
					sleep(1000);
				}
				break;
			}
		} else {
			while (!ev3_bluetooth_is_connected()) {
				//cycle_print((char*)"Waiting for connection...");
				sleep(1000);
			}
			shared_memory->bt_con = ev3_serial_open_file(EV3_SERIAL_BT);
			break;
		}
		sleep(1000);
	}
	//cycle_print((char*)"Connected.");
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
	NLINES = 8;

	*sensors = {
		.TLEFT_P = EV3_PORT_1,
		.COLOR_P = EV3_PORT_2,
		.ULTRA_P = EV3_PORT_3,
		.TRIGHT_P = EV3_PORT_4,

		.LEFT_P = EV3_PORT_A,
		.RIGHT_P = EV3_PORT_D
	};

	shared_memory->yellow = 0;
	shared_memory->blue = 0;
	shared_memory->red = 0;

	set_font(EV3_FONT_MEDIUM);
	//	Motor init
	ev3_motor_config(sensors->LEFT_P, LARGE_MOTOR);
	ev3_motor_config(sensors->RIGHT_P, LARGE_MOTOR);
	//	Sensor init
	ev3_sensor_config(sensors->ULTRA_P, ULTRASONIC_SENSOR);
	ev3_sensor_config(sensors->COLOR_P, COLOR_SENSOR);
	ev3_sensor_config(sensors->TLEFT_P, TOUCH_SENSOR);
	ev3_sensor_config(sensors->TRIGHT_P, TOUCH_SENSOR);

	btConnect();
}

void main_task(intptr_t unused) {

	shared_memory = new SharedMemory();
	sensor_data = new SensorData();
	sensors = new Sensors();

	aa = new AvoidAction(shared_memory, sensor_data, sensors);
	ca = new ColorAction(shared_memory, sensor_data, sensors);
	wa = new WalkAction(shared_memory, sensor_data, sensors);

	init();

	Arbitrator arbitrator(sensor_data, sensors);

	// Start the tasks, will sleep imidiatly
	act_tsk(aa->taskID());
	act_tsk(ca->taskID());
	act_tsk(wa->taskID());

	arbitrator.apply(aa);
	arbitrator.apply(ca);
	arbitrator.apply(wa);

	arbitrator.start();
	return;
}

void fin_task(intptr_t unused) {
	ter_tsk(BT_RECV_TASK);
	ter_tsk(MAIN_TASK);

	ter_tsk(AVOID_TASK);
	ter_tsk(COLOR_TASK);
	ter_tsk(WALK_TASK);

	ev3_motor_stop(sensors->LEFT_P, true);
	ev3_motor_stop(sensors->RIGHT_P, true);
}

void bt_recv_task(intptr_t unused) {
    static char buf[3];
	while ((fgets(buf, 3, shared_memory->bt_con))) {
		switch(buf) {
			case "y":
				shared_memory->yellow = 1;
				break;
			case "b":
				shared_memory->blue = 1;
				break;
			case "r":
				shared_memory->red = 1;
				break;
			default:
				break;
		}
		cycle_print(buf);
		if (shared_memory->yellow == 1
				&& shared_memory->blue == 1
				&& shared_memory->red == 1)
			act_tsk(FIN_TASK);
		//cycle_print((char*)"Bluetooth Task");
		sleep(500);
	}
}

void avoid_task(intptr_t){
	while(1) {
		//cycle_print((char*)"Sleep Avoid");
		slp_tsk();
		aa->perform();
	}
}

void color_task(intptr_t){
	while(1) {
		//cycle_print((char*)"Sleep Color");
		slp_tsk();
		ca->perform();
	}
}

void walk_task(intptr_t){
	while(1) {
		//cycle_print((char*)"Sleep Walk");
		slp_tsk();
		wa->perform();
	}
}
