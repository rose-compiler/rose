// This test code is trying to represent the problems just noticed in the wget ccs_.c file.
// This is a bug in the new logical vs. physical source position handling.
// The bug is that the associated statement "1;" is not unparsed.

#line 2 "css.c"

int yylex (void)
   {
  // This comment should appear in the generated code.
do_action:
      1;

  // Test this with a simple SgBasicBlock.
label_a:
      {}

   }



