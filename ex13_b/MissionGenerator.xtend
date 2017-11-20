package ev3rtLego.generator

import ev3rtLego.taskDSL.Avoid
import ev3rtLego.taskDSL.Color
import ev3rtLego.taskDSL.CompareOperator
import ev3rtLego.taskDSL.Direction
import ev3rtLego.taskDSL.Led
import ev3rtLego.taskDSL.LedColor
import ev3rtLego.taskDSL.Light
import ev3rtLego.taskDSL.Mission
import ev3rtLego.taskDSL.Motor
import ev3rtLego.taskDSL.Sound
import ev3rtLego.taskDSL.State
import ev3rtLego.taskDSL.Touch
import ev3rtLego.taskDSL.Ultrasonic

class MissionGenerator {

	
	def static toCpp(Mission root)'''
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
	    
	    «FOR behaviour: root.behaviours»
	    «toSensor(behaviour.sensors)»
	    {
	    «FOR action: behaviour.actions»
	    «toAction(action)»
	    «ENDFOR»
	    
	    }
	    
	    «ENDFOR»
	  
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
	'''
	
	def static toOperator(CompareOperator operator) {
		switch(operator) {
			
			case CompareOperator::EQ:
			return'''=='''
			
			case CompareOperator::NEQ:
			return'''!='''
			
			case CompareOperator::G:
			return'''>'''
			
			case CompareOperator::L:
			return'''<'''
			
			case CompareOperator::GEQ:
			return'''>='''
			
			case CompareOperator::LEQ:
			return'''<='''
			
			default: return'''=='''
		}
	}
	
	def static toColors(Color color){
		
		switch(color){
		case Color::BLACK:
		return '''COLOR_BLACK'''
		
		case Color::RED:
		return '''COLOR_RED'''
		
		case Color::YELLOW:
		return '''COLOR_YELLOW'''
		
		case Color::BLUE:
		return '''COLOR_BLUE'''
		
		case Color::WHITE:
		return '''COLOR_WHITE'''
		
		default: return '''COLOR_WHITE'''
		}
		}
		
	def static toDirection(Direction direction){
		
		switch(direction){
			
			case Direction::FORWARD:
			return ''''''
			
			case Direction::BACKWARD:
			return '''-'''	
		}
	}
	
	def static toMotor(Motor motor){
		
		switch(motor){
			
			case Motor::LEFT:
			return '''LEFT_P'''
			
			case Motor::RIGHT:
			return '''RIGHT_P'''	
		}
	}
	
	def static toLed(LedColor ledColor){
		switch(ledColor){
			
			case LedColor::GREEN:
			return '''LED_GREEN'''
			
			case LedColor::RED:
			return '''LED_RED'''
			
			case LedColor::ORANGE:
			return '''LED_ORANGE'''
			
			default: return '''LED_GREEN'''
			
		}
	}
	
	def static toState(State state){
		switch(state){
			
			case State::NOTPRESSED:
			return '''0'''
			
			case State::PRESSED:
			return '''1'''
			
			default: return '''0'''
		}
	}
	
	
	def static dispatch toSensor(Touch sensor)
	'''if(touch_left «toOperator(sensor.operator)» «toState(sensor.state)» ||
		touch_right «toOperator(sensor.operator)» «toState(sensor.state)»)'''	
	
	def static dispatch toSensor(Light sensor)
	'''if(color «toOperator(sensor.operator)» «toColors(sensor.color)»)'''	
	
	def static dispatch toSensor(Ultrasonic sensor)
	'''if(ultrasonic «toOperator(sensor.operator)» «sensor.distance»)'''
	
	def static dispatch toAction(Led action)
	'''ev3_led_set_color(«toLed(action.ledColor)»);'''
	
	def static dispatch toAction(Sound action)
	'''ev3_speaker_play_tone(«action.duration», «action.frequency»);'''
	
	def static dispatch toAction(Avoid action)
	'''//stop motors
	   ev3_motor_stop(LEFT_P, true);
	   ev3_motor_stop(RIGHT_P, true);
	   //move backwards with 180 angle to the right
	   ev3_motor_rotate(«toMotor(action.motor)», «action.angle», «toDirection(action.direction)»«action.speed», true);'''
		
}