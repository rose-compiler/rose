// position test
int xyz;

/*

When working with -rose:C_only, the frontend fails for 'if... else if...' constructs:
expTranslator: ../../../../ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:22147: 
     SgBasicBlock* sage_gen_block_statement(a_statement*): Assertion `startingFileInfo->ok()' failed.

This would now be an urgent fix for us.
*/
int x;
void foo() {
   for (int i=0; i < 10;i++)
   {
   }
   
   while (0);

   { while (0); }

   do 
   {
   }
   while(0);

   switch(x) {}

   switch(x); // error in ROSE

   switch(x) { case 0: case 1234567890: }
   switch(x) { case 0: ; }
   switch(x) { case 0: break; }
   switch(x) { case 0: default: }
   switch(x) { case 0: default: break; }

   if (0);
   else if (0);
}

