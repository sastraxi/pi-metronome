#include <fftw3.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define USE_LIGHT

#ifdef USE_LIGHT
	#include "light/gpio.h"
#endif

#define SEC_TO_NANOSEC 1000000000
#define M_PI 3.14159265358979323846
#define RING_BUFFER_SIZE_SEC 3.0

/* this program accepts RAW 16-bit S16LE samples only */
#define RATE 44100

inline double magn(fftw_complex c) {
	return c[0] * c[0] + c[1] * c[1];
}

int main()
{
	const int N = 4096;
	const double N_1 = N - 1;
	const int HALF_N = N/2;
	const int CHUNK = N/16; // read buffer AND fft window offset
	const int RING_BUFFER_SIZE = RATE * RING_BUFFER_SIZE_SEC;

	int countdown_to_fill = N;
	int write_ptr = 0, read_ptr = 0; // % ring_buffer_size to make sense of it
	int16_t *ring_buf;
	double *in;
	fftw_complex *out;
	fftw_plan my_plan;

	ring_buf = (int16_t*) fftw_malloc((size_t) RING_BUFFER_SIZE * sizeof(int16_t));
	in = (double*) fftw_malloc(sizeof(double) * N);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

	my_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

  #ifdef USE_LIGHT
	 __light_init();
  #endif

	while (1)
  {
		/* get new data from stdin */
		if (write_ptr + CHUNK <= RING_BUFFER_SIZE)
		{
			int rc = fread(&ring_buf[write_ptr], sizeof(int16_t), CHUNK, stdin);
			if (rc != CHUNK)
			{
				printf("EOF or error occurred");
				return 1;
			}
		}
		else
		{
			int rc, to_read;
			
			to_read = RING_BUFFER_SIZE - write_ptr;
			rc = fread(&ring_buf[write_ptr], sizeof(int16_t), to_read, stdin);
			if (rc != to_read)
			{
				printf("EOF or error occurred");
				return 1;
			}
			
			to_read = CHUNK - to_read;
			rc = fread(ring_buf, sizeof(int16_t), to_read, stdin);
			if (rc != to_read)
			{
				printf("EOF or error occurred");
				return 1;
			}
		}

		/* record our write */
		write_ptr += CHUNK;

		/* don't start processing until we have at least one fft window's worth of data */
		if (countdown_to_fill > 0) {
			countdown_to_fill -= CHUNK;
			if (countdown_to_fill > 0) continue;
		}

		/* copy float -> double, applying window function (hann) */
		for (int x = 0; x < N; ++x)
		{
			double multiplier = 0.5 * (1.0 - cos(2.0 * M_PI * x/N_1));
			in[x] = multiplier * (double) ring_buf[(read_ptr + x) % RING_BUFFER_SIZE] / (double) SHRT_MAX;
		}

		/* fast fourier go! */
		fftw_execute(my_plan);

		/* find the frequency bin with the highest frequency */
		double max = -1.0;
		double max_l_freq;
		double max_h_freq;
		for (int i = 0; i <= HALF_N; ++i)
		{
			double mag = magn(out[i]);
			/* printf("%f", mag); */

			if (mag > max) {
				max = mag;
				max_l_freq = i * ((double) RATE / N);
				max_h_freq = (i + 1) * ((double) RATE / N);
			}
		}

		/* print to the console (later: turn on an LED!) */
	  double scaled = max * 1.0;
		if (scaled > 20) {
			#ifdef USE_LIGHT
				setLight(0, 0, fmin(scaled, GPIO_PWM_RANGE), 0);
			#else
				printf("%.2f-%.2f hz: %f\n", max_l_freq, max_h_freq, max);
			#endif			
		} else {
			#ifdef USE_LIGHT
				setLight(0, 0, 0, 0);
			#endif
	 }

		/* record our read */
		read_ptr += CHUNK;

		/* fixup pointers so they fall in the range [0, ring_buffer_size) */
		write_ptr %= RING_BUFFER_SIZE;
		read_ptr %= RING_BUFFER_SIZE;
	}

	fftw_destroy_plan(my_plan);
	fftw_free(in);
	fftw_free(out);
	fftw_free(ring_buf);

	return 0;
}
