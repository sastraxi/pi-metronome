#ifndef __GPIO_H__
#define __GPIO_H__

  /* 25 - 40,000 */
  #define GPIO_PWM_RANGE 40000
  #define SET_PWM_RANGE_AT_INIT

  int GPIO_PINS[][3] = {
    {2, 3, 4},
    {14},
    {15},
    {18}
  };

  void __light_init(void);
  void __light_terminate(void);
  inline int setLightRGB(int, float, float, float);
  inline int setLight(int, float);

#endif
