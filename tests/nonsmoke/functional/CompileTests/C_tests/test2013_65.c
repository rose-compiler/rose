
int handle_sigchld()
   {
     int status;

  // DQ (9/5/2013): It is a problem when the statement expression is in a function paramter.
  // original code:   while (sig2str( (union {int __in;int __i;}){status} . __i & 0xff00 ) == -1)
  // DQ (9/6/2013): I think we get equivalent code, but it is not identical.  Perhaps because
  // a different type declaration would have been seen in the nodes that were skipped and instead
  // the optimized one is used alternatively.  The code that controls this is at the base of 
  // convert_conditional().
  // equivalent code: while(sig2str((union {int __i;}){status} . __i & 0xff00) == -1){

  // DQ (9/5/2013): It is a problem when the statement expression is in a function paramter.
  // switch (((union {int __in;int __i;}){status} . __i & 0xff00 ) == -1)
     switch (1)
        {
//        case 1: break;
        }
   }

