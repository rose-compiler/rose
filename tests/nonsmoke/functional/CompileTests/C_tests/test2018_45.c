
/* C90 does not support "//" as a comment, however, this appears in the test suit as a bug 
   I think this is not a bug in ROSE.
 */
void foobar()
   {
     int i;
     i = 7 //* comment */
          +5;
   }

