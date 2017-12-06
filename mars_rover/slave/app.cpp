#include "app.h"

int32_t FONT_WIDTH, FONT_HEIGHT, NLINES;

// Sensor mapping
sensor_port_t TLEFT_P = EV3_PORT_1;
sensor_port_t COLOR_MID_P = EV3_PORT_2;
sensor_port_t ULTRA_FRONT_P = EV3_PORT_3;
sensor_port_t TRIGHT_P = EV3_PORT_4;

// Sensor var declaration
bool_t touch_left_old;
bool_t touch_right_old;
colorid_t color_mid_old;
int16_t ultrasonic_front_old;

bool_t touch_left_new;
bool_t touch_right_new;
colorid_t color_mid_new;
int16_t ultrasonic_front_new;

// TODO: Address needs to be verified
uint8_t slave_address[6] = { 0x00, 0x17, 0xE9, 0xB2, 0x6C, 0x86 };
const char* pin = "0000";
static FILE *bt_con;

int line = 0;

void config() {
	// Sensor init
	ev3_sensor_config(ULTRA_FRONT_P, ULTRASONIC_SENSOR);
	ev3_sensor_config(COLOR_MID_P, COLOR_SENSOR);
	ev3_sensor_config(TLEFT_P, TOUCH_SENSOR);
	ev3_sensor_config(TRIGHT_P, TOUCH_SENSOR);
}

void cycle_print(char* message) {
	int printLine = ++line % NLINES;
	if (line >= NLINES)
		ev3_lcd_clear_line_range(printLine, printLine + 1);
	ev3_print(printLine, message);
}

void read_sensors() {
	touch_left_new = ev3_touch_sensor_is_pressed(TLEFT_P);
	touch_right_new = ev3_touch_sensor_is_pressed(TRIGHT_P);
	color_mid_new = ev3_color_sensor_get_color(COLOR_MID_P);
	ultrasonic_front_new = ev3_ultrasonic_sensor_get_distance(ULTRA_FRONT_P);
}

void send_data() {
	if(touch_left_old != touch_left_new) {
		fprintf(bt_con, "%s%d\n", "tl", touch_left_new);
		touch_left_old = touch_left_new;
	}

	if(touch_right_old != touch_right_new) {
		fprintf(bt_con, "%s%d\n", "tr", touch_right_new);
		touch_right_old = touch_right_new;
	}

	if(color_mid_old != color_mid_new) {
		fprintf(bt_con, "%s%d\n", "cm", color_mid_new);
		color_mid_old = color_mid_new;
	}

	if(ultrasonic_front_old != ultrasonic_front_new) {
		fprintf(bt_con, "%s%d\n", "uf", ultrasonic_front_new);
		ultrasonic_front_old = ultrasonic_front_new;
	}
}

void btConnect() {
	while(true) {
		while (!ev3_bluetooth_is_connected()) {
			cycle_print((char*)"Waiting for connection...");
			sleep(1000);
		}
		bt_con = ev3_serial_open_file(EV3_SERIAL_BT);
		break;
		sleep(1000);
	}
	cycle_print((char*)"Connected.");
	cycle_print((char*)"Slave");
}

void set_font(lcdfont_t font) {
	ev3_lcd_set_font(font);
	ev3_font_get_size(font, &FONT_WIDTH, &FONT_HEIGHT);
	NLINES = EV3_LCD_HEIGHT / FONT_HEIGHT;
}

void init() {
	set_font(EV3_FONT_SMALL);
	btConnect();
	config();
	read_sensors();

	touch_left_old = touch_left_new;
	touch_right_old = touch_right_new;
	color_mid_old = color_mid_new;
	ultrasonic_front_old = ultrasonic_front_new;
}

void main_task(intptr_t unused) {
	init();
	while(true)
	{
		read_sensors();
		send_data();

		sleep(500);
	}
}
