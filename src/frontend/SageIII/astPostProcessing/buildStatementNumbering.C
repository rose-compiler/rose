// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#if 1
// DQ (9/12/2009): Adding support for new name qualification (not ready yet).

void
SgGlobal::buildStatementNumbering()
   {
  // Initialize the source sequence numbers located in each statement.

     class NumberStatementsTraversal : public AstSimpleProcessing
        {
          public:
               int statementCounter;

               NumberStatementsTraversal(int value) : statementCounter(value) {}

               void visit ( SgNode* astNode )
                  {
                    SgStatement* statement = isSgStatement(astNode);
                    if (statement != NULL)
                       {
                         statement->set_source_sequence_value(statementCounter);
                         statementCounter++;
                       }
                  }
        };

  // DQ (9/11/2009): Measure the cost of the renumbering to support the name qualification.
     TimingPerformance timer ("Statement sequence numbering (name qualification support):");

     SgSourceFile* file = (SgSourceFile*) this;
     NumberStatementsTraversal t(0);

     t.traverse(file,preorder);

#if 0
     printf ("Exiting in SgSourceFile::buildStatementNumbering() \n");
     ROSE_ABORT();
#endif
   }


class NumberStatementsTraversal : public AstSimpleProcessing
   {
     public:
          int statementCounter;

          NumberStatementsTraversal(int value) : statementCounter(value) {}

          void visit ( SgNode* astNode )
             {
               SgStatement* statement = isSgStatement(astNode);
               if (statement != NULL)
                  {
                    statement->set_source_sequence_value(statementCounter);
                    statementCounter++;
                  }
             }
   };


void
fixupUsingNamespaceDeclarations()
   {
  // DQ (9/11/2009): STart of support for processing using declarations.

#if 0
     printf ("Exiting in SgSourceFile::fixupUsingNamespaceDeclarations() \n");
     ROSE_ABORT();
#endif
   }

#endif
