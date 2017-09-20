#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

RT_TASK interrupt_task;
uint8_t nr_interrupts = 0;
int i_fd;

void count_interrupts(void *arg)
{
	int value, ret;

	while(1) {
		if((ret = read(i_fd, &value, sizeof(value))) == -1) {
			exit(ret);
		}
		nr_interrupts++;
		rt_printf("Interrupts: %d\nValue: %d\n", nr_interrupts, value);
	}
}

int main(int argc, char* argv[])
{
	int w_fd, value, ret;
	value = 1;

	w_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio22",O_WRONLY);
	i_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio24",O_RDONLY);

	int xeno_trigger=GPIO_TRIGGER_EDGE_FALLING|GPIO_TRIGGER_EDGE_RISING;
	ret=ioctl(i_fd, GPIO_RTIOC_IRQEN, &xeno_trigger);

	// Create/Start task
	rt_task_create(&interrupt_task, "Peter", 0, 50, 0);
	rt_task_start(&interrupt_task, &count_interrupts, NULL);

	while(1) {
		usleep(500000);
		if ((ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, &value)) == -1) {
			return ret;
		}
		value ^= 1;
	}
}
