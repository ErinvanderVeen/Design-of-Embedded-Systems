#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <alchemy/task.h>

RT_TASK demo_task;

void demo(void *arg) {
        RT_TASK_INFO curtaskinfo;
        rt_task_inquire(NULL,&curtaskinfo);
        int num = * (int *)arg;
        rt_printf("Task name: %s %d\n", curtaskinfo.name, num);
}

int main(int argc, char* argv[])
{
        // Allocate int on heap to improve performance
        int* index = malloc(sizeof(int));

        for(*index = 1; *index < 6; (*index)++) {
                // Create task
                rt_task_create(&demo_task, "Task ", 0, 50, 0);

                // Start task
                rt_task_start(&demo_task, &demo, index);
        }

        // Free the string that was allocated on the heap
        free(index);
}
