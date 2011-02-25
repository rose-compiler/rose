// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupSourcePositionInformation.h"
/*
   This file constains the code to fixup the source position information in the AST.
   By default we store the information that we bather from EDG, however this information
   is incomplete and misses a number of details that are important within ROSE.
   Incomplete aspects include (likely because EDG is not trying to support source-to-source 
   details):
      1) namespaces lack the postion information about the location of the keyword ("namespace").
      2) constants are often missing source postion information (when not constant folded).
         Constant folded constants are of course not in the source code and so are marked
         as compiler generated within ROSE.
      3) paramter lists (i.e. positions of "(" and ")")
      4) the location of the ";" is often unavialable (or not properly extracted from EDG, 
         since I think it might be present in come cases).

   These details aside, EDG is in general extremely good about saveing source position information
   (and in great details, more detail that we require in ROSE).
   
   However, in ROSE we want the start and end of each expression and statement (at least the statements).
   To get this information we have built a traversal which uses synthesized attributes to compute the
   farthest ending position of any part of a subtree represented by a statement and we use that for
   the end of the statement (the same is done for the start of a statement, though this is in general
   already correct).

   For expressions this approach would cause the location of operators to be lost, so we still have to 
   address this.

   Suggestions for better handling of source positoon information in ROSE:
      1) A number of declarations consist of multiple parts and when we store only the start and end 
         of the construct we miss the positions of the different parts.  EDG separates out the positions
         of the identifier, any specifiers, and the declarator or enum value.  We could save this 
         information at the cost of adding to the memory requirements of the AST.

 */

void fixupSourcePositionInformation ( SgNode* node )
   {
     TimingPerformance timer ("Fixup source position information:");

  // This simplifies how the traversal is called!
     FixupSourcePositionInformation astFixupTraversal;

     FixupSourcePositionInformationInheritedAttribute inheritedAttribute;

#if 0
     printf ("In fixupSourcePositionInformation(): Generate the dot output of the SAGE III AST  (node = %s) \n",node->class_name().c_str());
     SgProject* project = isSgProject(node->get_parent());
     ROSE_ASSERT (project != NULL);
     generateDOT ( *project );
#endif

  // I think the default should be preorder so that the interfaces would be more uniform

  // While we debug this and other code we need to be able to optionally turn this on and off.
#if 1
     astFixupTraversal.traverse(node,inheritedAttribute);
#else
     printf ("fixupSourcePositionInformation traversal was SKIPPED \n");
#endif
   }

FixupSourcePositionInformationInheritedAttribute::
FixupSourcePositionInformationInheritedAttribute()
   {
  // Default Constructor
   }

FixupSourcePositionInformationInheritedAttribute
FixupSourcePositionInformation::evaluateInheritedAttribute 
   ( SgNode* node, FixupSourcePositionInformationInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In FixupSourcePositionInformation::evaluateInheritedAttribute(): node = %p = %s \n",node,node->sage_class_name());
#endif

  // DQ (10/27/2007): Added as a test!
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
          ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);
        }

     return inheritedAttribute;
   }


FixupSourcePositionInformationSynthesizedAttribute::
FixupSourcePositionInformationSynthesizedAttribute( const FixupSourcePositionInformationSynthesizedAttribute & X )
   : startingPosition(X.startingPosition), endingPosition(X.endingPosition), associatedNode(X.associatedNode)
   {
  // Copy Constructor
  // printf ("Calling FixupSourcePositionInformationSynthesizedAttribute copy constructor (this = %p, X = %p) \n",this,&X);
#if 0
     ROSE_ASSERT(associatedNode != NULL);
     ROSE_ASSERT(startingPosition != NULL);
     ROSE_ASSERT(endingPosition != NULL);
#endif
   }

FixupSourcePositionInformationSynthesizedAttribute::
FixupSourcePositionInformationSynthesizedAttribute()
   : startingPosition(NULL), endingPosition(NULL), associatedNode(NULL)
   {
  // Default Constructor
  // printf ("Calling FixupSourcePositionInformationSynthesizedAttribute default constructor (this = %p) \n",this);
   }

void
FixupSourcePositionInformation::fixupCompilerGeneratedNodesToBeUniformlyMarked ( SgLocatedNode* locatedNode )
   {
     SgExpression* expression = isSgExpression(locatedNode);
     if (locatedNode->get_startOfConstruct()->isCompilerGenerated() == true)
        {
          if (locatedNode->get_endOfConstruct()->isCompilerGenerated() == false)
             {
               printf ("Warning: expression->get_endOfConstruct()->isCompilerGenerated() == false (IR node not uniformally marked as compiler generated) \n");
               locatedNode->get_endOfConstruct()->setCompilerGenerated();
             }

       // If this is an expression then make sure that the operatorPosition is uniformally marked.
          if (expression != NULL)
             {
            // ROSE_ASSERT(expression->get_operatorPosition() != NULL);
               if (expression->get_operatorPosition() != NULL && expression->get_operatorPosition()->isCompilerGenerated() == false)
                  {
                    printf ("Warning: expression->get_operatorPosition()->isCompilerGenerated() == false (IR node not uniformally marked as compiler generated) \n");
                    expression->get_operatorPosition()->setCompilerGenerated();
                  }
             }
        }
       else
        {
       // Make sure that these are what we expect
          ROSE_ASSERT(locatedNode->get_startOfConstruct()->isCompilerGenerated() == false);
          ROSE_ASSERT(locatedNode->get_endOfConstruct()->isCompilerGenerated()   == false);

       // If this is an expression then make sure that the operatorPosition is uniformally marked.
          if (expression != NULL)
             {
               if (expression->get_operatorPosition() != NULL)
                  {
                    ROSE_ASSERT(expression->get_operatorPosition()->isCompilerGenerated() == false);
                  }
                 else
                  {
                 // This is allowed since it does not make sense for all expressions!
                 // printf ("In fixupCompilerGeneratedNodesToBeUniformlyMarked(): expression = %p = %s has no operatorPosition information \n",expression,expression->class_name().c_str());
                  }
            // ROSE_ASSERT(expression->get_operatorPosition() != NULL);
             }
        }
   }


void
FixupSourcePositionInformation::fixupSourcePositionUnavailableInFrontendToBeUniformlyMarked ( SgLocatedNode* locatedNode )
   {
  // Note that the isSourcePositionUnavailableInFrontend information can be set uniformally, but is not always true uniformally.
  // However, if the starting position is set so that isSourcePositionUnavailableInFrontend == true then the ending positon should be
  // marked similarly.  There are however, case were the ending position in unavailable even if the starting position is known.

     SgExpression* expression = isSgExpression(locatedNode);
     if (locatedNode->get_startOfConstruct()->isSourcePositionUnavailableInFrontend() == true)
        {
          if (locatedNode->get_endOfConstruct()->isSourcePositionUnavailableInFrontend() == false)
             {
               printf ("Warning: expression->get_endOfConstruct()->isCompilerGenerated() == false (IR node not uniformally marked as SourcePositionUnavailableInFrontend) \n");
               locatedNode->get_endOfConstruct()->setSourcePositionUnavailableInFrontend();
             }

       // If this is an expression then make sure that the operatorPosition is uniformally marked.
          if (expression != NULL)
             {
            // ROSE_ASSERT(expression->get_operatorPosition() != NULL);
               if (expression->get_operatorPosition() != NULL && expression->get_operatorPosition()->isSourcePositionUnavailableInFrontend() == false)
                  {
                    printf ("Warning: expression->get_operatorPosition()->isCompilerGenerated() == false (IR node not uniformally marked as SourcePositionUnavailableInFrontend) \n");
                    expression->get_operatorPosition()->setSourcePositionUnavailableInFrontend();
                  }
             }
        }
       else
        {
       // Make sure that these are what we expect
          ROSE_ASSERT(locatedNode->get_startOfConstruct()->isSourcePositionUnavailableInFrontend() == false);
       // ROSE_ASSERT(locatedNode->get_endOfConstruct()->isSourcePositionUnavailableInFrontend()   == false);

       // If this is an expression then make sure that the operatorPosition is uniformally marked.
          if (expression != NULL)
             {
               if (expression->get_operatorPosition() != NULL)
                  {
                    ROSE_ASSERT(expression->get_operatorPosition()->isSourcePositionUnavailableInFrontend() == false);
                  }
                 else
                  {
                 // This is allowed since it does not make sense for all expressions!
                 // printf ("In fixupCompilerGeneratedNodesToBeUniformlyMarked(): expression = %p = %s has no operatorPosition information \n",expression,expression->class_name().c_str());
                  }
            // ROSE_ASSERT(expression->get_operatorPosition() != NULL);
             }
        }
   }



FixupSourcePositionInformationSynthesizedAttribute
FixupSourcePositionInformation::evaluateSynthesizedAttribute 
   ( SgNode* node, FixupSourcePositionInformationInheritedAttribute ia, SynthesizedAttributesList l )
   {
#if 0
     printf ("In evaluateSynthesizedAttribute(): node = %p = %s \n",node,node->class_name().c_str());
#endif

     FixupSourcePositionInformationSynthesizedAttribute returnAttribute;
  // printf ("returnAttribute = %p \n",&returnAttribute);

     returnAttribute.associatedNode = node;
     ROSE_ASSERT(returnAttribute.associatedNode != NULL);

  // SgGlobal* globalScope      = isSgGlobal(node);
  // if (locatedNode != NULL && globalScope == NULL)
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
       // DQ (12/23/2006): This can not be an error since transformations commonly don't test an 
       // endOfConstruct Sg_File_Info object.  Thus we can either fix it up or ignore it, it is 
       // easier to fix it up for now, thought this may be a performance issue at some point.
          if (locatedNode->get_endOfConstruct() == NULL)
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("Note: locatedNode->get_endOfConstruct() == NULL (locatedNode = %p = %s), generating new Sg_File_Info object ... \n",locatedNode,locatedNode->class_name().c_str());
#endif
            // ROSE_ASSERT(false);

               ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
               Sg_File_Info* newFileInfo = new Sg_File_Info(*(locatedNode->get_startOfConstruct()));
               locatedNode->set_endOfConstruct(newFileInfo);

            // DQ (12/17/2006): Set the parent of any new endOfConstruct
               newFileInfo->set_parent(locatedNode);
             }

       // This test will fail for SgGlobal (we should fix that!)
          if (isSgGlobal(node) == NULL && isSgFile(node) == NULL)
             {
               if (locatedNode->get_startOfConstruct()->ok() == false || locatedNode->get_endOfConstruct()->ok() == false)
                  {
                    locatedNode->get_startOfConstruct()->display("locatedNode->get_startOfConstruct()");
                    locatedNode->get_endOfConstruct()->display("locatedNode->get_endOfConstruct()");
                  }

               ROSE_ASSERT(locatedNode->get_startOfConstruct()->ok() == true);
               ROSE_ASSERT(locatedNode->get_endOfConstruct()->ok()   == true);
             }

       // Make sure that the starting and ending source positions are marked uniformally (testing compiler generated flag).
          fixupCompilerGeneratedNodesToBeUniformlyMarked (locatedNode);

       // Make sure that isSourcePositionUnavailableInFrontend is marked uniformally.
          fixupSourcePositionUnavailableInFrontendToBeUniformlyMarked ( locatedNode );

          returnAttribute.startingPosition = locatedNode->get_startOfConstruct();
          returnAttribute.endingPosition   = locatedNode->get_endOfConstruct();

       // This test will fail for SgGlobal (we should fix that!)
          if (isSgGlobal(node) == NULL && isSgFile(node) == NULL)
             {
               ROSE_ASSERT(returnAttribute.startingPosition->ok() == true);
               ROSE_ASSERT(returnAttribute.endingPosition->ok() == true);
             }

       // Avoid processing the global scope since it is already set
          SgGlobal* globalScope = isSgGlobal(node);
          if (globalScope == NULL)
             {
               processChildSynthesizedAttributes ( returnAttribute, l );
             }

       // This test will fail for SgGlobal (we should fix that!)
          if (isSgGlobal(node) == NULL && isSgFile(node) == NULL)
             {
               ROSE_ASSERT(returnAttribute.startingPosition->ok() == true);
               ROSE_ASSERT(returnAttribute.endingPosition->ok() == true);
             }

       // Handle special cases:
       // This case applies to class definitions and should work for templated classes as well.
          SgClassDefinition* classDefinition = isSgClassDefinition(node);
          if (classDefinition != NULL)
             {
            // Set the end of the class definition to match the associated class declaration (access it through the parent).
            // printf ("Resetting special case of ending position of SgClassDefinition to match the parent SgClassDeclaration \n");
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classDefinition->get_parent());
               ROSE_ASSERT (classDeclaration != NULL);
               ROSE_ASSERT(classDefinition->get_startOfConstruct() != classDefinition->get_endOfConstruct());
               *(classDefinition->get_endOfConstruct()) = *(classDeclaration->get_endOfConstruct());
               ROSE_ASSERT(classDefinition->get_endOfConstruct()->ok() == true);
            // classDefinition->get_endOfConstruct()->display("Resetting special case of ending position of SgClassDefinition to match the parent SgClassDeclaration");
             }

       // If this is a member function then reach into it and reset a few details.
          SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(node);
          if (memberFunctionDeclaration != NULL)
             {
               SgCtorInitializerList* ctorInitializationList = isSgCtorInitializerList(memberFunctionDeclaration->get_CtorInitializerList());
               if (ctorInitializationList != NULL)
                  {
                 // Sg_File_Info* startingFileInfo = ctorInitializationList->get_startOfConstruct();
                    if (ctorInitializationList->get_ctors().empty() == true)
                       {
                      // As a compiler generate IR nodes the source position is not defined.
                      // printf ("Marking SgCtorInitializerList in SgMemberFunctionDeclaration to be compiler generated (since it is empty()) \n");
                      // startingFileInfo->setCompilerGenerated();
                         ctorInitializationList->setCompilerGenerated();
                       }
                  }
                 else
                  {
                    printf ("ctorInitializationList == NULL for memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
                  }
               
             }
        }

  // Handle the case of a SgInitializedName, which is not currently a SgLocatedNode
     SgInitializedName* initializedName = isSgInitializedName(node);
     if (initializedName != NULL)
        {
       // initializedName->get_file_info()->display("initializedName: debug");

          ROSE_ASSERT(initializedName->get_startOfConstruct() != NULL);
          ROSE_ASSERT(initializedName->get_startOfConstruct()->ok() == true);

          returnAttribute.startingPosition = initializedName->get_startOfConstruct();
          returnAttribute.endingPosition   = initializedName->get_startOfConstruct();

          processChildSynthesizedAttributes ( returnAttribute, l );
        }

  // Handle the case of a SgInitializedName, which is not currently a SgLocatedNode
     SgPragma* pragma = isSgPragma(node);
     if (pragma != NULL)
        {
       // initializedName->get_file_info()->display("initializedName: debug");

          ROSE_ASSERT(pragma->get_startOfConstruct() != NULL);
          ROSE_ASSERT(pragma->get_startOfConstruct()->ok() == true);

          returnAttribute.startingPosition = pragma->get_startOfConstruct();
          returnAttribute.endingPosition   = pragma->get_startOfConstruct();

          processChildSynthesizedAttributes ( returnAttribute, l );
        }

  // Handle the case of a SgFile, which is not a SgLocatedNode
     SgFile* file = isSgFile(node);
     if (file != NULL)
        {
          ROSE_ASSERT(file->get_file_info() != NULL);

          if (file->get_file_info()->ok() == false) {
               file->get_file_info()->display("file: debug");
          }

          ROSE_ASSERT(file->get_file_info()->ok() == true);

          returnAttribute.startingPosition = file->get_file_info();
          returnAttribute.endingPosition   = file->get_file_info();
        }

     ROSE_ASSERT(returnAttribute.startingPosition != NULL);
     ROSE_ASSERT(returnAttribute.endingPosition != NULL);

     ROSE_ASSERT(returnAttribute.associatedNode != NULL);
#if 0
     printf ("Leaving evaluateSynthesizedAttribute(): node = %p = %s \n",node,node->sage_class_name());
     returnAttribute.startingPosition->display("Leaving evaluateSynthesizedAttribute(): returnAttribute.startingPosition");
     returnAttribute.endingPosition->display("Leaving evaluateSynthesizedAttribute(): returnAttribute.endingPosition");
#endif

  // This test will fail for SgGlobal (we should fix that!)
     if (isSgGlobal(node) == NULL && isSgFile(node) == NULL)
        {
       // DQ (12/22/2006): This test fails for Qing's loopProcessor (and the translators tests).
          ROSE_ASSERT(returnAttribute.startingPosition->ok() == true);
          ROSE_ASSERT(returnAttribute.endingPosition->ok() == true);
        }

  // DQ (12/21/2006): Only test for "<=" if these are valid positions from the front-end.
     if (returnAttribute.startingPosition->isSourcePositionUnavailableInFrontend() == false && 
         returnAttribute.endingPosition->isSourcePositionUnavailableInFrontend() == false) 
        {
       // This will fail where the end of an expression or statement is in a different file than the start.
       // ROSE_ASSERT( *(returnAttribute.startingPosition) <= *(returnAttribute.endingPosition));
          if ( (*(returnAttribute.startingPosition) <= *(returnAttribute.endingPosition)) == false )
             {
               printf ("Error: (*(returnAttribute.startingPosition) <= *(returnAttribute.endingPosition)) == false \n");
               SgNode* tempNode = returnAttribute.startingPosition->get_parent();
               printf ("returnAttribute.startingPosition->get_parent() = %p = %s \n",tempNode,tempNode->class_name().c_str());
               returnAttribute.startingPosition->display("Leaving evaluateSynthesizedAttribute(): returnAttribute.startingPosition");
               returnAttribute.endingPosition->display("Leaving evaluateSynthesizedAttribute(): returnAttribute.endingPosition");
             }
          ROSE_ASSERT( *(returnAttribute.startingPosition) <= *(returnAttribute.endingPosition));
        }
#if 0
       else
        {
       // This is allowable since the ending position is not always available even when the starting position information is available.
          SgNode* tempNode = returnAttribute.startingPosition->get_parent();
          printf ("Note: isSourcePositionUnavailableInFrontend() == false --- returnAttribute.startingPosition->get_parent() = %p = %s \n",tempNode,tempNode->class_name().c_str());
        }
#endif

     return returnAttribute;
   }





void
FixupSourcePositionInformation::processChildSynthesizedAttributes
   ( FixupSourcePositionInformationSynthesizedAttribute & returnAttribute, SynthesizedAttributesList & l )
   {
  // This function handles the details of evaluating the list of child attributes and updating the returnAttribute
  // with the correct information. We need this function because not all IR nodes that contain source file position
  // information are derived from SgLocatedNode (though the IR should be changed to reflect this, I think).
#if 0
     printf ("In processChildSynthesizedAttributes(): returnAttribute.associatedNode = %p = %s \n",
          returnAttribute.associatedNode,
          (returnAttribute.associatedNode != NULL) ? returnAttribute.associatedNode->class_name().c_str() : "N/A");
#endif
     ROSE_ASSERT(returnAttribute.associatedNode != NULL);

     Sg_File_Info* subTreeStartingPosition = NULL;
     Sg_File_Info* subTreeEndingPosition   = NULL;

  // DQ (1/6/2007): Leave SgBasicBlock IR nodes alone so that we can contain errors in the source possition to a scope.
  // else a wrong source position can change the source position of statements very far away from the location of the 
  // actual problem (see test2007_02.C).  We actually trust the position from EDG which we use for setting the SgBasicBlock
  // so skip modifiying it since it is likely an error to do so.  Later we want to trap locations where the source position 
  // of SgBasicBlocks might be reset because that will help us locate errors in the source positions of other IR nodes.
     SgBasicBlock* block = isSgBasicBlock(returnAttribute.associatedNode);
     if (block != NULL)
        {
       // printf ("Leave SgBasicBlock IR nodes with unmodified source positions to help contain possible errors to the local scope where they occur. \n");
          return;
        }

  // printf ("SynthesizedAttributesList size() = %ld \n",l.size());
     for (SynthesizedAttributesList::iterator i = l.begin(); i != l.end(); i++)
        {
       // compute the min and max Sg_File_Info object.
       // ROSE_ASSERT( (*i).associatedNode != NULL);

       // DQ (11/1/2006): This should not be required, but a bad synthesized attribute 
       // is being built (using the default constructor) and so associatedNode field is NULL.
          if ( (*i).associatedNode != NULL )
             {
            // printf ("Processing synthesized attribute from (*i).associatedNode = %p = %s \n",(*i).associatedNode,(*i).associatedNode->class_name().c_str());

            // handle the starting positon first
               if (subTreeStartingPosition == NULL)
                  {
                    subTreeStartingPosition = (*i).startingPosition;
                    ROSE_ASSERT(subTreeStartingPosition != NULL);
                  }
                 else
                  {
                    ROSE_ASSERT(subTreeStartingPosition != NULL);
                    ROSE_ASSERT((*i).startingPosition != NULL);

                 // This code uses the relational operators defined for the Sg_File_Info class (these are not defined where the source files are different).
                    subTreeStartingPosition = (*((*i).startingPosition) < *subTreeStartingPosition) ? (*i).startingPosition : subTreeStartingPosition;
                  }

            // handle the ending positon
               if (subTreeEndingPosition == NULL)
                  {
                    subTreeEndingPosition = (*i).endingPosition;
                    ROSE_ASSERT(subTreeEndingPosition != NULL);
                  }
                 else
                  {
                    ROSE_ASSERT(subTreeEndingPosition != NULL);
                    ROSE_ASSERT((*i).endingPosition != NULL);

                 // This code uses the relational operators defined for the Sg_File_Info class (these are not defined where the source files are different).
                    subTreeEndingPosition = (*((*i).endingPosition) > *subTreeEndingPosition) ? (*i).endingPosition : subTreeEndingPosition;
                  }
             }
            else
             {
            // It appears that this occurs where there are function declarations withouth a valid pointer 
            // to the function definition and so a default synthesized attribute is constructed in the 
            // array of child synthesized attributes.
            // printf ("Warning, this unknown synthesized attribute should never have been built ... \n");
             }               
        }

#if 0
     if (subTreeStartingPosition != NULL)
          subTreeStartingPosition->display("subTreeStartingPosition: debug");
       else
          printf ("subTreeStartingPosition == NULL \n");

     if (subTreeEndingPosition != NULL)
          subTreeEndingPosition->display("subTreeEndingPosition: debug");
       else
          printf ("subTreeEndingPosition == NULL \n");
#endif

  // These variables are created to make the code more clear, we could just update
  // returnAttribute.startingPosition and returnAttribute.endingPosition directly.
     Sg_File_Info* currentStartingPosition = returnAttribute.startingPosition;
     Sg_File_Info* currentEndingPosition   = returnAttribute.endingPosition;

     ROSE_ASSERT(currentStartingPosition != NULL);
     ROSE_ASSERT(currentEndingPosition   != NULL);
#if 0
     currentStartingPosition->display("currentStartingPosition: debug");
     currentEndingPosition->display("currentEndingPosition: debug");
#endif

  // Let these be the default values and then update these if the subtree has a better value.
  // returnAttribute.startingPosition = currentStartingPosition;
  // returnAttribute.endingPosition   = currentEndingPosition;

  // Now fixup the returnAttribute's source position information using that from the subtree (child, synthizied, attributes).
     if (subTreeStartingPosition != NULL)
        {
       // Note that > is not the same as !(<=) since the file names must match for any relational operator to return the value of "true".
       // returnAttribute.startingPosition = (*currentStartingPosition < *subTreeStartingPosition) ? currentStartingPosition : subTreeStartingPosition;
          if (*currentStartingPosition < *subTreeStartingPosition)
             {
            // printf ("*currentStartingPosition < *subTreeStartingPosition \n");
               returnAttribute.startingPosition = currentStartingPosition;
             }
            else
             {
            // This does not really imply that *currentStartingPosition >= *subTreeStartingPosition
            // printf ("NOT *currentStartingPosition < *subTreeStartingPosition \n");
               returnAttribute.startingPosition = subTreeStartingPosition;
             }
        }

  // Now fixup the returnAttribute's source position information using that from the subtree (child, synthizied, attributes).
     if (subTreeEndingPosition != NULL)
        {
       // Note that > is not the same as !(<=) since the file names must match for any relational operator to return the value of "true".
       // returnAttribute.endingPosition   = (*currentEndingPosition   > *subTreeEndingPosition)   ? currentEndingPosition   : subTreeEndingPosition;
          if (*currentEndingPosition > *subTreeEndingPosition)
             {
            // printf ("*currentEndingPosition > *subTreeEndingPosition \n");
               returnAttribute.endingPosition = currentEndingPosition;
             }
            else
             {
            // This does not really imply that *currentEndingPosition <= *subTreeEndingPosition
            // printf ("NOT *currentEndingPosition > *subTreeEndingPosition \n");
            // ROSE_ASSERT (*currentEndingPosition < *subTreeEndingPosition);
               returnAttribute.endingPosition = subTreeEndingPosition;
             }
        }

  // Now adjust the source position information if required for make it correct (we only modify the line number and column numbers).
     if (subTreeStartingPosition != NULL && *currentStartingPosition > *subTreeStartingPosition)
        {
#if 0
          printf ("Warning: position of returnAttribute.associatedNode = %p = %s has incorrect STARTING source code position information \n",
               returnAttribute.associatedNode,returnAttribute.associatedNode->class_name().c_str());
       // currentStartingPosition->display("current location information");
          subTreeStartingPosition->display("better computed location information");
#endif
#if 1
       // DQ (12/22/2006): This may be a source of errors when the update changes the flags that 
       // classify the IR node (e.g. compiler generated, output in code generation, etc.).
       // printf ("Skipping reset of Sg_File_Info object (update start position) \n");
       // *currentStartingPosition = *subTreeStartingPosition;
          currentStartingPosition->updateSourcePosition(subTreeStartingPosition);
#else
       // Leave SgBasicBlock IR nodes alone so that we can contain errors in the source possition to a scope.
       // else a wrong source position can change the source position of statements very far away from the 
       // location of the actual problem (see test2007_02.C).
          SgBasicBlock* block = isSgBasicBlock(returnAttribute.associatedNode);
          if (block != NULL)
             {
             }
          currentStartingPosition->updateSourcePosition(subTreeStartingPosition);
#endif
        }
#if 0
    // Debugging code (leave in place for now)
       else
        {
          if (subTreeStartingPosition != NULL)
             {
               printf ("subTreeStartingPosition != NULL: IR node appears to be OK, does not need to be reset \n");
               subTreeStartingPosition->display("not as good of a START solution");
             }
            else
             {
               printf ("Note: subTreeStartingPosition == NULL: no valid leaves in this subtree \n");
             }
        }
#endif

  // Now adjust the source position information if required for make it correct (we only modify the line number and column numbers).
     if (subTreeEndingPosition != NULL && *currentEndingPosition < *subTreeEndingPosition)
        {
#if 0
          printf ("Warning: position of returnAttribute.associatedNode = %p = %s has incorrect ENDING source code position information \n",
               returnAttribute.associatedNode,returnAttribute.associatedNode->class_name().c_str());
          currentEndingPosition->display("current location information");
          subTreeEndingPosition->display("better computed location information");
#endif
       // DQ (12/22/2006): This may be a source of errors when the update changes the flags that 
       // classify the IR node (e.g. compiler generated, output in code generation, etc.).
       // printf ("Skipping reset of Sg_File_Info object (update end position) \n");
       // *currentEndingPosition = *subTreeEndingPosition;
          currentEndingPosition->updateSourcePosition(subTreeEndingPosition);
        }
#if 0
    // Debugging code (leave in place for now)
       else
        {
          if (subTreeEndingPosition != NULL)
             {
               printf ("subTreeEndingPosition != NULL: IR node appears to be OK, does not need to be reset \n");
               subTreeEndingPosition->display("not as good of an END solution");
             }
            else
             {
               printf ("Note: subTreeEndingPosition == NULL: no valid leaves in this subtree \n");
             }
        }
#endif
   }



void
fixupSourcePositionConstructs()
   {
  // DQ (3/10/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup source position constructs:");

     FixupSourcePositionConstructs t;

  // Traverse all the memory pools for now!
     t.traverseMemoryPool();
   }

void
FixupSourcePositionConstructs::visit(SgNode* node)
   {

#if 0
     printf ("##### FixupSourcePositionConstructs::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

  // DQ (10/27/2007): Added as a test!
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
#if 0
          if (isSgExprStatement(locatedNode) != NULL)
               printf ("FixupSourcePositionConstructs::visit(): Testing start and end of construct Sg_File_Info objects for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif
          ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
          if (locatedNode->get_endOfConstruct() == NULL)
             {
               printf ("FixupSourcePositionConstructs::visit(): Building a matching endOfConstruct to the available startOfConstruct for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
               locatedNode->set_endOfConstruct( new Sg_File_Info (* ( locatedNode->get_startOfConstruct() ) ) );
             }
          ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);
        }
   }

