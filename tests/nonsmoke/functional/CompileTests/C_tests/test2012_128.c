// DQ (10/28/2012):
// This test code is that experiments with a label being asscoaited with a case label, which works...
// This fails in ROSE currently.

void foobar()
   {
     int atype;
     switch (atype) 
        {
          case 14:
//        done: break;
          done: /* no associated statement 48; THIS IS LEGAL CODE */

          case 16:
             {
             }
        }
   }

