#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <alchemy/task.h>
#include <alchemy/sem.h>

RT_TASK lp_task, mp_task, hp_task;
RT_SEM lh_sem;

void high_task(void *arg) {
	RT_TASK_INFO curtaskinfo;
	rt_task_inquire(NULL,&curtaskinfo);

	uint8_t i;
	for(i = 0; i < 3; i++) {
		rt_printf("%s priority task tries to lock semaphore\n", curtaskinfo.name);
		rt_sem_p_timed(&lh_sem, NULL);
		rt_printf("%s priority task locks semaphore\n", curtaskinfo.name);
		rt_sem_v(&lh_sem);
		rt_printf("%s priority task unlocks semaphore\n", curtaskinfo.name);
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
		rt_sem_p_timed(&lh_sem, NULL);
		rt_printf("%s priority task locks semaphore\n", curtaskinfo.name);

		if(i == 0)
			rt_task_start(&mp_task, &medium_task, 0);

		rt_printf("%s priority task unlocks semaphore\n", curtaskinfo.name);
		rt_sem_v(&lh_sem);
	}
	printf("..........................................%s priority task ends\n", curtaskinfo.name);
}

int main(int argc, char* argv[])
{
	rt_sem_create(&lh_sem,"Low High Semaphore",1,S_FIFO);

	// Create and Start tasks
	rt_task_create(&lp_task, "Low", 0, 1, 0);
	rt_task_create(&mp_task, "Medium", 0, 2, 0);
	rt_task_create(&hp_task, "High", 0, 3, 0);

	rt_task_start(&lp_task, &low_task, 0);

}
