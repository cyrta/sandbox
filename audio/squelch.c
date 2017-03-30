/* Zero signal to digital silence when sufficiently quiet
 *
 *
   squelch zeroes an incoming PCM signal to digital silence when sufficiently quiet
   for a sufficient time. This can be helpful when compressing a recorded signal
   using RLE, for example.

   Signal comes in through stdin and the squelched signal is written to stdout.

   Stream format is assumed to be raw 16-bit signed-integer PCM.
 
   Usage
   -----

   some_program --producing --samples | squelch [OPTION]... | ...


    example:
        sox       -r 16k -t raw -e signed -b 16 -c 1 -V1 -\
               -r 11025 -t raw - sinc 200-4500 |\
                 squelch   -l 5 -d 4096 -t 64 |\
                   sox       -t raw -e signed -c 1 -b 16 -r 11025 - recording.wav


   Command line options
   --------------------

     -u SAMPLES
           output buffer length, samples; defaults to 2048

             -l LEVEL
                   silence threshold, in absolute amplitude; defaults to 1024 (-36 dB)

                     -d SAMPLES
                           duration of silence to trigger squelch, in samples; defaults to 4096

                             -t SAMPLES
                                   transition time, in samples; defaults to 512

  code: https://github.com/windytan/squelch

  more: http://www.windytan.com/2013/07/squelch-it-out.html

 *
 * Oona Räisänen 2012 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char **argv) {

  /* Defaults */
  short int      ssize    = 16;
  unsigned int   buflen   = 2048;
  int            limit    = 1024;
  unsigned int   duration = 4096;
  unsigned int   ttime    = 512;

  unsigned short bufptr = 0;
  short int      pcm;
  short int      outbuf[buflen];
  short int      silcount = 0, ampcount = 0;
  float          amp = 1.0f;
  bool           silent = false, falling = false, rising = false;
  int            c;

  /* Command line options */
  while ((c = getopt (argc, argv, "u:l:d:t:")) != -1)
    switch (c) {
      case 'u':
        buflen = atoi(optarg);
        break;
      case 'l':
        limit = atoi(optarg);
        break;
      case 'd':
        duration = atoi(optarg);
        break;
      case 't':
        ttime = atoi(optarg);
        break;
      case '?':
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        return EXIT_FAILURE;
      default:
        break;
    }

  /* Actual signal */
  while (read(0, &pcm, ssize/8)) {

    /* Squelch is active */
    if (silent) {

      if (falling) {
        amp = (float)(ttime - ampcount) / ttime;
        outbuf[bufptr] = pcm * amp;
        ampcount ++;
        if (ampcount > ttime)
          falling = false;
      } else {
        outbuf[bufptr] = 0x0000;
      }

      /* Signal comes back */
      if (abs(pcm) > limit) {
        silent   = false;
        rising   = true;
        ampcount = 0;
        silcount = 0;
      }

    /* Squelch not active */
    } else {

      if (rising) {
        amp = ((float)ampcount) / ttime;
        outbuf[bufptr] = pcm * amp;
        ampcount ++;
        if (ampcount > ttime) rising = false;
      } else {
        outbuf[bufptr] = pcm;
      }

      /* Signal is silent */
      if (abs(pcm) < limit) {
        silcount ++;
        if (silcount > duration) {
          silent   = true;
          falling  = true;
          ampcount = 0;
        }
      }
    }

    if (++bufptr == buflen) {
      if (!write(1, &outbuf, 2 * buflen))
        return (EXIT_FAILURE);
      fflush(stdout);
      bufptr = 0;
    }

  }
}
