#include <stdio.h>
#include <fftw3.h>
#include <math.h>

#define M_PI 3.14159265358979323846

const int SAMPLE_RATE = 44100;
const int SINE_FREQ = 500;

inline double magn(fftw_complex c)
{
   return c[0] * c[0] + c[1] * c[1];
}

int main()
{
   const int N = 1000;
   const int half_N = N / 2;

   double *in;
   fftw_complex *out;
   fftw_plan my_plan;

   in = (double*) fftw_malloc(sizeof(double) * N);
   out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

   /* populate in with a sine wave of SINE_FREQ hz */
   double inv_period = (double) SINE_FREQ / SAMPLE_RATE;
   printf("1/period: %f\n", inv_period);
   for (int i = 0; i < N; ++i) {
      in[i] = sin(M_PI * 2.0 * inv_period * i);
   }

   my_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

   fftw_execute(my_plan);

   /* find the frequency bin with the highest frequency */
   double max = -1.0;
   double max_l_freq;
   double max_h_freq;
   for (int i = 0; i <= half_N; ++i) {
      double mag = magn(out[i]);
      /* printf("%f", mag); */

      if (mag > max) {
         max = mag;
         max_l_freq = i * ((double) SAMPLE_RATE / N);
         max_h_freq = (i + 1) * ((double) SAMPLE_RATE / N);
      }
   }

   printf("%.2f-%.2f hz: %f\n", max_l_freq, max_h_freq, max);

   fftw_destroy_plan(my_plan);
   fftw_free(in);
   fftw_free(out);

   return 0;
}
