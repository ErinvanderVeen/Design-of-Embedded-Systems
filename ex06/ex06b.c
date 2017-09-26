#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <alchemy/task.h>
#include <alchemy/mutex.h>

RT_TASK lp_task, mp_task, hp_task;
RT_MUTEX lh_mux;

void high_task(void *arg) {
	RT_TASK_INFO curtaskinfo;
	rt_task_inquire(NULL,&curtaskinfo);

	uint8_t i;
	for(i = 0; i < 3; i++) {
		rt_printf("%s priority task tries to lock mutex\n", curtaskinfo.name);
		rt_mutex_acquire_timed(&lh_mux, NULL);
		rt_printf("%s priority task locks mutex\n", curtaskinfo.name);
		rt_mutex_release(&lh_mux);
		rt_printf("%s priority task unlocks mutex\n", curtaskinfo.name);
	}
	printf("..........................................%s priority task ends\n", curtaskinfo.name);
}

void medium_task(void *arg) {
	RT_TASK_INFO curtaskinfo;
	rt_task_inquire(NULL,&curtaskinfo);

	uint8_t i;
	for(i = 0; i < 12; i++) {
		if(i == 2)
			rt_task_start(&hp_task, &high_task, 0);

		rt_printf("%s task running\n", curtaskinfo.name);
	}
	printf("------------------------------------------%s priority task ends\n", curtaskinfo.name);
}

void low_task(void *arg) {
	RT_TASK_INFO curtaskinfo;
	rt_task_inquire(NULL,&curtaskinfo);

	uint8_t i;
	for(i = 0; i < 3; i++) {
		rt_mutex_acquire_timed(&lh_mux, NULL);
		rt_printf("%s priority task locks mutex\n", curtaskinfo.name);

		if(i == 0)
			rt_task_start(&mp_task, &medium_task, 0);

		rt_printf("%s priority task unlocks mutex\n", curtaskinfo.name);
		rt_mutex_release(&lh_mux);
	}
	printf("..........................................%s priority task ends\n", curtaskinfo.name);
}

int main(int argc, char* argv[])
{
	rt_mutex_create(&lh_mux,"Low High Mutex");

	// Create and Start tasks
	rt_task_create(&lp_task, "Low", 0, 1, 0);
	rt_task_create(&mp_task, "Medium", 0, 2, 0);
	rt_task_create(&hp_task, "High", 0, 3, 0);

	rt_task_start(&lp_task, &low_task, 0);

}
