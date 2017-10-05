#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

RT_TASK* interrupt_handler_task;

void interrupt_handler(void *arg)
{
	rt_printf("Test\n");
}

int create_interrupt_handler_task() {
	interrupt_handler_task = malloc(sizeof(RT_TASK));
	return (int) interrupt_handler_task;
}

int get_interrupt_handler_void() {
	return (int) &interrupt_handler;
}

int get_interrupt_handler_task() {
	return (int) interrupt_handler_task;
}
