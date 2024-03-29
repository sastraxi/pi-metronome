#include <pigpio.h>
#include "gpio.h"

int GPIO_PINS[][3] = {
  {2, 3, 4},
  {14},
  {15},
  {18},
  {16, 20, 21}
};

void __light_init(void) {
  gpioInitialise();
  #ifdef SET_PWM_RANGE_AT_INIT
    const int lightCount = sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]);
    for (int light = 0; light < lightCount; ++light) {
      const int pinCount = sizeof(GPIO_PINS[light]) / sizeof(GPIO_PINS[light][0]);
      for (int v = 0; v < pinCount; ++v) {
        gpioSetPWMrange(GPIO_PINS[light][v], GPIO_PWM_RANGE);
      }
    }
  #endif
}

void __light_terminate(void) {
  gpioTerminate();
}

int setLightRGB(int pin, float red, float green, float blue) {
  const float colour[] = {red, green, blue};
  for (int v = 0; v < 3; ++v) {
    int rv = gpioPWM(GPIO_PINS[pin][v], colour[v]);
    if (rv != 0) return rv;
  }
  return 0;
}

int setLight(int pin, float intensity) {
  return gpioPWM(GPIO_PINS[pin][0], intensity);
}
