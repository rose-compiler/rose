/*
gives the following error when compiled with ROSE:
identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:20917:
SgBasicBlock* sage_gen_block_statement(a_statement*): Assertion `startingFileInfo->ok()' failed.
/home/saebjornsen1/links/gcc3.3.3: line 4: 10314 Aborted                
/home/saebjornsen1/ROSE-October/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator
-rose:C_only $@
*/

void func1()
{
   static int we;
    if (we >= 0) {
    } else
    if (1 /*we != 0*/) {
    }
};

