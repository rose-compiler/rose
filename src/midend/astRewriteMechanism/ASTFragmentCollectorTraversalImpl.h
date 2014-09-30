#ifndef AST_REWRITE_FRAGMENT_COLLECTOR_TRAVERSAL_C
#define AST_REWRITE_FRAGMENT_COLLECTOR_TRAVERSAL_C

// #include "rose.h"
#include "rewrite.h"

// ####################################################################
// Functions for AST_FragmentIdentificationInheritedAttributeType class
// ####################################################################

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType::
~AST_FragmentIdentificationInheritedAttributeType ()
   {
  // recordTreeFragments = false;
     treeFragmentFromCorrectFile = false;
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType::
AST_FragmentIdentificationInheritedAttributeType ()
   {
  // recordTreeFragments = false;
     treeFragmentFromCorrectFile = false;
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType::
AST_FragmentIdentificationInheritedAttributeType ( const AST_FragmentIdentificationInheritedAttributeType & X )
   {
     operator=(X);
   }

template <class ASTNodeCollection>
typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType &
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType::
operator=( const AST_FragmentIdentificationInheritedAttributeType & X )
   {
  // recordTreeFragments         = X.recordTreeFragments;
     treeFragmentFromCorrectFile = X.treeFragmentFromCorrectFile;
     currentFileName             = X.currentFileName;

     return *this;
   }

// ####################################################################
// Functions for AST_FragmentIdentificationSynthesizedAttributeType class
// ####################################################################

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
~AST_FragmentIdentificationSynthesizedAttributeType()
   {
     isAMarker = false;
     markerString = "";
     treeFragment = NULL;
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
AST_FragmentIdentificationSynthesizedAttributeType()
   {
     isAMarker = false;
     markerString = "";
     treeFragment = NULL;

  // Make sure the vector of SgStatementPtrList is initialized to be the correct size
     initializeNodeListToCorrectSize();
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
AST_FragmentIdentificationSynthesizedAttributeType(
const AST_FragmentIdentificationSynthesizedAttributeType & X )
   {
     operator=(X);
   }

template <class ASTNodeCollection>
typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType &
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
operator=( const AST_FragmentIdentificationSynthesizedAttributeType & X )
   {
     isAMarker    = X.isAMarker;
     markerString = X.markerString;
     treeFragment = X.treeFragment;

     treeFragementListArray = X.treeFragementListArray;

     ROSE_ASSERT ( unsigned(ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG) == 
                   treeFragementListArray.size() );

     return *this;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
initializeNodeListToCorrectSize ()
   {
  // Make sure the vector of SgStatementPtrList is initialized to be the correct size
     SgStatementPtrList tempList;
     for (int i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          treeFragementListArray.push_back(tempList);
        }

  // printf ("treeFragementListArray.size() = %" PRIuPTR " \n",treeFragementListArray.size());
     ROSE_ASSERT ( unsigned(ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG) == 
                   treeFragementListArray.size() );
   }

#if 1
template <class ASTNodeCollection>
typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType &
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
operator+= ( const AST_FragmentIdentificationSynthesizedAttributeType & X )
   {
  // accumulate the child attributes to construct the parent attribute
  // list<SgStatement*> treeFragementListArray[LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG];
  // for (int i=unknownIntermediatePositionInScope; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)

  // Ignore copying the unknownIntermediatePositionInScope list of statements that might have been accumulated.
  // These are often the application's declarations that have been introduced into the intermediate 
  // file so that the source code strings representing the transformations will compile properly 
  // (so that all possible variables that could be used in a source code string representing a 
  // transformation will be defined).
     for (int i = ASTNodeCollection::GlobalScopePreamble; 
          i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
       // treeFragementListArray[i] += X.treeFragementListArray[i];
          // ROSE_ASSERT ( treeFragementListArray[i].size() == 0 || X.treeFragementListArray[i].size() >= 0 ); -- JJW 10-18-2007 always true
          if ( X.treeFragementListArray[i].size() > 0 )
             {
#if 0
            // If not empty then what is there
               printf ("treeFragementListArray[d].size() = %" PRIuPTR " \n",i,treeFragementListArray[i].size());
               if (treeFragementListArray[i].size() != 0)
                  {
                    list<SgStatement*>::iterator j;
                    for (j = treeFragementListArray[i].begin(); j != treeFragementListArray[i].end(); j++)
                       {
                         printf ("Mysterious code fragment = %s \n",(*j)->unparseToString().c_str());
                       }
                  }
#endif
            // The existing list should be empty
               ROSE_ASSERT(treeFragementListArray[i].size() == 0);
            // Since the existing list is empty we can just use the assignment operator to fill it
               treeFragementListArray[i] = X.treeFragementListArray[i];
//             printf ("In operator+=(): treeFragementListArray[%d].size() = %" PRIuPTR " \n",i,treeFragementListArray[i].size());
             }
        }

     return *this;
   }
#endif

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
display ( std::string s ) const
   {
     printf ("Inside of AST_FragmentIdentificationSynthesizedAttributeType::display(%s) \n",s.c_str());
     printf ("     isAMarker = %s \n",(isAMarker == true) ? "TRUE" : "FALSE");
     printf ("     Marker string = %s \n",markerString.c_str());
     printf ("     treeFragment = %p \n",treeFragment);

     bool treeFragmentCodeAttached    = false;
     bool listOfTreeFragmentsAttached = false;
     std::string treeFragmentCodeString;
     if (treeFragment != NULL)
        {
#if 1
          treeFragmentCodeString = treeFragment->unparseToString();
#endif
          treeFragmentCodeAttached = true;
        }
     
     printf ("     treeFragment code = %s \n",treeFragmentCodeString.c_str());

     for (int i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
       // Get the name of the position (better than using just an index value)
          ROSE_ASSERT (treeFragementListArray[i].size() >= 0);
          std::string positionName = ASTNodeCollection::markerStrings[i][0] + "/" + 
                                ASTNodeCollection::markerStrings[i][1];
          ROSE_ASSERT (positionName.c_str() != NULL);
          printf ("     treeFragementListArray[%2d:%80s].size() = %" PRIuPTR " \n",
               i,positionName.c_str(),treeFragementListArray[i].size());

#if 1
          std::vector<SgStatement*>::const_iterator j;
          for (j = treeFragementListArray[i].begin(); j != treeFragementListArray[i].end(); j++)
             {
               listOfTreeFragmentsAttached = true;
               ROSE_ASSERT ( *j != NULL);

            // Make sure this is a SgStatement object
               ROSE_ASSERT ( dynamic_cast<SgStatement*>(*j) != NULL);

            // generate the string and output it for debugging
#if 1
               std::string sourceCode = (*j)->unparseToString();
#else
               std::string sourceCode = "Skipped call to unparseToString()";
#endif
               printf ("          ((*j)->sage_class_name() = %s) sourceCode = %s \n",
                    (*j)->sage_class_name(),sourceCode.c_str());
             }
#endif
        }

  // They can't both be true
//   ROSE_ASSERT ( !(listOfTreeFragmentsAttached && treeFragmentCodeAttached) );

#if 0
     printf ("Leaving MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::display() \n");
#endif
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
setMarker ( std::string s)
   {
     isAMarker    = true;
     markerString = s;
   }

template <class ASTNodeCollection>
std::string
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
getMarker()
   {
     ROSE_ASSERT (isAMarker == true);
     return markerString;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
setTreeFragment ( SgStatement* astNode)
   {
//   isAMarker    = false;
//   markerString = "";
//   ROSE_ASSERT (isAMarker == false);
//   ROSE_ASSERT (markerString == "");
     treeFragment = astNode;
   }

template <class ASTNodeCollection>
SgStatement*
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
getTreeFragment()
   {
//   ROSE_ASSERT (isAMarker == false);
//   ROSE_ASSERT (markerString == "");
     return treeFragment;
   }

template <class ASTNodeCollection>
bool
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
isStartingMarker()
   {
     return isStartOrEndMarker( markerString, 0 );
   }

template <class ASTNodeCollection>
bool
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
isEndingMarker()
   {
     return isStartOrEndMarker( markerString, 1 );
   }

template <class ASTNodeCollection>
typename ASTNodeCollection::IntermediateFileStringPositionEnum
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
location()
   {
  // If this SynthesizedAttribute is a marker then this function returns the
  // index into the list of tree fragments associated with that marker.

     typename ASTNodeCollection::IntermediateFileStringPositionEnum returnValue = 
          ASTNodeCollection::unknownIntermediatePositionInScope;

#if 0
     printf ("In AST_FragmentIdentificationSynthesizedAttributeType::location(): isAMarker = %s \n",(isAMarker == true) ? "true" : "false");
#endif

     if (isAMarker == true)
        {
       // printf ("markerString = %s \n",markerString.c_str());

       // Compare against static string values in ASTFragmentRequirementStrings::targetStringArray
       // IntermediateFileStringPositionEnum i;
          int i;
          for (i = ASTNodeCollection::GlobalScopePreamble; // Used to start loop with GlobalScopeTopOfScope;
               i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG;
               i++)
             {
#if 0
               printf ("i = %d ASTFragmentRequirementStrings::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG = %d \n",
                    i,ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG);
               printf ("ASTNodeCollection::markerStrings[%d][0] = %s \n",i,ASTNodeCollection::markerStrings[i][0].c_str());
               printf ("ASTNodeCollection::markerStrings[%d][1] = %s \n",i,ASTNodeCollection::markerStrings[i][1].c_str());
#endif

               if ( (markerString == ASTNodeCollection::markerStrings[i][0]) ||
                    (markerString == ASTNodeCollection::markerStrings[i][1]) )
                  {
//                  printf ("Set the returnValue to non default value \n");
                    returnValue = typename ASTNodeCollection::IntermediateFileStringPositionEnum(i);
                  }
             }
        }
       else
        {
       // printf ("isAMarker == false \n");
        }

#if 0
     printf ("Inside of AST_FragmentIdentificationSynthesizedAttributeType::location() = %d (%s) \n",
          returnValue,ASTNodeCollection::positionName(returnValue).c_str());
#endif

     return returnValue;
   }

template <class ASTNodeCollection>
bool
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
isStartOrEndMarker( std::string markerString, int startOrEnd )
   {
  // Supporting function for isStartingMarker() and isEndingMarker() member functions
     bool returnValue = false;

     ROSE_ASSERT (startOrEnd == 0 || startOrEnd == 1);

#if 0
  // faster implementation but without the error checking
     returnValue = isAMarker;
#else
  // error checking (a more efficent implementation could just return the isAMarker data member)
  // Compare against static string values in ASTFragmentRequirementStrings::targetStringArray
     int i;
     for (i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          if (markerString == ASTNodeCollection::markerStrings[i][startOrEnd])
             {
            // printf ("Found matching marker string = %s \n",markerString.c_str());
               returnValue = true;
             }
        }
#endif

#if 0
     printf ("Inside of isStartOrEndMarker(%s) returnValue = %s \n",markerString.c_str(),(returnValue == true) ? "true" : "false");
#endif

     return returnValue;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
consistancyCheck( const std::string & s ) const
   {
  // printf ("AST_FragmentIdentificationSynthesizedAttributeType::consistancyCheck(%s) \n",s.c_str());

  // Note that markers now have a valid treeFragement pointer

     bool  foundString = false;
  // Compare against static string values in ASTFragmentRequirementStrings::targetStringArray
     for (int i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          if (markerString == ASTNodeCollection::markerStrings[i][0])
             {
//             printf ("Found matching START marker string = %s \n",markerString.c_str());
               foundString = true;
             }
        }

  // error checking
     bool foundError = false;
     if (foundString == true)
        {
          if (isAMarker == false)
             {
               printf ("Error: isAMarker == false but found a matching START string \n");
               foundError = true;
             }
        }
       else
        {
       // Make sure it is not an alternative value
          bool foundAlternativeMarkerString = false;
          for (int i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
             {
               if (markerString == ASTNodeCollection::markerStrings[i][1])
                  {
//                  printf ("Found matching END marker string = %s \n",markerString.c_str());
                    foundAlternativeMarkerString = true;
                  }
             }

          if (foundAlternativeMarkerString == true)
             {
               if (isAMarker == false)
                  {
                    printf ("Error: isAMarker == false but found a matching END string \n");
                    foundError = true;
                  }
             }
            else
             {
               if (isAMarker == true)
                  {
                    printf ("Error: isAMarker == true but can't find matching string \n");
                    foundError = true;
                  }
             }
        }

     if (foundError == true)
        {
          printf ("Error: isAMarker (%s) set incorrectly: markerString = %s \n",
               isAMarker ? "true" : "false",markerString.c_str());
        }

     ROSE_ASSERT (foundError == false);
   }


// #################################################################
//     Functions for AST_FragmentIdentificationTraversal class
// #################################################################

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationTraversal::
~AST_FragmentIdentificationTraversal ()
   {
     recordStatements = false;
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationTraversal::
AST_FragmentIdentificationTraversal ()
   : recordStatements(false)
   {
  // Nothing to do here
   }

template <class ASTNodeCollection>
typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationTraversal::
evaluateInheritedAttribute (
   SgNode* astNode,
   typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType inheritedValue )
   {
  // This function interprets the context within the intermediate file from which 
  // we extract AST fragments and sets the value of the returnAttribute.currentFileName
  // and returnAttribute.recordTreeFragments member data.

     ROSE_ASSERT (astNode != NULL);

#if 0
     printf ("$$$$$ In AST_FragmentIdentificationTraversal::evaluateInheritedAttribute(): astNode = %p astNode->sage_class_name() = %s inheritedValue.treeFragmentFromCorrectFile = %s \n",
             astNode,astNode->sage_class_name(),
             (inheritedValue.treeFragmentFromCorrectFile) ? "true" : "false");
#endif

     AST_FragmentIdentificationInheritedAttributeType returnAttribute;

  // Should we use the copy constructor (or special constructor) when we first build returnAttribute?
     returnAttribute.currentFileName = inheritedValue.currentFileName;

  // Don't accept a project node since we only want to process SgFile 
  // nodes as intermediate files for AST fragment generation (not sure why!)
     ROSE_ASSERT (astNode->variantT() != V_SgProject);

  // Make sure that we start off the traversal correctly (with the correct file name)
  // This way the use need not set the file name in the inherited attribute before 
  // starting the traversal.

  // DQ (9/2/2008): Fixup to support SgSourceFile and SgBinaryFile IR nodes.
  // if (astNode->variantT() == V_SgFile)
     if ( isSgFile(astNode) != NULL )
        {
       // Record the filename associated with the SgFile (the current file)
          SgFile* file = dynamic_cast<SgFile*>(astNode);
          ROSE_ASSERT(file != NULL);

       // returnAttribute.currentFileName             = ROSE::getFileName(file);
          returnAttribute.currentFileName             = file->getFileName();
          returnAttribute.treeFragmentFromCorrectFile = true;
        }
       else
        {
          SgStatement* statementNode = dynamic_cast<SgStatement*>(astNode);

       // Only consider recording statements that appear from the 
       // intermediate file (exclude others included using #include)

       // DQ (9/16/2005): This is where we have to handle IR nodes that we don't have in the primary AST.
       // If the IR node is not present in the primary AST it could be a declaration of a variable not 
       // previously seen which will be referenced later when we use the rewrite mechanism again!
       // So we have to include these IR nodes if they are not in the primary AST.  Not clear how to 
       // figure that out, except to test the symbol tables for declarations of the same name.
          if (statementNode != NULL)
             {
            // DQ (5/26/2005): Modified to use minimalist Sage III interface (trying to remove several ROSE::xxx() member functions)
            // std::string nodeFileName = ROSE::getFileName(statementNode);
               std::string nodeFileName = statementNode->get_file_info()->get_filename();
#if 0
               printf ("nodeFileName = %s returnAttribute.currentFileName = %s \n",
                    nodeFileName.c_str(),returnAttribute.currentFileName.c_str());
#endif

            // DQ (1/11/2006): Even statements from other files must be included 
            // in the AST (just MARKED as coming from different files).  Declarations 
            // from files included via include directives must show up in the AST to 
            // be used later.
            // OLD COMMENT: Check if the file names match (we only record fragments from the
            // OLD COMMENT: same file to provide consistant semantics in transformations).
            // if (nodeFileName == returnAttribute.currentFileName)
            //      returnAttribute.treeFragmentFromCorrectFile = true;
               returnAttribute.treeFragmentFromCorrectFile = true;
             }
        }

#if 0
  // returnAttribute.display("Called from inside AST_FragmentIdentificationTraversal::evaluateInheritedAttribute()");
     printf ("$$$$$ Leaving AST_FIT::evalInherAttr() returnAttribute.treeFragmentFromCorrectFile = %s \n",
             (returnAttribute.treeFragmentFromCorrectFile) ? "true" : "false");
#endif

     return returnAttribute;
   }

template <class ASTNodeCollection>
typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationTraversal::
evaluateSynthesizedAttribute (
   SgNode* astNode,
   typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType inheritedValue,
   typename MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationTraversal::SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
#if 0
     printf ("##### In AST_FIT::evalSynthAttr(#attr=%3d): astNode = %p recordStatements = %5s treeFragmentFromCorrectFile = %5s sage_class_name = %s \n",
          synthesizedAttributeList.size(),
          astNode,
          (recordStatements) ? "true" : "false",
          (inheritedValue.treeFragmentFromCorrectFile) ? "true" : "false",
          astNode->sage_class_name());
#endif
#if 0
     if (dynamic_cast<SgStatement*>(astNode) != NULL)
          printf ("----- In AST_FIT::evalSynthAttr(): astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

  // ROSE_ASSERT (synthesizedAttributeList.size() < 100);

     AST_FragmentIdentificationSynthesizedAttributeType returnAttribute;

  // Mark the AST fragments as being transformations so that they will be unparsed properly with the
  // other statements even though they have a different filename associated with them. This approach
  // is better than changing the filenames.
     ROSE_ASSERT (astNode != NULL);
     if (astNode->get_file_info() != NULL)
        {
          ROSE_ASSERT (astNode->get_file_info() != NULL);
          astNode->get_file_info()->set_isPartOfTransformation(true);
        }

#if 0
     if (isSgStatement(astNode) != NULL)
        {
          SgStatement* statement = isSgStatement(astNode);
          printf ("ROSE::getLineNumber(statement) = %d \n",ROSE::getLineNumber(statement));
          printf ("statement->sage_class_name() = %s \n",statement->sage_class_name());
          printf ("statement->unparseToString() = %s \n",statement->unparseToString().c_str());

       // We can't assert this since a SgGlobal is defined to have a linenumber == 0
       // ROSE_ASSERT (ROSE::getLineNumber(statement) > 0);
        }
#endif

  // Master switch statement of semantic actions attached to each node
     switch (astNode->variantT())
        {
       // Handle both these cases together (since they are both collections of statements)
          case V_SgGlobal:
          case V_SgBasicBlock:

       // DQ (11/22/2003): Need to detect new code (transformations) specified in class definitions
          case V_SgClassDefinition:
             {
               SgStatement* statement = isSgStatement(astNode);
               ROSE_ASSERT (statement != NULL);
#if 0
               printf ("Found a statement sage_class_name() = %s \n",astNode->sage_class_name());
               printf ("inheritedValue.treeFragmentFromCorrectFile = %s \n",
                    (inheritedValue.treeFragmentFromCorrectFile == true) ? "true" : "false");
#endif
               if (inheritedValue.treeFragmentFromCorrectFile == true)
                  {
                 // Go through the list of attributes and separate out (bin) the ones that are
                 // between markers into groups (bins) that are given by the strings associated with
                 // each marker.

                 // Save the index of each starting marker as we iterate through the
                 // synthesizedAttributeList
                    int startingMarkerAttributeIndex = 0;

//                  printf ("In AST_FragmentIdentificationTraversal::evaluateSynthesizedAttribute(): synthesizedAttributeList.size() = %d \n",
//                       synthesizedAttributeList.size());

                    bool startingMarkerFound = false; // default value (assume starting marker has not been seen)
                    bool endingMarkerFound   = true;  // default value (assume ending marker has been seen)
                    unsigned int i;

                 // This value is set when the starting marker is found 
                 // (starting mark is found before ending marker by definition)
                    IntermediateFileStringPositionEnum locationCounter = 
                         ASTNodeCollection::unknownIntermediatePositionInScope;

                    for (i = 0; i < synthesizedAttributeList.size(); i++)
                       {
#if 0
                         printf ("\n\n");
                         printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
                         printf ("Looping through the synthesizedAttributeList (element #%d) (size=%d) \n",
                              i,synthesizedAttributeList.size());
                         printf ("     Looking for %s/%s \n",
                              ASTNodeCollection::markerStrings[(int)locationCounter][0].c_str(),
                              ASTNodeCollection::markerStrings[(int)locationCounter][1].c_str());
                         printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
#endif

                      // error checking
                         synthesizedAttributeList[i].consistancyCheck("Checking the child list");

                      // display each attribute
//                       synthesizedAttributeList[i].display("Called in evaluateSynthesizedAttribute");

                         if (synthesizedAttributeList[i].isStartingMarker() == true)
                            {
#if 0
                              printf ("Found the starting marker at statement #%d (count starting at zero) \n",i);
#endif
                              startingMarkerFound          = true;
                              startingMarkerAttributeIndex = i;

                           // error checking (must have found starting marker before ending marker)
                              ROSE_ASSERT (endingMarkerFound == true);
                              endingMarkerFound = false;

                           // Turn ON statement recording
//                            printf ("RECORDING ON: all statements between markers pushed onto list \n");
                              recordStatements = true;

                              locationCounter = synthesizedAttributeList[startingMarkerAttributeIndex].location();
                              ROSE_ASSERT (locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);

                           // But add the starting marker variable to the list of tree fragements
                           // so that attachments to the marker nodes can be preserved and 
                           // placed into the final AST.
                              ROSE_ASSERT (synthesizedAttributeList[i].treeFragment != NULL);
                              ROSE_ASSERT (locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);

                           // insert statement into list of statements
                              returnAttribute.treeFragementListArray[locationCounter].push_back( synthesizedAttributeList[i].treeFragment );
                            }
                           else
                            {
                              if (synthesizedAttributeList[i].isEndingMarker() == true)
                                 {
                                // Nothing to do here (since the locationCounter is incremented when
                                // we find the starting marker)
#if 0
                                   printf ("Found the ending marker at statement #%d (starting marker was at statement #%d) (count starting at zero) \n",i,startingMarkerAttributeIndex);
#endif
                                // error checking (must have found starting marker before ending marker)
                                   ROSE_ASSERT (startingMarkerFound == true);
                                   startingMarkerFound = false;
                                   endingMarkerFound   = true;

                                // But add the ending marker variable to the list of tree fragements
                                // so that attachments to the marker nodes can be preserved and 
                                // placed into the final AST.
                                   ROSE_ASSERT (synthesizedAttributeList[i].treeFragment != NULL);
                                   ROSE_ASSERT (locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);

                                // insert statement into list of statements
                                   returnAttribute.treeFragementListArray[locationCounter].push_back( synthesizedAttributeList[i].treeFragment );

                                // Turn OFF statement recording
//                                 printf ("RECORDING OFF: all statements between markers pushed onto list \n");
                                   recordStatements = false;
                                 }
                                else
                                 {
                                // Not true for functions appearing in the global scope!
                                // ROSE_ASSERT (startingMarkerFound == true);
                                // synthesizedAttributeList[i].display("Processing AST fragement between markers");

                                // printf ("Neither a starting nor ending marker! startingMarkerAttributeIndex = %d \n",startingMarkerAttributeIndex);

#if 0
                                   printf ("synthesizedAttributeList[%d].treeFragment = %s POINTER \n",
                                        i,(synthesizedAttributeList[i].treeFragment == NULL) ? "NULL" : "VALID");
                                   printf ("recordStatements = %s \n",recordStatements ? "true" : "false");
#endif
                                   if ( (recordStatements == true) && 
                                        (synthesizedAttributeList[i].treeFragment != NULL) )
                                      {
                                        ROSE_ASSERT (recordStatements == true);
                                        ROSE_ASSERT (synthesizedAttributeList[i].treeFragment != NULL);
                                        ROSE_ASSERT (locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);
                                        ROSE_ASSERT (unsigned(locationCounter) < returnAttribute.treeFragementListArray.size());

#if 0
                                        printf ("RECORDING: synthesizedAttributeList[%d].treeFragment \n",i);
                                        printf ("     locationCounter = %d (%s) \n",locationCounter,
                                             ASTNodeCollection::getIntermediateFilePositionName(locationCounter).c_str());
                                        printf ("returnAttribute.treeFragementListArray.size() = %d \n",
                                             returnAttribute.treeFragementListArray.size());
                                        printf ("returnAttribute.treeFragementListArray[locationCounter].size() = %d \n",
                                             returnAttribute.treeFragementListArray[locationCounter].size());
#endif

                                     // insert statement into list of statements
                                        returnAttribute.treeFragementListArray[locationCounter].push_back( synthesizedAttributeList[i].treeFragment );
                                        ROSE_ASSERT (returnAttribute.treeFragementListArray[locationCounter].size() > 0);
#if 0
                                        returnAttribute.display("Processing AST fragement between markers");
#endif
                                      }
                                     else
                                      {
                                     // Could still be a synthesized attribute from a function
                                     // containing AST fragments so check for that (call the operator+=).

#if 0
                                        synthesizedAttributeList[i].display("defalue case: synthesizedAttributeList[i]");
#endif
                                        SgStatement* statement = isSgStatement(astNode);
                                        ROSE_ASSERT (statement != NULL);

                                     // printf ("Calling returnAttribute.setTreeFragment(SgBasicBlock) \n");
                                        returnAttribute.setTreeFragment(statement);

#if 0
                                        returnAttribute.display("AFTER call to operator+=()");
#endif
                                      }
                                 }
                            }
                       }
                  }
                 else
                  {
                 // We have to include any SgBasicBlock that could appear between markers
                 // printf ("SgBasicBlock not located in current file \n");
                  }

#if 0
               printf ("Found the SgBasicBlock node ... exiting at base of case ... \n");
               ROSE_ABORT();
#endif
               break;
             }

          case V_SgVariableDeclaration:
             {
#if 0
               printf ("\n\n");
               printf ("************************************** \n");
               printf ("Found a variable declaration statement \n");
               printf ("************************************** \n");
#endif
               SgVariableDeclaration* varDeclaration = isSgVariableDeclaration (astNode);
               ROSE_ASSERT(varDeclaration != NULL);
               SgInitializedNamePtrList & nameList = varDeclaration->get_variables();
               if (nameList.size() == 1)
                  {
                 // The markers we have defined are a single integer variable
                    SgInitializedName* initializedName = *(nameList.begin());
                    SgName name = initializedName->get_name();
                    std::string variableName = name.str();

                 // This could be done in the inherited attribute!!!
                    bool foundMarker = ASTNodeCollection::isAMarkerString(variableName);
#if 0
                    printf ("foundMarker = %s: variableName = %s \n",
                         foundMarker ? "true" : "false",variableName.c_str());
#endif
                    if ( foundMarker == true )
                       {
//                       printf ("foundMarker = true: variableName = %s \n",variableName.c_str());
                      // Setting the marker here simplifies the test for the marker in the
                      // SgBasicBlock and SgGlobal cases above (though it does make for more 
                      // convoluted code so this implementation might be changed to simplify 
                      // the logic later)
                      // printf ("Setting marker string in synthesized attribute \n");
                         returnAttribute.setMarker(variableName);
                       }
#if 0
                      else
                       {
                      // printf ("Not a marker variable \n");
                         if ( inheritedValue.treeFragmentFromCorrectFile == true )
                            {
                           // Attach the current statement to the synthesized attribute so that 
                           // they can be collected in the SgGlobal and SgBasicBlock cases (above).
                              returnAttribute.setTreeFragment(varDeclaration);
                            }
                       }
#endif
                 // error checking
                    returnAttribute.consistancyCheck("Checking the returnAttribute: SgVariableDeclaration case");
                  }

            // NOTE: FALL THROUGH OF CASE!!!
            // break;
             }

          case V_SgFunctionDefinition:
          case V_SgFunctionDeclaration:
          case V_SgExprStatement:
          case V_SgBreakStmt:
          case V_SgContinueStmt:
          case V_SgClassDeclaration:
          case V_SgForStatement:
          case V_SgScopeStatement:
          case V_SgDeclarationStatement:
          case V_SgIfStmt:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgSwitchStatement:
          case V_SgCatchStatementSeq:
          case V_SgCatchOptionStmt:
          case V_SgMemberFunctionDeclaration:
          case V_SgCtorInitializerList:
          case V_SgFunctionParameterList:
          case V_SgVariableDefinition:
          case V_SgEnumDeclaration:
          case V_SgAsmStmt:
          case V_SgTypedefDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateInstantiationDefn:
          case V_SgStatement:
          case V_SgForInitStatement:
          case V_SgReturnStmt:
          case V_SgPragmaDeclaration:

       // DQ (11/14/2003): Added previously forgotten cases
          case V_SgCaseOptionStmt:
          case V_SgDefaultOptionStmt:

       // DQ (1/10/2006): Added case of SgNamespaceDefinitionStatement and SgNullStatement
          case V_SgNamespaceDefinitionStatement:
          case V_SgNullStatement:

       // DQ (4/30/2006): Added support for Using Directives (bug for 3.4.3 compiler, likely due to different header files internally)
          case V_SgUsingDirectiveStatement:

       // DQ (1/11/2006): Added case of SgNamespaceDeclarationStatement,
       // SgUsingDeclarationStatement and SgTemplateInstantiationFunctionDecl
          case V_SgNamespaceDeclarationStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          case V_SgTemplateInstantiationDirectiveStatement:

             {
            // printf ("Found a statement sage_class_name() = %s \n",astNode->sage_class_name());
               SgStatement* statement = isSgStatement(astNode);
               ROSE_ASSERT (statement != NULL);
            // Can't make this conditional dependent upon if recording is set since the child 
            // synthesised attributes a build before processin the basic block or global scope 
            // attributes.
               if ( inheritedValue.treeFragmentFromCorrectFile == true )
                  {
                 // Attach the current statement to the synthesized attribute so that 
                 // they can be collected in the SgGlobal and SgBasicBlock cases (above).
                 // printf ("calling setTreeFragment for statement \n");
                    returnAttribute.setTreeFragment(statement);
                  }
                 else
                  {
                 // DQ (9/16/2005): Here is where we have to handle the case of IR nodes that are not 
                 // in the secondary file (likely included from a #include directive inserted as a 
                 // transfromation.

                 // Check if the IR node is in the primary AST. To do this we need a map such as what 
                 // Milind generated to be stored at the SgFile level and which could be updated. An
                 // alternative would be to trace back through the secondary AST to find the scopes
                 // and check for the existance of each scope name in the primary AST.
                  }

            // error checking
               returnAttribute.consistancyCheck("Checking the returnAttribute: general SgStatement case");
               break;
             }
#if 0
          case V_SgNullStatement:
             {
            // ignore this case
               printf ("Ignoring case of SgNullStatement in evaluateSynthesizedAttribute() in ASTFragmentCollectorTraversal.C \n");
               break;
             }
#endif
          default:
             {
            // Make sure this is not a SgStatement (they should all be handled 
            // above (so this is error checking)).
               if (isSgStatement(astNode) != NULL)
                    printf ("Error Default reached in switch: astNode->sage_class_name = %s \n",astNode->sage_class_name());
               ROSE_ASSERT (isSgStatement(astNode) == NULL);
             }
        }

  // error checking
     returnAttribute.consistancyCheck("Checking the returnAttribute: default case");

  // Accumulate data from child synthesized attributes into the returnAttribute
     for (unsigned int i = 0; i < synthesizedAttributeList.size(); i++)
        {
#if 0
          printf ("\n\n");
          printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
          printf ("Looping through the synthesizedAttributeList (element #%d) \n",i);
          printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");

          printf ("(%s)->unparseToString() = %s \n",astNode->sage_class_name(),astNode->unparseToString().c_str());
       // printf ("astNode->sage_class_name() = %s \n",astNode->sage_class_name());
       // printf ("astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

       // Can't have both non-empty lists and valid treeFragment pointer
          if (synthesizedAttributeList[i].treeFragment != NULL)
             {
            // printf ("Reset the synthesizedAttributeList[%d].treeFragment pointer \n",i);
               synthesizedAttributeList[i].treeFragment = NULL;
             }

       // error checking
          synthesizedAttributeList[i].consistancyCheck("Checking the child list");
#if 0
       // display each attribute
          synthesizedAttributeList[i].display("Called in evaluateSynthesizedAttribute");
#endif
       // Accumulate all the SgStatements in the different lists
          returnAttribute += synthesizedAttributeList[i];
        }

#if 0
     printf ("At BASE of AST_FragmentIdentificationTraversal::evaluateSynthesizedAttribute() \n");
  // ROSE_ABORT();
#endif

  // error checking
     returnAttribute.consistancyCheck("Checking the returnAttribute");
     
     return returnAttribute;
   }

// endif for AST_REWRITE_FRAGMENT_COLLECTOR_TRAVERSAL_C
#endif





