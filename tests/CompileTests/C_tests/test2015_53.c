// This code demonstrates where the internal block handling in ROSE
// is incorrect when the default statement has a GNU statement-expression
// that is not wrapped in a SgBaslicBlock.  When DEMO_BUG is set to 1,
// the while statement will not be unparsed, when it is set to 0, the
// while statement will be unparsed properly.

#define DEMO_BUG 1

void foobar()
   {
     switch( 1 )
        {
          case 0:
             {
               break; 
             }
#if DEMO_BUG
       // BUG: This code causes ROSE to get the internal block handling wrong.
          default:
               ({ int random_var_name179345536=(int)0L; random_var_name179345536; }) ;
#else
       // This code will work fine and generate and AST that unparses the while loop at the end.
          default:
             {
               ({ int random_var_name179345536=(int)0L; random_var_name179345536; }) ;
             }
#endif
        }

     while( 1 )
        {
          break; 
        }
   }

