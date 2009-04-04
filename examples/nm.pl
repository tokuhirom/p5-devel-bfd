use strict;
use warnings;
use Devel::BFD;

my $fname = shift @ARGV or die "Usage: $0 path/to/libs.a";
my $f = Devel::BFD->openr($fname);
my @table = $f->symtable();
for my $row (@table) {
    printf "%08X %s\n", $row->{value}, $row->{name};
}

