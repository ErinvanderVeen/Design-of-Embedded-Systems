#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>
#include <wiringPi.h>

RT_TASK* interrupt_handler_task;
int i_fd;
int D0, D1, D2, D3, D4, D5, D6, D7;

int val_down = 0;
int val_up = 1;

int less_than(RTIME* p1, RTIME* p2) {
	return *p1 < *p2;
}

RTIME* subtract(RTIME* p1, RTIME* p2) {
	*p1 = *p1 - *p2;
	return p1;
}

RTIME* get_timer(RTIME* time) {
	*time = rt_timer_read();
	return time;
}

int led_on(int fd) {
	write(fd,&val_up,sizeof(val_up));
	return fd;
}

int led_off(int fd) {
	write(fd,&val_down,sizeof(val_down));
	return fd;
}

int spin_timer(RTIME* ct) {
	
	SRTIME ns = rt_timer_ticks2ns(*ct);
	rt_timer_spin(ns / 400);
	return 1;
}

int get_fd(int id) {
	switch(id) {
		case 0: return D0; break;
		case 1: return D1; break;
		case 2: return D2; break;
		case 3: return D3; break;
		case 4: return D4; break;
		case 5: return D5; break;
		case 6: return D6; break;
		case 7: return D7; break;
		default: exit(-1);
	}
	// Should not happen
	return 0;
}

int setup_leds() {
	D0 = open("/dev/rtdm/pinctrl-bcm2835/gpio2", O_WRONLY);
	ioctl(D0, GPIO_RTIOC_DIR_OUT, &val_down);

	D1 = open("/dev/rtdm/pinctrl-bcm2835/gpio3", O_WRONLY);
	ioctl(D1, GPIO_RTIOC_DIR_OUT, &val_down);

	D2 = open("/dev/rtdm/pinctrl-bcm2835/gpio4", O_WRONLY);
	ioctl(D2, GPIO_RTIOC_DIR_OUT, &val_down);

	D3 = open("/dev/rtdm/pinctrl-bcm2835/gpio17", O_WRONLY);
	ioctl(D3, GPIO_RTIOC_DIR_OUT, &val_down);

	D4 = open("/dev/rtdm/pinctrl-bcm2835/gpio27", O_WRONLY);
	ioctl(D4, GPIO_RTIOC_DIR_OUT, &val_down);

	D5 = open("/dev/rtdm/pinctrl-bcm2835/gpio22", O_WRONLY);
	ioctl(D5, GPIO_RTIOC_DIR_OUT, &val_down);

	D6 = open("/dev/rtdm/pinctrl-bcm2835/gpio10", O_WRONLY);
	ioctl(D6, GPIO_RTIOC_DIR_OUT, &val_down);

	D7 = open("/dev/rtdm/pinctrl-bcm2835/gpio9", O_WRONLY);
	ioctl(D7, GPIO_RTIOC_DIR_OUT, &val_down);

	return 1;
}

int setup_interrupt_handler() {
	interrupt_handler_task = malloc(sizeof(RT_TASK));

	int PIN = 23;

	wiringPiSetupGpio();

	i_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio23", O_RDONLY);

	int xeno_trigger = GPIO_TRIGGER_EDGE_FALLING;
	ioctl(i_fd, GPIO_RTIOC_IRQEN, &xeno_trigger);

	pullUpDnControl(PIN, PUD_UP);

	return i_fd;
}
