AC_INIT([automake_hello_world], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
AC_SEARCH_LIBS([fftw_malloc], [fftw3], [], [
  AC_MSG_ERROR([unable to find fftw3()])
])
