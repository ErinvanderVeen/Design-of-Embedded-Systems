#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <alchemy/task.h>

RT_TASK demo_task, demo_task2;

void demo(void *arg) {
        RTIME num = * (RTIME *)arg;
        rt_task_sleep(1000000000);
        int return_val = rt_task_set_periodic(NULL, TM_NOW, 1000000000*num);
        while(return_val == 0){
                rt_printf("Task nr: %llu\n", num);
                rt_task_wait_period(NULL);
        }
}

int main(int argc, char* argv[])
{
        // Allocate string on heap
        char* str = malloc(10);

        // Load default string
        strcpy(str, "Task ");

        RTIME i;
        for(i = 1; i < 4; i++) {
                // Only last part of buffer is overwritten
                // to increase performance
                sprintf(&str[5], "%d", i);

                // Create task
                rt_task_create(&demo_task, str, 0, 50, 0);

                // Start task
                rt_task_start(&demo_task, &demo, &i);
        }

        printf("end program by CTRL-C\n");
        pause();

        // Free the string that was allocated on the heap
        free(str);
}
