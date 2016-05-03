/*
   Bug contributed by: Chadd Williams <chadd@cs.umd.edu>
 */

/*******START CODE ***********/
#ifdef __clang__
int main()
#else
main()
#endif
{
    int m;
    for (m =0; m < 10; m++);

}
/**********END CODE*********/

/*
I ran this using the dotGenerator:

dotGenerator -rose:C_only testFor.c 

and received this error:

Finished building EDG AST, now build the SAGE AST ... 
dotGenerator: ../../../ROSE-0.5.31a/EDG/src/sage_gen_be.C:11137: SgForStatement *sage_gen_for_statement (a_statement *): Assertion `sageStmt != __null' failed.
Aborted

*/
