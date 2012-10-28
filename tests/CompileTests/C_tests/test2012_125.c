// DQ (10/28/2012):
// This test code is taken from tcpdump: file print-bgp.c
// It demonstrates a source sequence problem in C89 support.
// It fails as a result if recent bug fixes yesterday.  It
// was caught as part of compiling against tcpdump prior 
// to commit/push of yesterdays work.

void foobar()
   {
     int atype;
     switch (atype) 
        {
          case 14:
//        done: break;
          done: 48;

          case 16:
             {
             }
        }
   }



