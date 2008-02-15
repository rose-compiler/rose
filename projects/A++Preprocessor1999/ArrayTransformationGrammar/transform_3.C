#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"

#include "ROSE_Transformations.h"

// include "transformation_3.h"

void
ArrayAssignmentUsingTransformationGrammar::transform ( ROSE_TransformableStatementBlock* roseProgramTree )
   {
  // This function organizes the transformation of the ROSE Program Tree
  // Not sure if we want to have this function return the rootNode?

     ROSE_TransformableNode* rootNode = roseProgramTree->transform();
     ROSE_ASSERT (rootNode != NULL);
   }

// StatementDataBase*
void
ArrayAssignmentUsingTransformationGrammar::setUpDataBase( ROSE_TransformableStatementBlock* roseProgramTree )
   {
  // This function sets up the local data base of information specific to this
  // statement.  We store information about:
  //    number of Rhs operands
  //    operands
  //        names

     StatementDataBase::traverse (roseProgramTree);

#if 0
     printf ("Sorry, not implemented! ArrayAssignmentUsingTransformationGrammar::setUpDataBase() \n");
     ROSE_ABORT();
#endif
  // return localDataBase;
   }


