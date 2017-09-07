#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <alchemy/task.h>

RT_TASK demo_task;

void demo(void *arg) {
        RT_TASK_INFO curtaskinfo;
        rt_task_inquire(NULL,&curtaskinfo);
        uint8_t num = * (uint8_t *)arg;
        rt_printf("Task name: %s\nTask Nr: %d\n\n", curtaskinfo.name, num);
}

int main(int argc, char* argv[])
{
        // Allocate string on heap
        char* str = malloc(10);

        // Load default string
        strcpy(str, "Task ");

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

        // Free the string that was allocated on the heap
        free(str);
}
