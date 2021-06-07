
// DQ (5/22/2021): This example generates an incorrect AST!
void foobar()
   {
     int x;
     switch(x)
          case 0:
          // This SgBasicBlock is not correct here, it is put into the function body's basic block.
             {
               x = 76;
               break;
             }
   }
