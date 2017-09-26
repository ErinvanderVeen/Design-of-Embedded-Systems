#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

#include <string.h>
#include <errno.h>

RT_TASK interrupt_task;
int i_fd, w_fd;

void button_interrupt(void *arg)
{
	int nr_interrupts, i_value, ret, l_state;
	l_state = 1;
	nr_interrupts = 0;

	while(1) {
		if((ret = read(i_fd, &i_value, sizeof(i_value))) == -1) {
			int errcode = errno;
			printf("ERROR %d : %s\n", errcode, strerror(errcode));
			exit(ret);
		}
		if ((ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, &l_state)) == -1) {
			int errcode = errno;
			printf("ERROR %d : %s\n", errcode, strerror(errcode));
			exit(ret);
		}
		rt_printf("Interupts: %d\n", ++nr_interrupts);
		l_state ^= 1;
	}
}

int main(int argc, char* argv[])
{
	int ret;

	w_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio22",O_WRONLY);
	i_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio23",O_RDONLY);

	int xeno_trigger=GPIO_TRIGGER_EDGE_RISING;
	ret=ioctl(i_fd, GPIO_RTIOC_IRQEN, &xeno_trigger);

	// Create/Start task
	rt_task_create(&interrupt_task, "Peter", 0, 50, 0);
	rt_task_start(&interrupt_task, &button_interrupt, NULL);

	printf("\nType CTRL-C to end this program\n\n" );
	pause();
}
