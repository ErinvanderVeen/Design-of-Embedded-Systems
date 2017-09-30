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

RTIME array[MAX_COUNTER];
RTIME diff_array[MAX_COUNTER];

void write_RTIMES(char * filename, unsigned int number_of_values,
		RTIME *time_values){
	unsigned int n=1;
	FILE *file;
	file = fopen(filename,"w");
	while (n<number_of_values) {
		fprintf(file,"%u,%llu\n",n,time_values[n]);
		n++;
	}
	fclose(file);
}

void calculate_diff() {
	uint16_t i;

	for(i = 1; i < MAX_COUNTER; i++) {
		diff_array[i] = array[i] - array[i - 1];
	}

}

void periodic_write(void *arg)
{
	uint16_t counter = 0;
	rt_task_set_periodic(NULL, TM_NOW, 100000);
	while(counter < MAX_COUNTER) {
		rt_task_wait_period(NULL);
		array[counter] = rt_timer_read();
		counter++;
	}
	calculate_diff();
	write_RTIMES("result.csv", MAX_COUNTER, &diff_array[1]);
	printf("Done\n");
}

int main(int argc, char* argv[])
{
	// Create/Start periodic task
	rt_task_create(&task_periodic, "Frouke", 0, 50, 0);
	rt_task_start(&task_periodic, &periodic_write, NULL);

	printf("\nType CTRL-C to end this program\n\n" );
	pause();
}
