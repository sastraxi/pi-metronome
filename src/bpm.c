#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <pigpio.h>

#include "light/gpio.h"

#define M_PI 3.14159265358979323846
#define M_2PI 6.28318530718

#define SEC_TO_MICROSEC 1000000
#define MICROSEC_TO_SEC 0.000001

const double INV_UPDATE_HZ = 1.0 / 2000.0;

const int NUM_LIGHTS = 4;
const double INV_NUM_LIGHTS = 1.0 / (double) NUM_LIGHTS;

#define SIN_EXPONENT 15.0

inline double max(double a, double b) {
  if (a > b) return a;
  return b;
}

int wait_microsec(unsigned int n)
{
  return gpioSleep(PI_TIME_RELATIVE, 0, n);
}

unsigned long get_system_timer()
{
  int sec, microsec;
  gpioTime(PI_TIME_RELATIVE, &sec, &microsec);
  unsigned long systime = sec * SEC_TO_MICROSEC + microsec;
  return systime;
}

/**
 * Period: 1.0
 */
inline double tickfn(double p)
{
  // ping pong
  double int_part;
  double t = modf(p * 0.5, &int_part);
  t = (t > 0.5)
    ? 1.0 - 2.0 * (t - 0.5)
    : 2.0 * t;

  return fabs(
    pow(sin(t * M_2PI), SIN_EXPONENT)
  );
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    printf("Usage: %s <bpm>\n", argv[0]);
    return 1;
  }

  const double bpm = (float) atoi(argv[1]);
  const double rate = bpm / 60.0; // inverse of hz
  const unsigned int sleep_time = SEC_TO_MICROSEC * INV_UPDATE_HZ;

  __light_init();

  setLightRGB(0, 0.0f, 0.0f, 0.0f);
  setLight(1, 0.0f);
  setLight(2, 0.0f);
  setLight(3, 0.0f);

  unsigned long base_t = get_system_timer();
	while (1)
  {
    const unsigned long t = get_system_timer();
    if (t < base_t) {
      // we rolled over!
      printf("Rolled over! Old = %d, new = %d", base_t, t);
      return 1; // FIXME: don't terminate
    }

    double p = MICROSEC_TO_SEC * (t - base_t);
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
      double delay = INV_NUM_LIGHTS * (double) i;
      float intensity = tickfn(p * rate - delay) * GPIO_PWM_RANGE;
      if (i == 0) {
        setLightRGB(i, intensity, intensity, intensity);
      } else {
        setLight(i, intensity);
      }
    }

    wait_microsec(sleep_time);
	}

  __light_terminate();

	return 0;
}
