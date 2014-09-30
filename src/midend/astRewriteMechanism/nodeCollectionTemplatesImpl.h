#ifndef ABSTRACT_INTERFACE_NODE_COLLECTION_TEMPLATES_C
#define ABSTRACT_INTERFACE_NODE_COLLECTION_TEMPLATES_C

// #include "rose.h"
#include "rewrite.h"

// ************************************************************
// ************************************************************

template <class T>
void 
AbstractInterfaceNodeCollection<T>::
addString ( SgNode* astNode, const TransformationStringType & transformation )
   {
  // ROSE_ASSERT (transformation.associatedASTNode == NULL);
     ROSE_ASSERT (transformation.getRelativeScope()    != T::unknownScope);
     ROSE_ASSERT (transformation.getRelativePosition() != T::unknownPositionInScope);

     inputTransformationStrings.push_back(transformation);
   }

template <class T>
AbstractInterfaceNodeCollection<T>::AbstractInterfaceNodeCollection()
   {
   }

template <class T>
AbstractInterfaceNodeCollection<T>::~AbstractInterfaceNodeCollection()
   {
   }

template <class T>
AbstractInterfaceNodeCollection<T> &
AbstractInterfaceNodeCollection<T>::operator= ( const AbstractInterfaceNodeCollection<T> & X )
   {
     inputTransformationStrings  = X.inputTransformationStrings;
     sortedTransformationStrings = X.sortedTransformationStrings;
     treeFragement               = X.treeFragement;

     return *this;
   }


template <class T>
int
AbstractInterfaceNodeCollection<T>::numberOfTransformationStrings()
   {
     return inputTransformationStrings.size();
   }

template <class T>
int
AbstractInterfaceNodeCollection<T>::numberOfSortedTransformationStrings()
   {
     int numberOfStrings = 0;
     // pmp 08JUN05
     //   changed to cope with changes in template processing in gcc 3.4
     //   was: for (int i = 0; i < T::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
     for (int i = 0; i < T::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        numberOfStrings += numberOfSortedTransformationStrings(i);
     return numberOfStrings;
   }

template <class T>
int
AbstractInterfaceNodeCollection<T>::numberOfStatements()
   {
     int numberStatements = 0;
     // pmp 08JUN05
     //   changed to cope with changes in template processing in gcc 3.4
     //   was: for (int i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
     for (int i = 0; i < T::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        numberStatements += numberOfStatements(i);
     return numberStatements;
   }

template <class T>
int
AbstractInterfaceNodeCollection<T>::numberOfSortedTransformationStrings( int i )
   {
     return sortedTransformationStrings[i].size();
   }

template <class T>
int
AbstractInterfaceNodeCollection<T>::numberOfStatements( int i )
   {
  // Adjust the size to subtract off the marker declaration statements
     printf ("In AbstractInterfaceNodeCollection<T>::numberOfStatements(): treeFragement[%d].size() = %" PRIuPTR " \n",i,treeFragement[i].size());

  // Note that the unknown case is not implemented and so does 
  // not pickup marker declarations (so subtract off zero).
     int numberOfMarkerDeclarations = (treeFragement[i].size() == 0) ? 0 : 2;
     return treeFragement[i].size() - numberOfMarkerDeclarations;
   }

template <class T>
void
AbstractInterfaceNodeCollection<T>::display ( const char* s ) const
   {
  // I'm a little suprised that this is required!
     display(std::string(s));
   }

template <class T>
void
AbstractInterfaceNodeCollection<T>::display ( const std::string & s ) const
   {
     printf ("In AbstractInterfaceNodeCollection<T>::display(%s): \n",s.c_str());

     int counter = 0;
     printf ("Collection of Input Transformation Strings: \n");
     typename InputTransformationStringCollectionType::const_iterator inputStringIterator;
     printf ("     Input Transformation String Collection: \n");
     for (inputStringIterator = inputTransformationStrings.begin(); 
          inputStringIterator != inputTransformationStrings.end(); 
          inputStringIterator++)
        {
          printf ("     string (%d) = \n%s \n",counter,(*inputStringIterator).getSourceCodeString().c_str());
          counter++;
        }

     printf ("Collection of Sorted Transformation Strings: \n");
     SortedTransformationStringCollectionType::const_iterator sortedStringCollectionIterator;
     counter = 0;
     for (sortedStringCollectionIterator = sortedTransformationStrings.begin(); 
          sortedStringCollectionIterator != sortedTransformationStrings.end(); 
          sortedStringCollectionIterator++)
        {
          std::string name = T::getIntermediateFilePositionName(typename T::IntermediateFileStringPositionEnum(counter));
          printf ("     String Collection (%s): \n",name.c_str());
          std::list<std::string>::const_iterator stringIterator;
          for (stringIterator = (*sortedStringCollectionIterator).begin(); 
               stringIterator != (*sortedStringCollectionIterator).end(); 
               stringIterator++)
             {
                printf ("          string = \n%s \n",(*stringIterator).c_str());
             }
          counter++;
        }

     printf ("Collection of AST Nodes (converted from strings): \n");
     NodeCollectionType::const_iterator nodeCollectionIterator;
     counter = 0;
     for (nodeCollectionIterator = treeFragement.begin(); 
          nodeCollectionIterator != treeFragement.end(); 
          nodeCollectionIterator++)
        {
          std::string name = T::getIntermediateFilePositionName(typename T::IntermediateFileStringPositionEnum(counter));
          printf ("     Node Collection statements (%s): \n",name.c_str());
          SgStatementPtrList::const_iterator statementIterator;
          for (statementIterator = (*nodeCollectionIterator).begin(); 
               statementIterator != (*nodeCollectionIterator).end(); 
               statementIterator++)
             {
                ROSE_ASSERT ((*statementIterator) != NULL);
             // DQ (1/17/2004): Print out all comments along with the statements
             // printf ("          AST node = \n%s \n",(*statementIterator)->unparseToString().c_str());
                printf ("          AST node = \n%s \n",(*statementIterator)->unparseToCompleteString().c_str());
             }
          counter++;
        }
   }

template <class T>
SgStatementPtrList
AbstractInterfaceNodeCollection<T>::getStatementList ( typename T::IntermediateFileStringPositionEnum i )
   {
      ROSE_ASSERT (i >= 0);
     // pmp 08JUN05
     //   changed to cope with changes in template processing in gcc 3.4
     //   was: ROSE_ASSERT (i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG);
      ROSE_ASSERT (i < T::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG);

     return treeFragement[i];
   }

template <class T>
bool
AbstractInterfaceNodeCollection<T>::isInGlobalScope ( SgNode* astNode )
   {
  // Used internally to put transformations into the correct scope in the intermediate
  // file and to know when to skip the use of auxilary declaration in the global preamble.

  // printf ("In HighLevelInterfaceNodeCollection::IntermediateFileGenerator::isInGlobalScope(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

  // Check to see if we want to map to a global scope
     SgStatement* currentStatement = dynamic_cast<SgStatement*>(astNode);
     ROSE_ASSERT (currentStatement != NULL);
     bool currentStatementIsGlobalScope              = dynamic_cast<SgGlobal*>(astNode) != NULL;
     bool currentStatementIsDeclarationInGlobalScope = dynamic_cast<SgGlobal*>(currentStatement->get_parent()) != NULL;

  // Use the global scope in either case
     bool useGlobalScope = currentStatementIsGlobalScope || currentStatementIsDeclarationInGlobalScope;

     return useGlobalScope;
   }


template <class T>
std::list<std::string>
AbstractInterfaceNodeCollection<T>::
getPreambleStrings(
     SgNode* astNode, 
     typename T::ScopeIdentifierEnum lowerBoundScope, 
     typename T::ScopeIdentifierEnum upperBoundScope ) const
   {
  // This function should return a list of strings required for the preamble to 
  // the source code representing the transformation 

     std::list<std::string> returnList;

#if 0
     printf ("In getPreambleStrings(): astFragmentStringContainer.size() = %d \n",
          astFragmentStringContainer.size());
#endif

  // Iterate throgh all the strings and pick out the ones that qualify
     typename InputTransformationStringCollectionType::const_iterator j;
     for (j  = inputTransformationStrings.begin();
          j != inputTransformationStrings.end(); j++)
        {
#if 0
       // debugging code
          (*j).display("In loop over gatheredData");
          printf ("astNode = %p astNode->sage_class_name() = %s \n",astNode,astNode->sage_class_name());
#endif

       // We are only looking for scopes within a specific range since strings 
       // targeting global scopes (e.g. #include, etc.) must be separated from 
       // strings targeting function scopes (e.g. function calls).
//        if ( ( (*j).isSameScope(astNode) == false ) &&
//             ( (*j).relativeScope >= lowerBoundScope ) && ( (*j).relativeScope <= upperBoundScope ) )
          if ( ( (*j).relativeScope >= lowerBoundScope ) && ( (*j).relativeScope <= upperBoundScope ) )
             {
             // This case uses the strings that are NOT targeted at the current scope and 
             // either assembles them into the PREAMBLE if they are targeted at higher level 
             // (parent) scopes OR ignores them if they were associated with lower scopes 
             // (where we can assume they have already been added).
#if 0
                printf ("String located on astNode->sage_class_name() = %s targeted for use at (*j).astNode->sage_class_name() = %s \n",astNode->sage_class_name(),(*j).associatedASTNode->sage_class_name());
#endif

               // pmp 08JUN05
               //   added scope qualifier to Preamble to deal w/ changes in the frontend of gcc 3.4
               //   was: if ( ( (*j).isTargetedForHigherLevelScope(astNode) == true ) || ((*j).relativeScope == Preamble) )
               if ( ( (*j).isTargetedForHigherLevelScope(astNode) == true ) || ((*j).relativeScope == T::Preamble) )
                  {
                 // These strings could contain declarations that we be required by the 
                 // transformations explicitly targeted for this scope so include these
                 // into the preamble (also always copy strings targeted for the preamble 
                 // into the preamble).

                 // found a string for the preamble
                    std::string tempString = (*j).sourceCode;
                    returnList.push_back(tempString);

#if 0
                    printf ("found a string for the preamble \n");
                    ROSE_ABORT();
#endif
                  }
#if 0
                 else
                  {
                 // ignore these strings, since they have already been introduced (in the lower scopes)
                     printf ("Ignoring string already placed into lower (nested) scope ... (exiting) \n");
                     std::string tempString = (*j).sourceCode;
                     printf ("tempString = %s \n",tempString.c_str());
//                   ROSE_ABORT();
                  }
#endif
             }
        }

#if 0
     printf ("Exiting at base of HighLevelRewrite::SynthesizedAttribute::getPreambleStrings() \n");
     ROSE_ABORT();
#endif

     return returnList;
   }

// *********************************************************************
// *********************************************************************
// *********************************************************************
// *********************************************************************

template <class T>
void
AbstractInterfaceNodeCollection<T>::consistancyCheck( const std::string & label ) const
   {
  // This function checks for redundent strings specified for inclusion 
  // into the AST at the same point.  It is designed to catch errors early.

  // Iterate throgh all the strings and pick out the ones that qualify
     int outerCounter = 0;
     typename InputTransformationStringCollectionType::const_iterator i;
     for (i  = inputTransformationStrings.begin();
          i != inputTransformationStrings.end(); i++)
        {
#if 0
       // debugging code
          printf ("(*i).associatedASTNode = %p (*i).associatedASTNode->sage_class_name() = %s \n",
               (*i).associatedASTNode,(*i).associatedASTNode->sage_class_name());
//        (*i).display("In outer loop over gatheredData");
#endif

          int innerCounter = 0;
          typename InputTransformationStringCollectionType::const_iterator j;
          for (j  = inputTransformationStrings.begin();
               j != inputTransformationStrings.end(); j++)
             {
#if 0
            // debugging code
               printf ("(*j).associatedASTNode = %p (*j).associatedASTNode->sage_class_name() = %s \n",
                    (*j).associatedASTNode,(*j).associatedASTNode->sage_class_name());
//             (*j).display("In inner loop over gatheredData");
#endif

            // print out the index values of the list
            // printf ("innerCounter = %d outerCounter = %d \n",innerCounter,outerCounter);

               if ( (innerCounter != outerCounter) && ( (*i).sourceCode == (*j).sourceCode ) )
               {
               // Error: print info and abort!
                  printf ("Error in HighLevelRewrite::SynthesizedAttribute::consistancyCheck: Found two identical sourse strings specified for insertion into AST! \n");
                  printf ("   consistancyCheck(label = %s) \n",label.c_str());
                  printf ("      i->sourceCode = %s \n", i->sourceCode.c_str());
                  printf ("      j->sourceCode = %s \n", j->sourceCode.c_str());
                  ROSE_ABORT();
               }

               innerCounter++;
             }

          outerCounter++;
        }
   }


template <class T>
AttachedPreprocessingInfoType
AbstractInterfaceNodeCollection<T>::getPreprocessingInfoList ( 
   SgStatement* statement, 
   PreprocessingInfo::RelativePositionType position )
   {
  // This function modifies the list of comments/directives to remove 
  // them from the marker declaration (the input statement).

     AttachedPreprocessingInfoType returnCommentAndDirectiveList;

     ROSE_ASSERT (statement != NULL);
     AttachedPreprocessingInfoType* markerCommentsAndDirectives = statement->getAttachedPreprocessingInfo();

     if (markerCommentsAndDirectives != NULL)
        {
       // Search comments for those placed after the starting marker
          AttachedPreprocessingInfoType::iterator i;
          for (i = markerCommentsAndDirectives->begin(); i != markerCommentsAndDirectives->end(); i++)
             {
            // printf ("Relavant comment or directive attached to starting marker declaration = %s \n",(*i)->getString());
               if ( (*i)->getRelativePosition() == position)
                    returnCommentAndDirectiveList.push_back(*i);
             }

       // Not remove the comment/directive from the marker declaration (the input statement)
          for (i = returnCommentAndDirectiveList.begin(); i != returnCommentAndDirectiveList.end(); i++)
             {
            // printf ("Remove the comment/directive from the marker declaration (string = %s) \n",(*i)->getString());

            // DQ (9/25/2007): Moved from std::list to std::vector.
            // markerCommentsAndDirectives->remove(*i);
               markerCommentsAndDirectives->erase(std::find(markerCommentsAndDirectives->begin(),markerCommentsAndDirectives->end(),(*i)));
             }

       // Error checking (make sure that there are no remaining directives at 
       // the marker at the specified position
          for (i = markerCommentsAndDirectives->begin(); i != markerCommentsAndDirectives->end(); i++)
             {
               ROSE_ASSERT ( (*i)->getRelativePosition() != position );
             }
        }

     return returnCommentAndDirectiveList;
   }

template <class T>
void
AbstractInterfaceNodeCollection<T>::stripOffMarkerDeclarations ( SgStatementPtrList & statementList )
   {
  // Markers must be variable declarations (with the correct name as well)
     SgVariableDeclaration* startingMarkerVariableDeclaration = isSgVariableDeclaration(statementList.front());
     ROSE_ASSERT (startingMarkerVariableDeclaration != NULL);

  // DQ (9/25/2007): Moved to std::vector from std::list uniformally within ROSE.
  // statementList.pop_front();
     statementList.erase(statementList.begin());

     SgVariableDeclaration* endingMarkerVariableDeclaration = isSgVariableDeclaration(statementList.back());
     ROSE_ASSERT (endingMarkerVariableDeclaration != NULL);
     statementList.pop_back();

//   AttachedPreprocessingInfoType *startingMarkerCommentsAndDirectives = 
//        startingMarkerVariableDeclaration->getAttachedPreprocessingInfo();
//   AttachedPreprocessingInfoType *endingMarkerCommentsAndDirectives = 
//        endingMarkerVariableDeclaration->getAttachedPreprocessingInfo();

//   ROSE_ASSERT (getPreprocessingInfoList(startingMarkerVariableDeclaration, PreprocessingInfo::after ).size() == 0);
//   ROSE_ASSERT (getPreprocessingInfoList(endingMarkerVariableDeclaration,   PreprocessingInfo::before).size() == 0);
   }

// Possible name:
// getCommentsThatCannotBeAttachedToTransformationStatements
// attachCommentsAtMarkersToTransformationStatementAndReturnUnattachableComments
// processCommentsAndDirectives

template <class T>
void
AbstractInterfaceNodeCollection<T>::markStatementListWithMarkersAsTransformation ( SgStatementPtrList & statementList )
   {
  // Make sure no comments or other preprocessing info are attached to the marker declaration

  // List should have at least a starting and ending marker variable declarations
     ROSE_ASSERT (statementList.size() >= 2);

     if (statementList.size() > 2)
        {
          SgVariableDeclaration* startingMarkerVariableDeclaration = isSgVariableDeclaration(statementList.front());
          SgVariableDeclaration* endingMarkerVariableDeclaration   = isSgVariableDeclaration(statementList.back());

          SgStatementPtrList::iterator firstTransformationStatement;
          SgStatementPtrList::iterator lastTransformationStatement;
          firstTransformationStatement = std::find(statementList.begin(),statementList.end(),(SgStatement*)startingMarkerVariableDeclaration);
          lastTransformationStatement  = std::find(statementList.begin(),statementList.end(),(SgStatement*)endingMarkerVariableDeclaration);

       // Make sure that we found a valid element
          ROSE_ASSERT( firstTransformationStatement != statementList.end() );
          ROSE_ASSERT( lastTransformationStatement  != statementList.end() );

       // Test for if there is a previous element (error if not)
          ROSE_ASSERT( lastTransformationStatement != statementList.begin() );

       // Get the next statement
          firstTransformationStatement++;

       // Backup one statement (need to terminate at last statement (so for loop test will pass for single statement)
       // lastTransformationStatement--;

       // printf ("##### This function (markStatementListWithMarkersAsTransformation) does not yet mark statements: statementList.size() = %" PRIuPTR " \n",statementList.size());
          for (SgStatementPtrList::iterator i = firstTransformationStatement; i != lastTransformationStatement; i++)
             {
            // Use common mechanism to determining if we should output the code in the generation phase
               LowLevelRewrite::markForOutputInCodeGenerationForRewrite (*i);

            // Sg_File_Info* fileInfo = (*i)->get_file_info();
            // ROSE_ASSERT(fileInfo != NULL);
            // (*i)->get_file_info()->setTransformation();
            // (*i)->get_file_info()->set_filenameString();
#if 0
               printf ("Statement to be inserted into AST = %s = %s \n",(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
               (*i)->get_file_info()->display("Inside of markStatementListWithMarkersAsTransformation() debug");
#endif
#if 0
               if (SageInterface::get_name(*i) == "TauTimer")
                  {
                    printf ("Exiting internally for case of class name == TauTimer \n");
                    ROSE_ASSERT(false);
                  }
#endif
             }
        }
   }

template <class T>
AttachedPreprocessingInfoType
AbstractInterfaceNodeCollection<T>::processCommentsAndDirectives ( SgStatementPtrList & statementList )
   {
  // Make sure no comments or other preprocessing info are attached to the marker declaration

  // List should have at least a starting and ending marker variable declarations
     ROSE_ASSERT (statementList.size() >= 2);

     SgVariableDeclaration* startingMarkerVariableDeclaration = isSgVariableDeclaration(statementList.front());
     SgVariableDeclaration* endingMarkerVariableDeclaration   = isSgVariableDeclaration(statementList.back());

  // Get attached preprocessing info (the comments after the starting marker
  // and the comments before the ending marker)
     ROSE_ASSERT(startingMarkerVariableDeclaration != NULL);
     AttachedPreprocessingInfoType relavantStartingMarkerCommentAndDirectiveList
          = getPreprocessingInfoList(startingMarkerVariableDeclaration,PreprocessingInfo::after);

     ROSE_ASSERT(endingMarkerVariableDeclaration   != NULL);
     AttachedPreprocessingInfoType relavantEndingMarkerCommentAndDirectiveList
          = getPreprocessingInfoList(endingMarkerVariableDeclaration,PreprocessingInfo::before);

  // Build the list of comments and directives (starting with the ones attached to the starting marker
     AttachedPreprocessingInfoType relavantCommentAndDirectiveList = relavantStartingMarkerCommentAndDirectiveList;

  // Then add the comments and directives attached to the ending marker.
     if (relavantEndingMarkerCommentAndDirectiveList.size() > 0)
        {
          AttachedPreprocessingInfoType::iterator i;
          for (i = relavantEndingMarkerCommentAndDirectiveList.begin(); i != relavantEndingMarkerCommentAndDirectiveList.end(); i++)
               relavantCommentAndDirectiveList.push_back(*i);
        }

  // *************************************************************
  // At this point we have the list of all comments and directives that could have (were) attached to the markers.
  // Now we will assign them to any statements in the input "statementList" in the code below.
  // *************************************************************

  // printf ("relavantCommentAndDirectiveList.size() = %" PRIuPTR " \n",relavantCommentAndDirectiveList.size());

  // Check if we have any comments or directives to worry about
     if (relavantCommentAndDirectiveList.size() > 0)
        {
       // Search for the statement before the endingMarkerVariableDeclaration
       // if it is the startingMarkerVariableDeclaration then return the 
       // relavantCommentAndDirectiveList else try to attach elements of the 
       // relavantCommentAndDirectiveList to the statements between the 
       // startingMarkerVariableDeclaration and the endingMarkerVariableDeclaration.

       // DQ (1/11/2006):
       // Look for any statement that would be output since they are the only
       // ones to which we would want to attach comments or CPP directives.
       // OLD COMMENT: If we have more than a starting and ending marker declaration then we 
       // OLD COMMENT: must have some transformation statements present.
       // bool transformationStatementPresent = (statementList.size() > 2);
          bool transformationStatementPresent = false;
          for (SgStatementPtrList::iterator i = statementList.begin(); i != statementList.end(); i++)
             {
            // printf ("(*i)->get_file_info()->isOutputInCodeGeneration() = %s \n",(*i)->get_file_info()->isOutputInCodeGeneration() ? "true" : "false");
               if ( (*i)->get_file_info()->isOutputInCodeGeneration() == true )
                    transformationStatementPresent = true;
             }

       // printf ("transformationStatementPresent = %s \n",transformationStatementPresent ? "true" : "false");

       // If the previous statment was a marker then there are no transformation statements 
       // between the markers, but there could have been a comment or CPP directive so check 
       // the markers for any attached comments between the markers.
          if (transformationStatementPresent == false)
             {
            // No transformation statements present (just the markers and comments or directives)
#if 0
               printf ("Case of at most (ONLY) comments placed between markers \n");
               ROSE_ABORT();
#endif
             }
            else
             {
            // Should be more than just the marker declarations
               ROSE_ASSERT ( statementList.size() > 2 );

            // Get the parent scope and see if it is a scope of declaration statments or just statements
               SgScopeStatement* parentScope = endingMarkerVariableDeclaration->get_scope();
               ROSE_ASSERT (parentScope != NULL);

            // Since the parent scope contains lists of different types we have to process 
            // two different cases (I would like to avoid conversion of list types since it is unsafe)!
            // Actually I don't like that we have to have two separate cases here!
               if (parentScope->containsOnlyDeclarations() == true)
                  {
                 // handle case of declaration is global scope

                    SgDeclarationStatementPtrList & declarationList = parentScope->getDeclarationList();
                    SgDeclarationStatementPtrList::iterator firstTransformationStatement;
                    SgDeclarationStatementPtrList::iterator lastTransformationStatement;
                    firstTransformationStatement = std::find(declarationList.begin(),declarationList.end(),startingMarkerVariableDeclaration);
                    lastTransformationStatement  = std::find(declarationList.begin(),declarationList.end(),endingMarkerVariableDeclaration);

                 // Make sure that we found a valid element
                    ROSE_ASSERT( firstTransformationStatement != declarationList.end() );
                    ROSE_ASSERT( lastTransformationStatement  != declarationList.end() );

                 // Test for if there is a previous element (error if not)
                    ROSE_ASSERT( lastTransformationStatement != declarationList.begin() );

                 // printf ("Before line--; line->unparseToString() = %s \n",(*line)->unparseToString().c_str());

                 // Get the next statement
                    firstTransformationStatement++;

                 // Backup one statement
                    lastTransformationStatement--;

                 // printf ("After line--; line->unparseToString() = %s \n",(*line)->unparseToString().c_str());

                    AttachedPreprocessingInfoType::iterator i;

                 // Accumulate references to the comments list so that they can be removed
                 // AttachedPreprocessingInfoType removalList;

                 // Iterate over the list of comments on the current node
                    for (i = relavantCommentAndDirectiveList.begin(); i != relavantCommentAndDirectiveList.end(); i++)
                       {
                      // If the comment was positioned before the marker then 
                      // copy it to the previous line's comment list as an 
                      // afterward comment.
                         if ( (*i)->getRelativePosition() == PreprocessingInfo::before )
                            {
                           // (*lastTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::after);
                           // (*lastTransformationStatement)->addToAttachedPreprocessingInfo(*i,SgLocatedNode::appendPreprocessingInfoToList);
                              (*lastTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::after);
                           // removalList.push_back(*i);
                            }
                           else
                            {
                              ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::after );
                           // (*firstTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::before);
                           // (*firstTransformationStatement)->addToAttachedPreprocessingInfo(*i,SgLocatedNode::prependPreprocessingInfoToList);
                              (*firstTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::before);
                           // removalList.push_back(*i);
                            }
                       }

#if 0
                 // Now remove the entries from the list on the current statements (marker declaration) 
                 // (not critical since the list will be removed once we go out of scope).
                    for (i = removalList.begin(); i != removalList.end(); i++)
                       {
                      // DQ (9/25/2007): Moved from std::list to std::vector.
                      // relavantCommentAndDirectiveList.remove(*i);
                         relavantCommentAndDirectiveList.erase(find(relavantCommentAndDirectiveList.begin(),relavantCommentAndDirectiveList.end(),i));
                       }
#else
                    relavantCommentAndDirectiveList.clear();
#endif
                 // printf ("After removing elements copied to previous declaration: relavantCommentAndDirectiveList.size() = %" PRIuPTR " \n",relavantCommentAndDirectiveList.size());

                 // printf ("Sorry not implemented, comments/directives are in global scope in processCommentsAndDirectives \n");
                 // ROSE_ASSERT (false);
                  }
                 else
                  {
                 // handle case of declaration NOT in global scope
                    SgStatementPtrList & statementList = parentScope->getStatementList();

                    SgStatementPtrList::iterator firstTransformationStatement;
                    SgStatementPtrList::iterator lastTransformationStatement;
                    firstTransformationStatement = std::find(statementList.begin(),statementList.end(),startingMarkerVariableDeclaration);
                    lastTransformationStatement  = std::find(statementList.begin(),statementList.end(),endingMarkerVariableDeclaration);

                 // Make sure that we found a valid element
                    ROSE_ASSERT( firstTransformationStatement != statementList.end() );
                    ROSE_ASSERT( lastTransformationStatement  != statementList.end() );

                 // Test for if there is a previous element (error if not)
                    ROSE_ASSERT( lastTransformationStatement != statementList.begin() );

                 // printf ("Before line--; line->unparseToString() = %s \n",(*line)->unparseToString().c_str());

                 // Get the next statement
                    firstTransformationStatement++;

                 // Backup one statement
                    lastTransformationStatement--;

                 // printf ("After line--; line->unparseToString() = %s \n",(*line)->unparseToString().c_str());

                    AttachedPreprocessingInfoType::iterator i;

                 // Accumulate references to the comments list so that they can be removed
                 // AttachedPreprocessingInfoType removalList;

                 // Iterate over the list of comments on the current node
                    for (i = relavantCommentAndDirectiveList.begin(); i != relavantCommentAndDirectiveList.end(); i++)
                       {
                      // If the comment was positioned before the marker then 
                      // copy it to the previous line's comment list as an 
                      // afterward comment.
                         if ( (*i)->getRelativePosition() == PreprocessingInfo::before )
                            {
                           // (*lastTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::after);
                           // (*lastTransformationStatement)->addToAttachedPreprocessingInfo(*i,SgLocatedNode::appendPreprocessingInfoToList);
                              (*lastTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::after);
                           // removalList.push_back(*i);
                            }
                           else
                            {
                              ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::after );
                           // (*firstTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::before);
                           // (*firstTransformationStatement)->addToAttachedPreprocessingInfo(*i,SgLocatedNode::prependPreprocessingInfoToList);
                              (*firstTransformationStatement)->addToAttachedPreprocessingInfo(*i,PreprocessingInfo::before);
                           // removalList.push_back(*i);
                            }
                       }
#if 0
                 // Now remove the entries from the list on the current statements (marker declaration) 
                 // (not critical since the list will be removed once we go out of scope).
                    for (i = removalList.begin(); i != removalList.end(); i++)
                       {
                         relavantCommentAndDirectiveList.remove(*i);
                       }
#else
                    relavantCommentAndDirectiveList.clear();
#endif
                 // printf ("After removing elements copied to previous statement: relavantCommentAndDirectiveList.size() = %" PRIuPTR " \n",relavantCommentAndDirectiveList.size());
                  }

            // Since there are statements all the relavant comments and directives 
            // can be attached to the statements directly.
               ROSE_ASSERT(relavantCommentAndDirectiveList.size() == 0);
             }
        }
       else
        {
       // printf ("No comments or CPP directives attached to these markers \n");
        }

//   ROSE_ASSERT(startingMarkerVariableDeclaration != NULL);
//   ROSE_ASSERT(endingMarkerVariableDeclaration   != NULL);
//   ROSE_ASSERT (getPreprocessingInfoList(startingMarkerVariableDeclaration, PreprocessingInfo::after ).size() == 0);
//   ROSE_ASSERT (getPreprocessingInfoList(endingMarkerVariableDeclaration,   PreprocessingInfo::before).size() == 0);

#if 0
     printf ("At BASE of AbstractInterfaceNodeCollection<T>::processCommentsAndDirectives() \n");
     ROSE_ABORT();
#endif

     return relavantCommentAndDirectiveList;
   }

// endif for ABSTRACT_INTERFACE_NODE_COLLECTION_TEMPLATES_C
#endif











