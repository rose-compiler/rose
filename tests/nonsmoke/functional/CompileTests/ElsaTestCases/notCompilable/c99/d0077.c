//  ./zebra-0.93b-1/bgpd-h3ia.i.c_out:48:/home/dsw/oink_extra/ballAruns/tmpfiles/./zebra-0.93b-1/bgpd-h3ia.i:10087:125: Parse error (state 286) at __restrict__

// Test qualifiers in array brackets; Ben tells me that this is a
// C99-ism.  "__restrict" is implemented as a gnu extension so I have
// it commented out for now since I don't see an elsa/c99.gr

extern int regexec
  (
   int [/*__restrict*/ const volatile], // abstract
   int __pmatch [const /*__restrict*/ volatile], // direct
   int __eflags);
