#include <stdio.h>
#include <fftw3.h>
#include <math.h>

const int SAMPLE_RATE = 128;
const int SINE_FREQ = 200;

int main()
{
   const int N = 1024;
   const int half_N = N / 2;

   double *in;
   fftw_complex *out;
   fftw_plan my_plan;

   in = (double*) fftw_malloc(sizeof(double) * N);
   out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

   // populate in with a sine wave of SINE_FREQ hz
   float inv_period = (float) SINE_FREQ / SAMPLE_RATE;
   printf("1/period: %f\n", inv_period);
   for (int i = 0; i < N; ++i) {
      in[i] = sinf(inv_period * i);
   }

   my_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

   fftw_execute(my_plan);

   for (int i = 0; i <= half_N; ++i) {
      printf("%i:\t%f\n", i, out[i][0]);
   }

   fftw_destroy_plan(my_plan);
   fftw_free(in);
   fftw_free(out);

   return 0;
}
