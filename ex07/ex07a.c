#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

int main(int argc, char* argv[])
{
	int fd, value, ret;

	value = 1;
	fd = open("/dev/rtdm/pinctrl-bcm2835/gpio22",O_WRONLY);

	while(1) {
		usleep(500000);
		ret = ioctl(fd, GPIO_RTIOC_DIR_OUT, &value);
		value ^= 1;
	}

}
