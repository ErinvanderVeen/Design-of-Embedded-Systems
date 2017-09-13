#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define ITER 10

static RT_TASK  t1;
static RT_TASK  t2;

uint8_t global = 0;
RT_SEM global_sem;

void taskOne(void *arg)
{
	uint8_t i;
	for (i = 0; i < ITER; i++) {
		// Wait for the semaphore
		rt_sem_p_timed(&global_sem, NULL);

		rt_printf("I am taskOne and global = %d................\n", ++global);

		// Release Global
		rt_sem_v(&global_sem);
	}
}

void taskTwo(void *arg)
{
	uint8_t i;
	for (i = 0; i < ITER; i++) {
		// Wait for the semaphore
		rt_sem_p_timed(&global_sem, NULL);

		rt_printf("I am taskTwo and global = %d----------------\n", --global);

		// Release Global
		rt_sem_v(&global_sem);
	}
}

int main(int argc, char* argv[]) {
	// Create semaphores
	rt_sem_create(&global_sem, "global counter semaphore", 0, S_FIFO);

	// Create tasks
	rt_task_create(&t1, "task1", 0, 1, 0);
	rt_task_create(&t2, "task2", 0, 1, 0);
	rt_task_start(&t1, &taskOne, 0);
	rt_task_start(&t2, &taskTwo, 0);

	// Start cycle
	rt_sem_v(&global_sem);

	// Clean up the semaphores
	rt_sem_delete(&global_sem);

	return 0;
}
