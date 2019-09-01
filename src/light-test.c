#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "light/gpio.h"

int main()
{
  __light_init();

	while (1)
  {
    setLightRGB(
      0,
      (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE,
      (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE,
      (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE
    );
    setLight(1, (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE);
    setLight(2, (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE);
    setLight(3, (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE);
    sleep(1);
	}

  __light_terminate();

	return 0;
}
