#!/usr/bin/perl
# windytan 2017, public domain
use warnings;
use strict;

my $img_width      = 1200;
my $img_height     = 900;
my $yscale         = 0.6;
my $xscale         = 0.13;
my $rowheight      = 5;
my $start_y        = 160;
my $start_x        = 300;
my $num_lines      = 130;
my $fill_height    = 200;
my $data_separator = ',';
my $slope          = -1.2;
my $outfile        = 'plot.png';

my $rownum = 0;
my @image;
while (my $line = <>) {
  chomp $line;
  my @cols = split /$data_separator/, $line;

  # white fill
  for my $colnum (0 .. $#cols) {
    my $x = $start_x + $colnum * $xscale + $rownum*$slope;

    my $liney = $start_y + $rownum * $rowheight - $cols[$colnum]*$yscale;

    for my $filly ($liney+1..$liney+$fill_height) {
      if ($x >= 0 && $filly >= 0) {
        $image[$x][$filly] = 255;
      }
    }
  }

  # black line
  for my $colnum (0 .. $#cols) {
    my $x = $start_x + $colnum * $xscale + $rownum*$slope;

    my $liney = $start_y + $rownum * $rowheight - $cols[$colnum]*$yscale;

    if ($x >= 0 && $liney >= 0) {
      $image[$x][$liney] = 0;
    }
  }

  last if (++$rownum >= $num_lines);
}

open my $magick, '|-', 'convert -depth 8 -size '.$img_width.'x'.$img_height.
                       ' gray:- '.$outfile or die ($!);
for my $y (0 .. $img_height-1) {
  for my $x (0 .. $img_width-1) {
    my $px = $image[$x][$y] // 255;
    print $magick chr($px);
  }
}
close $magick;