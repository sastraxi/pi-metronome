#ifndef __GPIO_H__
#define __GPIO_H__

  #include <pigpio.h>

  /* 25 - 40,000 */
  #define GPIO_PWM_RANGE 255
  #define SET_PWM_RANGE_AT_INIT

  const int** GPIO_PINS = [
    [2, 3, 4]
  ];

  void __light_init(void) {
    gpioInitialise();
    #ifdef SET_PWM_RANGE_AT_INIT
      const int pinCount = sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]);
      for (int pin = 0; pin < pinCount; ++pin) {
        for (int v = 0; v < 3; ++v) {
          gpioSetPWMrange(GPIO_PINS[pin][v], GPIO_PWM_RANGE);
        }
      }
    #endif
  }

  void __light_terminate(void) {
    gpioTerminate();
  }
  
  inline int setLight(int pin, float red, float green, float blue) {
    const float* colour = [red, green, blue];
    for (int v = 0; v < 3; ++v) {
      int rv = gpioPWM(GPIO_PINS[pin][v], colour[v]);
      if (rv != 0) return rv;
    }
    return 0;
  }

#endif
