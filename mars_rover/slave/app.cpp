#include "app.h"

int32_t FONT_WIDTH, FONT_HEIGHT, NLINES;

// Sensor mapping
sensor_port_t
TLEFT_P = EV3_PORT_1, 
COLOR_MID_P = EV3_PORT_2,
ULTRA_FRONT_P = EV3_PORT_3,
TRIGHT_P = EV3_PORT_4;

// Sensor var declaration
bool_t
touch_left = false,
touch_right = false,
do_exit = false;
colorid_t color_mid;
int16_t ultrasonic_front = 0;

//Address needs to be verified
uint8_t slave_address[6] = { 0x00, 0x17, 0xE9, 0xB2, 0x6C, 0x86 };
const char* pin = "0000";
static FILE *bt_con;
bool_t is_master = false;

int red, green, blue; 

int line = 0;

void init() {
    set_font(EV3_FONT_SMALL);
    btConnect();
}

void cycle_print(char* message) {
    int printLine = ++line % NLINES;
    if (line >= NLINES)
        ev3_lcd_clear_line_range(printLine, printLine + 1);
    ev3_print(printLine, message);
}
void read_sensors(int display_line) {
	touch_left = ev3_touch_sensor_is_pressed(TLEFT_P);
	touch_right = ev3_touch_sensor_is_pressed(TRIGHT_P);
	color_mid= ev3_color_sensor_get_color(COLOR_MID_P);
	ultrasonic_front = ev3_ultrasonic_sensor_get_distance(ULTRA_FRONT_P);
}
void config() 
{
    //	Sensor init
	ev3_sensor_config(ULTRA_FRONT_P, ULTRASONIC_SENSOR);
	ev3_sensor_config(COLOR_MID_P, COLOR_SENSOR);
	ev3_sensor_config(TLEFT_P, TOUCH_SENSOR);
    ev3_sensor_config(TRIGHT_P, TOUCH_SENSOR);
    
    red = 0;
    green = 0;
    blue = 0;
}

void detect_colors()
{
    /* 
       Separate if statements that check if one of the three colors is read
       by the robot and the respective color hasn't been found before (equal to 0).
    */    

    if(color_mid == COLOR_WHITE) //white border detected
    {
        ev3_led_set_color(LED_RED);
        fprintf(bt_con,"%s", "w\n"); //notify other robot.
        cycle_print((char*)"White border found!");
        ev3_speaker_play_tone(250, 200);
    }

    if(color_mid== COLOR_RED && !red)
    {
        red = 1;
        //red found
        fprintf(bt_con,"%s", "r\n"); //notify other robot.
        cycle_print((char*)"Red found!");
        ev3_speaker_play_tone(250, 200);
    } 
    if(color_mid== COLOR_GREEN && !green)
    {
        green = 1;
        //green found
        fprintf(bt_con,"%s", "g\n"); //notify other robot.
        cycle_print((char*)"Green found!");
        ev3_speaker_play_tone(250, 200);
    } 
    if(color_mid== COLOR_BLUE && !blue)
    {
        blue = 1;
        //blue found
        fprintf(bt_con,"%s", "b\n"); //notify other robot.
        cycle_print((char*)"Blue found!");
        ev3_speaker_play_tone(250, 200);
    } 
}

void detect_touch()
{
    if(touch_left == 1 || touch_right == 1) //object detected
        {
            ev3_speaker_play_tone(500, 1000);
            ev3_led_set_color(LED_RED);
            fprintf(bt_con,"%s", "t\n"); //notify other robot.
            cycle_print((char*)"Either left or right touch was pressed!");
        }
    }
    
void detect_ultra()
{
     if(ultrasonic_front <=30) //object detected, try avoiding it
        {
            ev3_led_set_color(LED_ORANGE);
            fprintf(bt_con,"%s", "u\n"); //notify other robot.
            cycle_print((char*)"Ultra detected rock in distance!");
        }
}
int is_mission_accomplished()
{
    //Check if all the colors have value 1.
    if(red == 1 && green == 1 && blue == 1) 
    {
    //set led color to green, play tone and print success message.
    ev3_led_set_color(LED_GREEN);
    ev3_speaker_play_tone(250, 1000);
    cycle_print((char*)"Colors found!!!");
    return 1;
    }
    return 0;
}
void close_app(intptr_t btn) {
	do_exit = true;
	ev3_print(5, "Finishing..");
}

void bt_task(intptr_t unused) {

    static char c;
    while ((c = fgetc(bt_con))) {

        // reading from other robot. 
        // keep values of red, green and blue up-to-date. 

        if(!red && c == 'r')
        {
                    red = 1;
                    cycle_print((char*)"Red received!");
                    ev3_speaker_play_tone(250, 200);
        } else if(!green && c == 'g')
        {
                    green = 1;
                    cycle_print((char*)"Green received!");
                    ev3_speaker_play_tone(250, 200);
        } else if(!blue && c == 'b')
        {
                    blue = 1;
                    cycle_print((char*)"Blue received!");
                    ev3_speaker_play_tone(250, 200);
                  
        }
        
        dly_tsk(500);
    }
}

void btConnect() {
    while(true) {
        if (is_master) {
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
        } else {
            while (!ev3_bluetooth_is_connected()) { //check whether Bluetooth SPP is connected
                cycle_print((char*)"Waiting for connection...");
                sleep(1000);
            }
            bt_con = ev3_serial_open_file(EV3_SERIAL_BT); //open serial port as a file
            break;
        }
        sleep(1000);
    }
    cycle_print((char*)"Connected.");
    if (is_master)
    cycle_print((char*)"Master.");
    else
    cycle_print((char*)"Slave");
    act_tsk(BT_TASK);
}

bool_t isConnected() {
    T_SERIAL_RPOR rpor;
    ER ercd = serial_ref_por(SIO_PORT_SPP_MASTER_TEST, &rpor);
    return ercd == E_OK;
}

void set_font(lcdfont_t font) {
    ev3_lcd_set_font(font);
    ev3_font_get_size(font, &FONT_WIDTH, &FONT_HEIGHT);
    NLINES = EV3_LCD_HEIGHT / FONT_HEIGHT;
}


void main_task(intptr_t unused) {
    init();
    config();
    read_sensors(1);
    while(1)
    {
        ev3_led_set_color(LED_GREEN);
       
        
        detect_colors();
        detect_touch();
        detect_ultra();
        
        if(is_mission_accomplished()) break;

        sleep(100);
        read_sensors(1);
    }       
}



