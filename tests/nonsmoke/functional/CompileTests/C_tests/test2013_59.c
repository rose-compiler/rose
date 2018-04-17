
int handle_sigchld()
   {
     int status;

  // DQ (9/6/2013): This example test code is still a problem because a double block is still 
  // generated, even with the fix at the base of convert_conditional().

  // DQ (9/5/2013): It is a problem when the statement expression is in a function paramter.
  // original code:   switch (sig2str( (union {int __in;int __i;}){status} . __i & 0xff00 ) == -1)
  // DQ (9/6/2013): I think we get equivalent code, but it is not identical.  Perhaps because
  // a different type declaration would have been seen in the nodes that were skipped and instead
  // the optimized one is used alternatively.  The code that controls this is at the base of 
  // convert_conditional().
  // equivalent code: switch(sig2str((union {int __i;}){status} . __i & 0xff00) == -1){
     switch (sig2str( (union {int __in;int __i;}){status} . __i & 0xff00 ) == -1)
        {
          case 9: status = 8;
        }
   }

