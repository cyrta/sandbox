
// WAV file size maximiser - for use in pipes
//
// compile with: gcc -o maxwav maxwav.c

 //my micro-program maxwav which sets the lengths in the wav header to their maximum
//http://www.volkerschatz.com/noise/alsa.html

#include <stdlib.h>
#include <stdio.h>

#define BUFSZ	131072

typedef struct wavhead {
  unsigned int      RIFF;
  unsigned int      totalsizem8;
  unsigned int      WAVE;
  unsigned int      fmt;
  unsigned int      fmtsize;
  unsigned short    audiofmt;
  unsigned short    nchannels;
  unsigned int      samplerate;
  unsigned int      byterate;
  unsigned short    blockalign;
  unsigned short    bitspersample;
  unsigned int      data;
  unsigned int      restsize;
}
wavheader;

int main(int argc, char **argv)
{
  wavheader head;
  size_t bytes;
  char buf[BUFSZ];

  if( argc > 1 ) {
    fprintf(stderr, "usage: maxwav\n"
"Replaces the file length information in a wave (RIFF) file header by its\n"
"maximum.  Intended for use in UNIX pipes transmitting wave data.\n");
    return 0;
  }
  bytes= fread(&head, 1, sizeof(wavheader), stdin);
  if( bytes< sizeof(wavheader) ) {
    fwrite(&head, 1, bytes, stdout);
    return 0;
  }
  head.totalsizem8= 0x7FFFFFF7;
  head.restsize= head.totalsizem8 - ((char*)&head.restsize - (char*)&head.totalsizem8);
  fwrite(&head, 1, bytes, stdout);
  while( !feof(stdin) && !ferror(stdin) )
  {
    bytes= fread(buf, 1, BUFSZ, stdin);
    fwrite(buf, 1, bytes, stdout);
  }
  return 0;
}


