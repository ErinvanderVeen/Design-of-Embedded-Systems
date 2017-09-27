#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

#include <string.h>
#include <errno.h>

RT_TASK task_interrupt;
int w_fd, i_fd, ret, i_value;
int val_up = 1;
int val_down = 0;

void interrupt_void(void *arg)
{
	while(1) {
		ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, &val_up);
		ret = read(i_fd, &i_value, sizeof(i_value));
		ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, val_down);
	}
}

int main(int argc, char* argv[])
{
	w_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio4",O_WRONLY);
	i_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio13",O_RDONLY);

	int xeno_trigger=GPIO_TRIGGER_EDGE_FALLING;
	ret = ioctl(i_fd, GPIO_RTIOC_IRQEN, &xeno_trigger);

	// Create/Start tasks
	rt_task_create(&task_interrupt, "Camil", 0, 50, 0);
	rt_task_start(&task_interrupt, &interrupt_void, NULL);

	printf("\nType CTRL-C to end this program\n\n" );
	pause();
}
