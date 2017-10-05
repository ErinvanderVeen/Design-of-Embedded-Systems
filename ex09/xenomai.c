#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

extern int add(int, int);

void add_task_f(void *arg)
{
	rt_printf("%d\n", add(42,42));
}

int start_add_task() {
	RT_TASK* add_task = malloc(sizeof(RT_TASK));

	// Create/Start periodic task
	rt_task_create(add_task, "Frouke", 0, 99, 0);
	rt_task_start(add_task, &add_task_f, NULL);

	return 1;
}
