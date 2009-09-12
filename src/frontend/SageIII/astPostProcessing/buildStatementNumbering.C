
#include "rose.h"

void
SgSourceFile::
// buildStatementNumbering( SgSourceFile::NumberToStatementType & numberToStatementContainer )
buildStatementNumbering()
   {
  // DQ (9/11/2009): 

  // typedef std::vector<SgNode*> NumberToStatementType;
     typedef SgNodePtrList NumberToStatementType;
  // NumberToStatementType numberToStatementContainer;

     class NumberStatementsTraversal : public AstSimpleProcessing
        {
          public:
               NumberToStatementType & local_numberToStatementContainer;

               NumberStatementsTraversal(NumberToStatementType & x)
                  : local_numberToStatementContainer(x) {}

               void visit ( SgNode* astNode )
                  {
                    static int statementCounter = 0;
                    SgStatement* statement = isSgStatement(astNode);
                    if (statement != NULL)
                       {
                         local_numberToStatementContainer.push_back(statement);

                      // DQ (9/12/2009): this fails in make check in directory: "projects/autoTuning".
                      // ROSE_ASSERT(local_numberToStatementContainer[statementCounter] == statement);

                         statementCounter++;
                       }
                  }
        };

  // DQ (9/11/2009): Measure the cost of the renumbering to support the name qualification.
     TimingPerformance timer ("Statement numbering (name qualification support):");

     SgSourceFile* file = (SgSourceFile*) this;
  // NumberStatementsTraversal t(numberToStatementContainer);
     NumberStatementsTraversal t(p_statementNumberContainer);

     t.traverse(file,preorder);

#if 0
  // This provides a count of the total number of statements.
  // printf ("numberToStatementContainer.size() = %zu \n",numberToStatementContainer.size());
     printf ("p_statementNumberContainer.size() = %zu \n",p_statementNumberContainer.size());
#endif

#if 0
     printf ("Exiting in SgSourceFile::buildStatementNumbering() \n");
     ROSE_ASSERT(false);
#endif
   }



void
// SgSourceFile::
// buildStatementNumbering( SgSourceFile::NumberToStatementType & numberToStatementContainer )
fixupUsingNamespaceDeclarations()
   {
  // DQ (9/11/2009): STart of support for processing using declarations.

#if 0
     printf ("Exiting in SgSourceFile::fixupUsingNamespaceDeclarations() \n");
     ROSE_ASSERT(false);
#endif
   }

