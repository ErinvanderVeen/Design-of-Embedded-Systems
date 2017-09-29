#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

#include <string.h>
#include <errno.h>

#define MAX_COUNTER 10010

RT_TASK task_periodic, task_interrupt;
int i_fd, w_fd, ret, i_value;

uint16_t counter_send = 0;
uint16_t counter_receive = 0;

int val_up = 1;
int val_down = 0;

RTIME array[MAX_COUNTER];

void write_RTIMES(char * filename, unsigned int number_of_values,
		RTIME *time_values){
	unsigned int n=0;
	FILE *file;
	file = fopen(filename,"w");
	while (n<number_of_values) {
		fprintf(file,"%u,%llu\n",n,time_values[n]);  
		n++;
	}
	fclose(file);
}

void periodic_write(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, 10000000);
	while(counter_send < MAX_COUNTER) {
		ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, &val_up);
		rt_task_wait_period(NULL);
		array[counter_send] = rt_timer_read();
		ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, &val_down);
		counter_send++;
	}
}

void interrupt_void(void *arg)
{
	while(counter_receive < MAX_COUNTER) {
		ret = read(i_fd, &i_value, sizeof(i_value));
		array[counter_receive] = rt_timer_read() - array[counter_receive];
		counter_receive++;
	}
	rt_printf("DONE\n");
	write_RTIMES("result.csv", MAX_COUNTER, array);
}

int main(int argc, char* argv[])
{
	int ret;

	if((w_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio4",O_WRONLY)) == -1) {
		int errcode = errno;
		printf("ERROR %d : %s\n", errcode, strerror(errcode));
		exit(ret);
	}
	i_fd = open("/dev/rtdm/pinctrl-bcm2835/gpio13",O_RDONLY);

	int xeno_trigger=GPIO_TRIGGER_EDGE_FALLING;
	if((ret = ioctl(i_fd, GPIO_RTIOC_IRQEN, &xeno_trigger)) == -1) {
		int errcode = errno;
		printf("ERROR %d : %s\n", errcode, strerror(errcode));
		exit(ret);
	}

	ret = ioctl(w_fd, GPIO_RTIOC_DIR_OUT, &val_up);

	// Create/Start tasks
	rt_task_create(&task_interrupt, "Frouke", 0, 50, 0);
	rt_task_start(&task_interrupt, &interrupt_void, NULL);

	rt_task_create(&task_periodic, "Oussama", 0, 50, 0);
	rt_task_start(&task_periodic, &periodic_write, NULL);

	printf("\nType CTRL-C to end this program\n\n" );
	pause();
}
