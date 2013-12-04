void foobar()
   {
  // int* q;
  // for (q = ((void *)0), p = ptycmds; p != cmd; q = p, p = p->next);

     int i;

  // This is interesting because the body is a SgNullStatement and EDG does not 
  // provide source positions for such statements. So we build a token sequence
  // to represent the SgNullStatement, but the previous child in the call to 
  // evaluateSynthesizedAttribute() for the SgForStmt, is not a SgStatement
  // and so does not have a token sequence mapping.
     for (i = 0; i < 10; i++);

  // int x,y;
  // if (x++ + ++y+1 == ++x + ++y);
  // if (x++ + y++ == x++ + ++y+1);
   }
