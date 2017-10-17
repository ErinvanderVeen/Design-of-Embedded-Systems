#include "app.h"

int32_t FONT_WIDTH, FONT_HEIGHT;

// Speed settings
uint32_t
	DRIVE_SPEED = 30,
	SPECIAL_SPEED = 15;

// Sensor mapping
sensor_port_t
  TLEFT_P = EV3_PORT_1, 
  COLOR_P = EV3_PORT_2,
  ULTRA_P = EV3_PORT_3,
  TRIGHT_P = EV3_PORT_4;
    
// Motor mapping
motor_port_t
	LEFT_P = EV3_PORT_A,
	RIGHT_P = EV3_PORT_D;

bool_t
	touch_left = false,
	touch_right = false,
	do_exit = false;
    colorid_t color;
    int16_t ultrasonic = 0;

void set_font(lcdfont_t font) {
	ev3_lcd_set_font(font);
	ev3_font_get_size(font, &FONT_WIDTH, &FONT_HEIGHT);
}

void main_task(intptr_t unused) {
	init();
	read_sensors(1);
	while(true) {
		if (do_exit)
			break;
	ev3_led_set_color(LED_GREEN);
    ev3_motor_set_power(LEFT_P, DRIVE_SPEED);
    ev3_motor_set_power(RIGHT_P, DRIVE_SPEED);
    if(color == 1){ // black border detected
    	ev3_led_set_color(LED_RED);
    	avoid();
    }
    if(touch_left == 1 || touch_right == 1) //object detected
    {
    	ev3_speaker_play_tone(500, 1000);
    	ev3_led_set_color(LED_RED);
    	avoid();
    }
    if(ultrasonic <=30){
    	ev3_speaker_play_tone(250, 500);
    	ev3_led_set_color(LED_ORANGE);
    	avoid();
    }
    sleep(100);
    read_sensors(1);
	}
	// stop and close
	ev3_motor_stop(LEFT_P, true);
	ev3_motor_stop(RIGHT_P, true);
	ev3_lcd_clear_line(5);
	ev3_print(5, "Finished!");
}

void avoid()
{
	//stop motors
	ev3_motor_stop(LEFT_P, true);
	ev3_motor_stop(RIGHT_P, true);
	//move backwards with 180 angle to the right
	ev3_motor_rotate(RIGHT_P, 180, -30, true);
}

void init() {
	set_font(EV3_FONT_MEDIUM);
//	Motor init
	ev3_motor_config(LEFT_P, LARGE_MOTOR);
	ev3_motor_config(RIGHT_P, LARGE_MOTOR);
//	Sensor init
	ev3_sensor_config(ULTRA_P, ULTRASONIC_SENSOR);
	ev3_sensor_config(COLOR_P, COLOR_SENSOR);
	ev3_sensor_config(TLEFT_P, TOUCH_SENSOR);
	ev3_sensor_config(TRIGHT_P, TOUCH_SENSOR);
//	Attach exit handler
	ev3_button_set_on_clicked(ENTER_BUTTON, close_app, ENTER_BUTTON);
}

void read_sensors(int display_line) {
	touch_left = ev3_touch_sensor_is_pressed(TLEFT_P);
	touch_right = ev3_touch_sensor_is_pressed(TRIGHT_P);
	color = ev3_color_sensor_get_color(COLOR_P);
	ultrasonic = ev3_ultrasonic_sensor_get_distance(ULTRA_P);
	if (display_line >= 0)
		print_sensor_values(display_line);
}

void print_sensor_values(int start_line) {
	char str[100];
	ev3_lcd_clear_line_range(start_line, start_line + 4);
	snprintf(str, 100, "TouchL: %d", touch_left);
	ev3_print(start_line + 0, str);
	snprintf(str, 100, "TouchR: %d", touch_right);
	ev3_print(start_line + 1, str);
	snprintf(str, 100, "Ultra : %d", ultrasonic);
	ev3_print(start_line + 2, str);
	snprintf(str, 100, "Color : %d", color);
	ev3_print(start_line + 3, str);
}

void close_app(intptr_t btn) {
	do_exit = true;
	ev3_print(5, "Finishing..");
}
