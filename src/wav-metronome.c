#include <stdio.h>
#include <fftw3.h>
#include <math.h>
#include <time.h>

#define SEC_TO_NANOSEC 1000000000
#define M_PI 3.14159265358979323846

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

inline double magn(fftw_complex c)
{
   return c[0] * c[0] + c[1] * c[1];
}

inline int sleep(struct timespec *tim, double sec)
{  
   tim->tv_sec = (int) floor(sec);
   tim->tv_nsec = (int) (SEC_TO_NANOSEC * sec - floor(sec));
   return nanosleep(tim, NULL);
}

int main(int argc, char** argv)
{
   struct timespec tim;

   const int N = 1024;
   const double N_1 = N - 1;
   const int half_N = N/2;
   const int SKIP = N/16;   

   if (argc != 2) {
      printf("Usage: %s <path-to-wav>\n", argv[0]);
      return 1;
   }

   drwav* pWav = drwav_open_file(argv[1]);
   if (pWav == NULL) {
      printf("Could not load %s!", argv[1]);
      return 2;
   }

   float *buf;
   double *in;
   fftw_complex *out;
   fftw_plan my_plan;

   buf = (float*) fftw_malloc((size_t) pWav->totalPCMFrameCount * pWav->channels * sizeof(float));
   in = (double*) fftw_malloc(sizeof(double) * N);
   out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

   drwav_read_pcm_frames_f32(pWav, pWav->totalPCMFrameCount, buf);
   printf("%d samples at %d hz\n\n", pWav->totalPCMFrameCount, pWav->sampleRate);
   double skip_sec = (double) SKIP / pWav->sampleRate;

   my_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

   for (int off = 0; off += SKIP;)
   {
      /* idk why but the loop end wasn't working for me in the for... */
      if (off >= (pWav->totalPCMFrameCount - N))
      {
         break;
      }

      /* copy float -> double, applying window function (hann) */
      for (int x = 0; x < N; ++x)
      {
         double multiplier = 0.5 * (1.0 - cos(2.0 * M_PI * x/N_1));
         in[x] = multiplier * (double) buf[off + x];
      }

      fftw_execute(my_plan);

      /* find the frequency bin with the highest frequency */
      double max = -1.0;
      double max_l_freq;
      double max_h_freq;
      for (int i = 0; i <= half_N; ++i)
      {
         double mag = magn(out[i]);
         /* printf("%f", mag); */

         if (mag > max) {
            max = mag;
            max_l_freq = i * ((double) pWav->sampleRate / N);
            max_h_freq = (i + 1) * ((double) pWav->sampleRate / N);
         }
      }

      if (max > 2000) {
         printf("@%d\t%.2f-%.2f hz: %f\n", off, max_l_freq, max_h_freq, max);
      }

      sleep(&tim, skip_sec);
   }

   fftw_destroy_plan(my_plan);
   fftw_free(in);
   fftw_free(out);
   fftw_free(buf);

   return 0;
}
