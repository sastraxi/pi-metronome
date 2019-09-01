bin_PROGRAMS = program ffttest metronome lighttest bpm

program_SOURCES = program.c
program_LDADD = -lfftw3 -lm

ffttest_SOURCES = fft-test.c
ffttest_LDADD = -lfftw3 -lm

metronome_SOURCES = stdin-metronome.c
metronome_LDADD = -lfftw3 -lm -lpigpio

lighttest_SOURCES = light-test.c
lighttest_LDADD = -lm -lpigpio 

bpm_SOURCES = bpm.c
bpm_LDADD = -lm -lpigpio
bpm_CFLAGS = -mcpu=arm1176jzf-s -mfpu=vfp
