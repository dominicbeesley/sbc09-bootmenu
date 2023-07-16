#!/bin/env perl

use strict;
use List::Util qw/sum/;

# A script to convert 16-bit srec to 6809 basic pokes/calls

while (<>) {

	my $l = $_;
	$l =~ s/^\s+//;
	$l =~ s/#.*//;
	$l =~ s/[\s\r\n]+//;

	if ($l eq "") {
		# skip blank
	} elsif ($l =~ /^S([0-9])(([0-9A-F]{2})+)/) {
		my $cmd = $1;
		my @hex = ($2 =~ /(..)/g);
		my @bin = map { hex($_) } @hex;
		
		scalar(@bin)-1 == @bin[0] or die "Length @bin[0] != " . scalar(@bin)-1;

		(sum(@bin) & 0xFF) == 0xFF or die "Bad cksum " . sprintf "%x", sum(@bin) & 0xFF;

		my $addr = (@bin[1] * 256) + @bin[2];

		if ($cmd eq "1") {
			my @data = splice(@bin, 3, @bin[0]-3);
			my $l = "";
			while (scalar(@data) > 0) {
				if ($l ne "")
				{
					$l = $l . ":";
				}
				if (scalar(@data) >=4) {

					my $x4 = (@data[3] << 24) | (@data[2] << 16) | (@data[1] << 8) | (@data[0]);

					$l .= sprintf "!&%X=&%X", $addr, $x4;
					
					@data = splice(@data, 4);
					$addr = $addr + 4;
				} else {
					$l .= sprintf "?&%X=%X", $addr, @data[0];
					
					@data = splice(@data, 1);
					$addr = $addr + 1;					
				}
				if (length($l) > 200) {
					print "$l\n";
					$l = "";
				}
			}
			if ($l ne "") {
				print "$l\n";
			}
		} elsif ($cmd eq "9") {
			printf "CALL &%X\n", $addr;
		}
	} else {
		die "Unrecognized line $l";
	}
}