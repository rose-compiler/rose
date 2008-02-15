// Include rose.h so we can manipulate ROSE objects to recognize and do the transformations
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rose.h>
#include "TransformationBase.h"
#include "supported_dimensions.h"

// This file contains functions within the base class (for all the transformations)
// that are specific to the editing of the Sage program tree.

char*
ROSE_TransformationBase::transformationHeaderFile()
   {
     ROSE_ASSERT (headerFileName != NULL);
     return headerFileName;
   }

Boolean
ROSE_TransformationBase::templatesDataSetup()
   {
  // Return either TRUE or FALSE
     Boolean returnValue =
          ((targetTemplateFunction != NULL) && (transformationTemplateFunction != NULL)) ? TRUE : FALSE;
     for (int dim = 0; dim < ROSE_MAX_ARRAY_DIMENSION; dim++)
        {
          if (subscriptFunctionDeclarationStatement [dim] == NULL)
               returnValue = FALSE;
        }

     return returnValue;
   }


SgFunctionCallExp*
ROSE_TransformationBase::extractSubscriptFunctionDeclarationExpression ( SgVariableDeclaration* X )
   {
     SgFunctionCallExp* returnFunctionCall = NULL;
   
  // showSgStatement ( cout, X , "Called from  ROSE_TransformationBase::extractSubscriptFunctionDeclarationExpression()");

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(X);
     ROSE_ASSERT (variableDeclaration != NULL);

     SgInitializedNameList & variableList = variableDeclaration->get_variables();
     ROSE_ASSERT (variableList.size() == 1);

     for (SgInitializedNameList::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
#if 0     // BP : 10/16/2001, g++ cribs
          SgName & name = (*i).get_name();
#else
          const SgName & name = (*i).get_name();
#endif

          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
               cout << "Variable Name = " << name.str() << endl;

        // This is how to get the initializer, copied from the 
        // sage_statements.C (SgVariableDeclaration::printVariables function)
        // This might not always be a valid pointer, though it seems it useually is unless ROSE modifies the program tree
        // so perhaps this is a sign that we are not modifing the program tree correctly.
           ROSE_ASSERT ((*i).get_named_item() != NULL);
           if ((*i).get_named_item() != NULL)
              {
                ROSE_ASSERT ((*i).get_named_item() != NULL);
                SgInitializer *initializerOther = (*i).get_named_item()->get_initializer();

                ROSE_ASSERT (initializerOther != NULL);
                if (initializerOther != NULL)
                   {
                  // There are other things we could ask of the initializer
                     ROSE_ASSERT (initializerOther != NULL);
                  // showSgExpression (os,initializerOther,label,depth+1);
                  // returnFunctionCall = initializerOther->get_operand();

                     SgAssignInitializer* initializerFunction = isSgAssignInitializer(initializerOther);
		     
                     SgExpression* functionOperand = initializerFunction->get_operand();
                     returnFunctionCall = isSgFunctionCallExp(functionOperand);
		     ROSE_ASSERT (returnFunctionCall != NULL);
                   }
                  else
                   {
                     cout << "initializerOther = NULL" << endl;
                     ROSE_ABORT();
                   }
              }
         }

     return returnFunctionCall;
   }


void
ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates ( SgStatement::iterator & s_iter )
   {
  // The initial semantics of this function will be for it to increment the iterator twice (or maybe three times)

   // We will also for the moment assume that we know the layout of the header file
   // which defined the target and transformation templates (we wrote them so this is not unreasonable)
   // specifically we assume that there are three statements:
   //    1) the dummy class definition (so that the functions will compile)
   //    2) the target template
   //    3) the transformation template

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("### Inside of ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates \n");
   
     char outputFilename[256];
     sprintf(outputFilename,"roseTransformationProgramTree.show");

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Transform Source Code: outputFilename = %s \n",outputFilename);

     fstream ROSE_DataFile(outputFilename,ios::out);

     ROSE_DataFile << "\n\n\n" << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "   SUBSCRIPT FUNCTION DEFINITION  " << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "\n\n\n" << endl;

  // print out the showfile associated with the program tree generated for the input statement
  // Show the dummy class definition
     showSgStatement ( ROSE_DataFile, *s_iter,
          "ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates(SgStatement): show dummy class definition");

     printf ("ROSE_DISTINGUISED_NO_OF_DIMENSIONS = %d \n",ROSE_DISTINGUISED_NO_OF_DIMENSIONS);

  // CW: get code template (sage tree) for array index subscript function.
     for (int dim = 0; dim < ROSE_DISTINGUISED_NO_OF_DIMENSIONS; dim++)
        {
          s_iter++;  // skip over the function and get the declaration statement
          subscriptFunctionDeclarationStatement[dim] = isSgVariableDeclaration(*s_iter);
          ROSE_ASSERT (subscriptFunctionDeclarationStatement[dim] != NULL);
          s_iter++;

       // CW : I don't know whether it is necessary to store the declaration above
       // because it seems to be used only here. But I'm not sure.
          subscriptFunctionDeclarationExpression[dim]=
               extractSubscriptFunctionDeclarationExpression(subscriptFunctionDeclarationStatement[dim]);
          ROSE_ASSERT(subscriptFunctionDeclarationExpression[dim] != NULL);
        }

     if (ROSE_DISTINGUISED_NO_OF_DIMENSIONS < ROSE_MAX_ARRAY_DIMENSION)
        {
          s_iter++;  // skip over the function and get the declaration statement
          subscriptFunctionDeclarationStatement[ROSE_MAX_ARRAY_DIMENSION-1] = isSgVariableDeclaration(*s_iter);
          ROSE_ASSERT (subscriptFunctionDeclarationStatement[ROSE_MAX_ARRAY_DIMENSION-1] != NULL);
          s_iter++;

          subscriptFunctionDeclarationExpression[ROSE_MAX_ARRAY_DIMENSION-1]=
               extractSubscriptFunctionDeclarationExpression(subscriptFunctionDeclarationStatement[ROSE_MAX_ARRAY_DIMENSION-1]);
          ROSE_ASSERT(subscriptFunctionDeclarationExpression[ROSE_MAX_ARRAY_DIMENSION-1] != NULL);

          for (int dim = ROSE_DISTINGUISED_NO_OF_DIMENSIONS; dim < ROSE_MAX_ARRAY_DIMENSION-1; dim++)
             {
               subscriptFunctionDeclarationStatement[dim]  = 
                    subscriptFunctionDeclarationStatement [ROSE_MAX_ARRAY_DIMENSION-1];
               subscriptFunctionDeclarationExpression[dim] = 
                    subscriptFunctionDeclarationExpression[ROSE_MAX_ARRAY_DIMENSION-1];
               ROSE_ASSERT(subscriptFunctionDeclarationStatement  [dim] != NULL);
               ROSE_ASSERT(subscriptFunctionDeclarationExpression [dim] != NULL);
             }
        }

     ROSE_DataFile << "\n\n\n" << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "   DUMMY CLASS DEFINITION FOR TEMPLATE   " << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "\n\n\n" << endl;
     
  // print out the showfile associated with the program tree generated for the input statement
  // Show the dummy class definition
     showSgStatement ( ROSE_DataFile, *s_iter, 
          "ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates(SgStatement): show dummy class definition");

  // Skip over the dummy definition of the SimpleArrayAssignment class
     s_iter++;

  // Initialize the target template function pointer (a SgStatement object for now)
     targetTemplateFunction = *s_iter;

     ROSE_DataFile << "\n\n\n" << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "   TARGET FOR TRANSFORMATION TEMPLATE   " << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "\n\n\n" << endl;

  // Show the target template for this transformation
     showSgStatement ( ROSE_DataFile, *s_iter, 
          "ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates(SgStatement): show target");
     s_iter++;

  // Initialize the transformation template function pointer (a SgStatement object for now)
     transformationTemplateFunction = *s_iter;

     ROSE_DataFile << "\n\n\n" << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "   SOURCE FOR TRANSFORMATION TEMPLATE   " << endl;
     ROSE_DataFile << "############################################################" << endl;
     ROSE_DataFile << "\n\n\n" << endl;

  // Show the transformation template for this transformation
     showSgStatement ( ROSE_DataFile, *s_iter, 
          "ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates(SgStatement): show transformation");

  // Now we could go on to separate out the different parts of the target and 
  // transformation template functions but we will leave that for later

  // Build the object representing the internal structure of the target template
     targetTemplateData         = new ROSE_TransformationBase::TargetTemplate (targetTemplateFunction);
     ROSE_ASSERT (targetTemplateData != NULL);

  // Build the object representing the internal structure of the transformation template
     transformationTemplateData = new ROSE_TransformationBase::TransformationTemplate (transformationTemplateFunction);
     ROSE_ASSERT (transformationTemplateData != NULL);


     ROSE_ASSERT (templatesDataSetup() == TRUE);

  // printf ("Exiting at base of ROSE_TransformationBase::setupInternalTargetAndTransformationstemplates(SgStatement::iterator &) \n");
  // ROSE_ABORT();
   }















