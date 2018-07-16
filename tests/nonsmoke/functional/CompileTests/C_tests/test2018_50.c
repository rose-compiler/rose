void foobar(x1, larray)
     // order should not matter, no argument declaration defaults to int
	long *larray;
   {
     long l;
  // This unparses to x1[l] instaed of larray[l]
     larray[l];
   }
