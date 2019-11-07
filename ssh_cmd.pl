#!/usr/bin/perl -w
use strict;
use Net::SSH::Perl

my $cmd = 'ls /root';
my $ssh = Net::SSH::Perl->new("10.102.34.200", debug=>0);
$ssh->login("root","msp.321");
my ($stdout,$stderr,$exit) = $ssh->cmd("python3 /root/ats/client/pyhttpDelayedPost.py");
print("###stdout###\n");
print $stdout;
print("###stderr###\n");
print $stderr;
print("###exit###\n");
print $exit;
