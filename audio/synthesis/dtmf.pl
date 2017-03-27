use warnings;

$fs = 48000;
$pi = 3.141592653589793;

open U, "|sox -t .raw -e sig -b 16 -c 1 -r $fs - numbers.wav sinc 300-4300";

dtmf ("12345567899000100100");

close U;

sub dtmf {

  my %tones = ( 1 => [697,1209], 2 => [697,1336], 3 => [697,1477], "A"=>[697,1633],
                4 => [770,1209], 5 => [770,1336], 6 => [770,1477], "B"=>[770,1633],
                7 => [852,1209], 8 => [852,1336], 9 => [852,1477], "C"=>[852,1633],
                "*"=>[941,1209], 0 => [941,1336], "#"=>[941,1477], "D"=>[941,1633] );

  my $tdur = 0.1;
  my $pdur = 0.1;

  my $amp = 10000;

  for $char (split(//,$_[0])) {
    my $ph = 0;
    my $t = 0;
    my $n;

    for $n (0..int($tdur*$fs+.5)-1) {
      $t = $n/$fs;
      print U pack("s", ($amp * (cos(2*$pi*@{$tones{$char}}[0]*$t) +
                                 cos(2*$pi*@{$tones{$char}}[1]*$t))));
    }
    for (0..int($pdur*$fs+.5)-1) {
      print U pack("s", 0);
    }
  }
}