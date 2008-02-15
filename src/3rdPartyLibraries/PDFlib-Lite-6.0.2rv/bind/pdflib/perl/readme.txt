PDFlib and different versions of Perl
=====================================

Since extensions for Perl depend on the specific version of Perl in
use, we deliver various binary flavors of the PDFlib binding for Perl.

In the binary packages you will find a separate directory for 
each supported version of Perl:

- perl56	For Perl 5.6.x
- perl56-nt	For Perl 5.6.x (compiled without ithread support)
- perl58	For Perl 5.8.x
- perl58-nt	For Perl 5.8.x (compiled without ithread support)
- perlAS56	For ActiveState Perl 5.6
- perlAS58	For ActiveState Perl 5.8

Note that not all versions are available for all platforms.


If you see the following message:

  Unresolved symbol: Perl_Gthr_key_ptr (code)

you are using a version of PDFlib which has been built with ithread
support, but have a Perl binary without ithread support. To solve
this you must use the "nt" build of PDFlib.
