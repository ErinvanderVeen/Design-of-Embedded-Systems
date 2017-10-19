#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "ev3api.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "driver_interface_bluetooth.h"
#include "driver_interface_filesys.h"
#include "syssvc/serial.h"

#ifndef STACK_SIZE
#define STACK_SIZE 4096
#endif

#ifndef TOPPERS_MACRO_ONLY

#define sleep(x) tslp_tsk(1000 * x)
#define wakeup wup_tsk

#define ev3_print(x, s) ev3_lcd_draw_string(s, 0, x * FONT_HEIGHT)

#define ev3_lcd_clear() ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE)
#define ev3_lcd_clear_line(x) ev3_lcd_fill_rect(0, x * FONT_HEIGHT, EV3_LCD_WIDTH, FONT_HEIGHT, EV3_LCD_WHITE)
#define ev3_lcd_clear_line_range(x, y) ev3_lcd_fill_rect(0, x * FONT_HEIGHT, EV3_LCD_WIDTH, (y - x) * FONT_HEIGHT, EV3_LCD_WHITE)

/**
 * Default font
 */
extern int32_t FONT_WIDTH;
extern int32_t FONT_HEIGHT;

/**
 * Functions
 */
extern void set_font(lcdfont_t);
extern void init();
extern void btConnect();
extern bool_t isConnected();

extern void cycle_print(char*);

/**
 * Tasks
 * IN ORDER OF PRIORITY
 */
// Kills all other tasks
extern void fin_task(intptr_t);
// Recieves bluetooth communications
extern void bt_recv_task(intptr_t);
// Runs Aribitrator and updates sensors
extern void main_task(intptr_t);

extern void avoid_task(intptr_t);
extern void color_task(intptr_t);
extern void walk_task(intptr_t);

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
