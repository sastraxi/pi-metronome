#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "light/gpio.h"

#define MMIO_BASE       0x3F000000
#define SYSTMR_LO       ((volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI       ((volatile unsigned int*)(MMIO_BASE+0x00003008))

#define M_PI 3.14159265358979323846

#define SEC_TO_MICROSEC 1000000
#define MICROSEC_TO_SEC 0.000001

const int NUM_LIGHTS  = 4;
const double INV_NUM_LIGHTS = 1.0 / (double) NUM_LIGHTS;

#define SIN_EXPONENT 1.0

inline double max(double a, double b) {
  if (a > b) return a;
  return b;
}

/**
 * Wait N microsec (ARM CPU only)
 * From https://github.com/bztsrc/raspi3-tutorial/blob/master/07_delays/delays.c#L39
 */
void wait_microsec(unsigned int n)
{
    register unsigned long f, t, r;
    // get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    // read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // calculate expire value for counter
    t+=((f/1000)*n)/1000;
    do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);
}

/**
 * Get System Timer's counter
 */
unsigned long get_system_timer()
{
    unsigned int h=-1, l;
    // we must read MMIO area as two separate 32 bit reads
    h=*SYSTMR_HI;
    l=*SYSTMR_LO;
    // we have to repeat it if high word changed during read
    if(h!=*SYSTMR_HI) {
        h=*SYSTMR_HI;
        l=*SYSTMR_LO;
    }
    // compose long int value
    return ((unsigned long) h << 32) | l;
}

/**
 * Period: 1.0
 */
inline double tickfn(double p)
{
  return max(
    pow(sin(p * 2.0 * M_PI), SIN_EXPONENT),
    0.0
  );
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    printf("Usage: %s <bpm>\n", argv[0]);
    return 1;
  }

  const double bpm = (float) atoi(argv[1]);
  const double hz = 1.0 / (bpm / 60.0);
  const unsigned int sleep_time = SEC_TO_MICROSEC * hz;

  __light_init();
  setLightRGB(0, 0f, 0f, 0f);
  setLight(1, 0f);
  setLight(2, 0f);
  setLight(3, 0f);

  unsigned long base_t = get_system_timer();
	while (1)
  {
    const unsigned long t = get_system_timer();
    if (t < base_t) {
      // we rolled over!
      printf('Rolled over! Old = %d, new = %d', base_t, t);
    }


    double p = MICROSEC_TO_SEC * (t - base_t);
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
      double delay = INV_NUM_LIGHTS * (double) i;
      float intensity = tickfn((p + delay) * hz) * GPIO_PWM_RANGE;
      if (i == 0) {
        setLightRGB(i, intensity, intensity, intensity);
      } else {
        setLight(i, intensity);
      }
    }

    // setLight(1, (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE);
    // setLight(2, (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE);
    // setLight(3, (float) rand() / (float) RAND_MAX * GPIO_PWM_RANGE);
    
    wait_microsec(sleep_time);

    t = next_t;
	}

  __light_terminate();

	return 0;
}
