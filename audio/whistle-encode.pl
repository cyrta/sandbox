#!/usr/bin/perl
# windytan's pea whistle encoder
# http://www.windytan.com/2015/10/pea-whistle-steganography.html
# (c) 2015 Oona Räisänen
# ISC license

use warnings;
use strict;

my $data          = $ARGV[0] // "OHAI!";
my $outfile       = "whistle.wav";

my $fs            = 44100;       # sample rate
my $fc            = 2600;        # whistle pitch
my $bps           = 100;         # data speed
my $risetime      = 0.09;        # rise/fall slide duration
my $riseband      = 0.7 * $fc;   # bandwidth thereof
my $padding       = 0.5;         # silence before and after, sec
my $f_am          = 20;          # AM distortion frequency
my $depth_am      = 0.18;        # AM distortion modulation index
my $signal_gain   = dB(0);       # overall signal gain
my $noise_gain    = dB(12);      # overall noise gain post-bandpass
my $noise_shift   = 1.0;         # noise shift from harmonic (1 = none)
my $lp_alfa_fm    = 0.004;       # signal pre-fm lopass bandwidth
my $bp_alfa_noise = 0.009;       # noise bandpass bandwidth
my $fshift        = 0.077 * $fc; # FSK shift

# relative harmonic powers
my @harmonics       = (dB(0), dB(-17), dB(-33), dB(-28), dB(-31), dB(-44));
my @noise_harmonics = (dB(0), dB(-1),  dB(-2),  dB(-3),  dB(-4),  dB(-6));

$data           = "\xAA\xA7" . pack("C", length($data)) . $data;
my $dur         = length($data) * 8 / $bps;

my $prev_noise = 0;
my $prev_f = $fc - $riseband;
my $f, my $f_filtered, my $vol;
my $ph_am, my $ph_fm;

open my $stream, '|-', 'sox -t .s16 -c 1 -r ' . $fs . ' - ' . $outfile
  or die $!;
  
binmode($stream) or die $!;

print $stream pcmWord(0) x ($padding * $fs);

for (my $t = 0; $t < $risetime * 2 + $dur; $t += 1/$fs) {

  if ($t < $risetime) {
    # attack
    $f = $fc - $riseband + ($t/$risetime * $riseband);
    $vol = $t/$risetime;
    $harmonics[0] = ($t < $risetime * .66 ? 0 :
      ($t-$risetime * .66) / ($risetime * .33));

  } elsif ($t > $risetime + $dur) {
    # release
    $f = $fc - ($t-$dur-$risetime) / $risetime * $riseband;
    $vol = dB(0) - ($t-$dur-$risetime) / $risetime;
    $harmonics[0] = ($t-$dur-$risetime > $risetime * .33 ? 0 :
      dB(0) - ($t-$dur-$risetime) / ($risetime * .33));

  } else {

    $f = $fc;
    $harmonics[0] = dB(0);

    my $bit_i = int(($t - $risetime) * $bps);
    my $byte  = substr($data, int($bit_i / 8), 1);
    my $bit   = ((unpack("C", $byte) >> ($bit_i % 8)) & 1);

    $f = ($bit - .5) * $fshift + $fc;

    $vol = dB(0);
  }

  $f_filtered = $lp_alfa_fm * $f + (1 - $lp_alfa_fm) * $prev_f;
  $prev_f     = $f_filtered;

  my $noise = rand() - .5;
  my $noise_filtered = $bp_alfa_noise * $noise + (1 - $bp_alfa_noise) *
    $prev_noise;
  $prev_noise = $noise_filtered;

  $ph_fm += phaseInc($f_filtered);
  $ph_am += phaseInc($f_am);

  my $mix = 0;
  for my $n (0..$#harmonics) {
    my $sig = $harmonics[$n] * cos(($n + 1) * $ph_fm);
    $mix += $signal_gain * $sig * (dB(0) + $depth_am * cos($ph_am));
    $mix += $noise_gain * $noise_filtered * $noise_harmonics[$n] *
      cos(($n + 1) * ($ph_fm * $noise_shift));
  }

  print $stream pcmWord(dB(-14) * $vol * $mix);
}

print $stream pcmWord(0) x ($padding * $fs);

close $stream;

sub dB       { 10 ** ($_[0] / 20); }
sub pcmWord  { pack "s", round($_[0] * (2**16) / 2); }
sub phaseInc { 2 * 3.14159 * $_[0] / $fs; }
sub round    { $_[0] >= 0 ? int($_[0] + .5) : int($_[0] - .5); }