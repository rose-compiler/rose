
/*
One more problem has come up since I fixed the last one. I am using the -rose:C_only flag out of necessity, but this causes a new problem. The sample code below parses well if not using the -rose:C_only flag, with the flag, an error occurs. How do I get around this problem? Will the newer version of ROSE address this problem? If so, can I download it from anywhere yet?

Sample Code:
------------------------------------------------------
int main(void)
{
 int i,j,c=0;
 for(i=0; i<4; i++){
   for(j=0; j<4; j++)
     c++;
 }
 return 0;
}
------------------------------------------------------

The following is the error given by the translator:
identityTranslator: /home/dquinlan/ROSE/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:18339: SgForStatement* sage_gen_for_statement(a_statement*): Assertion `isSgBasicBlock(sageStmt) != __null' failed.
Aborted

This problem was found in the NAS IS benchmark, which I would like to parse.

Thanks,
Isaac
*/

int main(void)
   {
     int i,j,c=0;
     for(i=0; i<4; i++)
        {
          for(j=0; j<4; j++)
               c++;
        }

     return 0;
   }
