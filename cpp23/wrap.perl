#!/usr/bin/env perl

# Based on https://biowize.wordpress.com/2011/03/11/text-wrapping-with-dot-graphviz/

use strict;
 
my $usage = "setdotlabelwidth [char-width] < [dotfile]";
my $width = shift() or die("Usage: $usage $!");
 
while(<STDIN>)
{
  if(m/label="((?:\\.|[^\\"])*)"/)
  {
    my $labeltext = $1;
    my @words = split(/ /, $labeltext);
    my @newtext = ();
    my $newline = "";
    foreach my $word(@words)
    {
      if( length($newline) > 0 and
          length($newline) + length($word) > $width )
      {
        push(@newtext, $newline);
        $newline = "";
      }
      $newline .= " " if( length($newline) > 0 );
      $newline .= $word;
    }
    push(@newtext, $newline) if( length($newline) > 0 );
    my $newlabel = join("\\n", @newtext);
    s/label="((?:\\.|[^\\"])*)"/label="$newlabel"/;
  }
  print;
}
