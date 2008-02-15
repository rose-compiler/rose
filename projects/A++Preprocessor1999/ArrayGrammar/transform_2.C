#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"

// include "array_class_interface.h"
// include "../TransformBaseClass/TransformationBase.h"
// include "ROSE_Transformations.h"

#include "ROSE_Transformations.h"

// include "transformation_2.h"

void
ArrayAssignmentUsingGrammar::transform ( ROSE_StatementBlock* roseProgramTree )
   {
  // This function organizes the transformation of the ROSE Program Tree
  // Not sure if we want to have this function return the rootNode?
#if 1
     ROSE_Node* rootNode = roseProgramTree->transform();
     ROSE_ASSERT (rootNode != NULL);
#else
  // Here we experiment with another approach implementing a new grammar specific
  // to our specific transformation.  If this works then we will place most of what is currently
  // in transformation_2 into the base class.

     for (int i=0; i < roseProgramTree->numberOfStatements(); i++)
        {

        }
#endif
   }






























