use strict;
use Test::More tests => 1;

use Devel::BFD;

my $fname = shift @ARGV || '/usr/lib/libdb-4.6.so';
my $f = Devel::BFD->openr($fname);
ok $f->printable_name, $f->printable_name;
$f->symtable();

