#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fftw3.h>

#define FFTLEN 2048
#define SRATE  22050

// Return sinusoid power from complex DFT coefficients
double power (fftw_complex coeff) {
  return pow(coeff[0],2) + pow(coeff[1],2);
}

// Return the FFT bin index matching the given frequency
int GetBin (double Freq) {
  return (Freq / SRATE * FFTLEN);
}


int main () {

  double f0  = 2900;
  double f1  = 4700;
  double bps = 850;

  int bin0 = GetBin(f0);
  int bin1 = GetBin(f1);

  double w = 2 * M_PI * (1.0 * bps/2 / SRATE);
  
  unsigned char *a, *b;

  a = calloc(1,sizeof(unsigned char));  
  b = calloc(1,sizeof(unsigned char));

  unsigned char pix;

  short int word=0;

  unsigned char *Ibuffer;
  unsigned char *Qbuffer;
  unsigned char *tbuf;
  Ibuffer = calloc( FFTLEN, sizeof(unsigned char));
  Qbuffer = calloc( FFTLEN, sizeof(unsigned char));
  tbuf    = calloc( FFTLEN, sizeof(unsigned char));

  int    lenf = SRATE/bps;
  int i,bit,n=0,prevbit=0,dbit,sync=0;
  int MaxBin,prevMB;
  int acc,nextpll=0;
  double lo,prevlo;

  double Hann[FFTLEN] = {0};
  double Power[FFTLEN], lo_phase;

  fftw_complex *in;
  fftw_complex *out;
  fftw_plan    Plan;

  in  = fftw_alloc_complex(FFTLEN);
  out = fftw_alloc_complex(FFTLEN*2);

  Plan = fftw_plan_dft_1d(FFTLEN, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

  for (i = 0; i < FFTLEN; i++) in[i][0] = in[i][1] = 0;

  // Create 1-bitlen Hann window
  for (i = 0; i < lenf; i++) Hann[i] = 0.5 * (1 - cos( 2 * M_PI * i / (1.0*lenf-1) ) );

  for (i = 0; i < FFTLEN; i++) Ibuffer[i] = Qbuffer[i] = 0;

  acc      = 0;
  prevMB   = 0;
  lo_phase = 0;
  prevlo   = 0;

  while ( ++n ) {

    // Reference oscillator
    lo = cos(w * n + lo_phase);

    // Read data from DSP
    if (!fread(a,1,1,stdin)) break;
    if (!fread(b,1,1,stdin)) break;

    // Move buffers
    memcpy(tbuf,&Ibuffer[1],lenf-1);
    memcpy(Ibuffer,tbuf,lenf-1);
    memcpy(tbuf,&Qbuffer[1],lenf-1);
    memcpy(Qbuffer,tbuf,lenf-1);
    Ibuffer[lenf-1] = a[0];
    Qbuffer[lenf-1] = b[0];
   
    if (n % 2 == 0) {

      // Apply Hann window
      for (i = 0; i < lenf-1; i++) {
        in[i][0] = Ibuffer[i] * Hann[i];
        in[i][1] = Qbuffer[i] * Hann[i];
      }

      fftw_execute(Plan);

      
      // Find the bin with most power
      MaxBin = 0;
      for (i = bin0-20; i < bin1+20; i++) {
        Power[i] = power(out[i]);
        if (MaxBin == 0 || Power[i] > Power[MaxBin]) MaxBin = i;
      }
      MaxBin -= (bin0 + bin1) / 2;

    }
    
    
    // PLL
    if ( ((prevMB < 0 && MaxBin >= 0) || (prevMB >= 0 && MaxBin < 0)) && n > nextpll ) {
      if (fmod(w * n + lo_phase, M_PI) > M_PI/2)
        lo_phase -= .1;
      else if (fmod(w * n + lo_phase, M_PI) < M_PI/2)
        lo_phase += .1;

      lo_phase = fmod(lo_phase, 2*M_PI);

      nextpll = n + lenf / 2;
    }

    // Integrator
    acc += MaxBin;
    if ( (prevlo < 0 && lo >= 0) || (prevlo >= 0 && lo < 0)) {

      bit  = (acc > 0);
      dbit = (prevbit != bit);

      if (sync) {
        printf("%d", dbit);
        word = ((word << 1) + dbit) & 0xFF;
        if (word == 0) {
          sync = 0;
          printf("\n");
          fflush(0);
        }
      } else {
        word = ((word << 1) + dbit) & 0xFF;
        sync = (word == 0xF9);
      }

      acc     = 0;
      prevbit = bit;
    }

    prevMB = MaxBin;
    prevlo = lo;

  }

}
