#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <alchemy/task.h>
#include <alchemy/sem.h>

RT_TASK demo_task;
RT_SEM start_sem;

void demo(void *arg) {
        RT_TASK_INFO curtaskinfo;
        rt_task_inquire(NULL,&curtaskinfo);
        uint8_t num = * (uint8_t *)arg;

	// Wait untill we can start
	rt_sem_p_timed(&start_sem, NULL);
        rt_printf("Task name: %s\nTask Nr: %d\n\n", curtaskinfo.name, num);
}

int main(int argc, char* argv[])
{
        // Allocate string on heap
        char* str = malloc(10);

        // Load default string
        strcpy(str, "Task ");

	// Create the semaphore on which ever task must wait
	rt_sem_create(&start_sem, "start semaphore", 0, S_PRIO);

        uint8_t i;
        for(i = 1; i < 6; i++) {
                // Only last part of buffer is overwritten
                // to increase performance
                sprintf(&str[5], "%d", i);

                // Create task
                rt_task_create(&demo_task, str, 0, i, 0);

                // Start task
                rt_task_start(&demo_task, &demo, &i);
        }

	// Either broadcast, which allows all waiting tasks to get executed
	//rt_sem_broadcast(&start_sem);

	// Or use a for loop to increase the semaphore 6 times
        for(i = 1; i < 6; i++) { rt_sem_v(&start_sem); }


        // Free the string that was allocated on the heap
        free(str);
}
