#ifndef AST_REWRITE_FRAGMENT_COLLECTOR_TRAVERSAL_C
#define AST_REWRITE_FRAGMENT_COLLECTOR_TRAVERSAL_C

#include "rewrite.h"

// Notify compiler that explicit instantiation occurs elsewhere
extern template class AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>;
extern template class AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>;

// ####################################################################
// Functions for AST_FragmentIdentificationInheritedAttributeType class
// ####################################################################

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType::
~AST_FragmentIdentificationInheritedAttributeType ()
   {
     treeFragmentFromCorrectFile = false;
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationInheritedAttributeType::
AST_FragmentIdentificationInheritedAttributeType ()
   {
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
     treeFragment = nullptr;
   }

template <class ASTNodeCollection>
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
AST_FragmentIdentificationSynthesizedAttributeType()
   {
     isAMarker = false;
     markerString = "";
     treeFragment = nullptr;

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

     ASSERT_require( unsigned(ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG) ==
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

     ASSERT_require( unsigned(ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG) ==
                     treeFragementListArray.size() );
   }

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
          if ( X.treeFragementListArray[i].size() > 0 )
             {
            // The existing list should be empty
               ASSERT_require(treeFragementListArray[i].size() == 0);
            // Since the existing list is empty we can just use the assignment operator to fill it
               treeFragementListArray[i] = X.treeFragementListArray[i];
             }
        }

     return *this;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
display ( std::string s ) const
   {
     printf ("Inside of AST_FragmentIdentificationSynthesizedAttributeType::display(%s) \n",s.c_str());
     printf ("     isAMarker = %s \n",(isAMarker == true) ? "TRUE" : "FALSE");
     printf ("     Marker string = %s \n",markerString.c_str());
     printf ("     treeFragment = %p \n",treeFragment);

     std::string treeFragmentCodeString;
     if (treeFragment != nullptr)
        {
          treeFragmentCodeString = treeFragment->unparseToString();
        }
     printf ("     treeFragment code = %s \n",treeFragmentCodeString.c_str());

     for (int i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
       // Get the name of the position (better than using just an index value)
          ASSERT_require(treeFragementListArray[i].size() >= 0);
          std::string positionName = ASTNodeCollection::markerStrings[i][0] + "/" + 
                                ASTNodeCollection::markerStrings[i][1];
          ASSERT_not_null(positionName.c_str());
          printf ("     treeFragementListArray[%2d:%80s].size() = %" PRIuPTR " \n",
               i,positionName.c_str(),treeFragementListArray[i].size());

          std::vector<SgStatement*>::const_iterator j;
          for (j = treeFragementListArray[i].begin(); j != treeFragementListArray[i].end(); j++)
             {
               ASSERT_not_null(*j);

            // Make sure this is a SgStatement object
               ASSERT_not_null( dynamic_cast<SgStatement*>(*j) );

            // generate the string and output it for debugging
               std::string sourceCode = (*j)->unparseToString();
               printf ("          ((*j)->sage_class_name() = %s) sourceCode = %s \n",
                    (*j)->sage_class_name(),sourceCode.c_str());
             }
        }
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
     ASSERT_require(isAMarker == true);
     return markerString;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
setTreeFragment ( SgStatement* astNode)
   {
     treeFragment = astNode;
   }

template <class ASTNodeCollection>
SgStatement*
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
getTreeFragment()
   {
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

     if (isAMarker == true)
        {
       // Compare against static string values in ASTFragmentRequirementStrings::targetStringArray
          int i;
          for (i = ASTNodeCollection::GlobalScopePreamble; // Used to start loop with GlobalScopeTopOfScope;
               i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG;
               i++)
             {
               if ( (markerString == ASTNodeCollection::markerStrings[i][0]) ||
                    (markerString == ASTNodeCollection::markerStrings[i][1]) )
                  {
                    returnValue = typename ASTNodeCollection::IntermediateFileStringPositionEnum(i);
                  }
             }
        }

     return returnValue;
   }

template <class ASTNodeCollection>
bool
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
isStartOrEndMarker( std::string markerString, int startOrEnd )
   {
  // Supporting function for isStartingMarker() and isEndingMarker() member functions
     bool returnValue = false;

     ASSERT_require(startOrEnd == 0 || startOrEnd == 1);

  // error checking (a more efficent implementation could just return the isAMarker data member)
  // Compare against static string values in ASTFragmentRequirementStrings::targetStringArray
     int i;
     for (i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          if (markerString == ASTNodeCollection::markerStrings[i][startOrEnd])
             {
               returnValue = true;
             }
        }

     return returnValue;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::AST_FragmentIdentificationSynthesizedAttributeType::
consistancyCheck(const std::string&) const
   {
  // Note that markers now have a valid treeFragement pointer

     bool  foundString = false;
  // Compare against static string values in ASTFragmentRequirementStrings::targetStringArray
     for (int i = 0; i < ASTNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          if (markerString == ASTNodeCollection::markerStrings[i][0])
             {
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

     ASSERT_require(foundError == false);
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

     ASSERT_not_null(astNode);
     AST_FragmentIdentificationInheritedAttributeType returnAttribute;

  // Should we use the copy constructor (or special constructor) when we first build returnAttribute?
     returnAttribute.currentFileName = inheritedValue.currentFileName;

  // Don't accept a project node since we only want to process SgFile 
  // nodes as intermediate files for AST fragment generation (not sure why!)
     ASSERT_require(astNode->variantT() != V_SgProject);

  // Make sure that we start off the traversal correctly (with the correct file name)
  // This way the use need not set the file name in the inherited attribute before 
  // starting the traversal.

     if (isSgFile(astNode) != nullptr)
        {
       // Record the filename associated with the SgFile (the current file)
          SgFile* file = dynamic_cast<SgFile*>(astNode);
          ASSERT_not_null(file);
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
          if (statementNode != nullptr)
             {
               std::string nodeFileName = statementNode->get_file_info()->get_filename();
               returnAttribute.treeFragmentFromCorrectFile = true;
             }
        }

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
     AST_FragmentIdentificationSynthesizedAttributeType returnAttribute;

  // Mark the AST fragments as being transformations so that they will be unparsed properly with the
  // other statements even though they have a different filename associated with them. This approach
  // is better than changing the filenames.
     ASSERT_not_null(astNode);
     if (astNode->get_file_info() != nullptr)
        {
          ASSERT_not_null(astNode->get_file_info());
          astNode->get_file_info()->set_isPartOfTransformation(true);
        }

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
               ASSERT_not_null(statement);

               if (inheritedValue.treeFragmentFromCorrectFile == true)
                  {
                 // Go through the list of attributes and separate out (bin) the ones that are
                 // between markers into groups (bins) that are given by the strings associated with
                 // each marker.

                 // Save the index of each starting marker as we iterate through the
                 // synthesizedAttributeList
                    int startingMarkerAttributeIndex = 0;

                    bool startingMarkerFound = false; // default value (assume starting marker has not been seen)
                    bool endingMarkerFound   = true;  // default value (assume ending marker has been seen)
                    unsigned int i;

                 // This value is set when the starting marker is found 
                 // (starting mark is found before ending marker by definition)
                    IntermediateFileStringPositionEnum locationCounter = 
                         ASTNodeCollection::unknownIntermediatePositionInScope;

                    for (i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                      // error checking
                         synthesizedAttributeList[i].consistancyCheck("Checking the child list");

                         if (synthesizedAttributeList[i].isStartingMarker() == true)
                            {
                              startingMarkerFound          = true;
                              startingMarkerAttributeIndex = i;

                           // error checking (must have found starting marker before ending marker)
                              ASSERT_require(endingMarkerFound == true);
                              endingMarkerFound = false;

                           // Turn ON statement recording
                              recordStatements = true;

                              locationCounter = synthesizedAttributeList[startingMarkerAttributeIndex].location();
                              ASSERT_require(locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);

                           // But add the starting marker variable to the list of tree fragements
                           // so that attachments to the marker nodes can be preserved and 
                           // placed into the final AST.
                              ASSERT_not_null(synthesizedAttributeList[i].treeFragment);
                              ASSERT_require(locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);

                           // insert statement into list of statements
                              returnAttribute.treeFragementListArray[locationCounter].push_back( synthesizedAttributeList[i].treeFragment );
                            }
                           else
                            {
                              if (synthesizedAttributeList[i].isEndingMarker() == true)
                                 {
                                // Nothing to do here (since the locationCounter is incremented when
                                // we find the starting marker)
                                // error checking (must have found starting marker before ending marker)
                                   ASSERT_require(startingMarkerFound == true);
                                   startingMarkerFound = false;
                                   endingMarkerFound   = true;

                                // But add the ending marker variable to the list of tree fragements
                                // so that attachments to the marker nodes can be preserved and 
                                // placed into the final AST.
                                   ASSERT_not_null(synthesizedAttributeList[i].treeFragment);
                                   ASSERT_require(locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);

                                // insert statement into list of statements
                                   returnAttribute.treeFragementListArray[locationCounter].push_back( synthesizedAttributeList[i].treeFragment );

                                // Turn OFF statement recording
                                   recordStatements = false;
                                 }
                                else
                                 {
                                   if ( (recordStatements == true) && 
                                        (synthesizedAttributeList[i].treeFragment != nullptr) )
                                      {
                                        ASSERT_require(recordStatements == true);
                                        ASSERT_not_null(synthesizedAttributeList[i].treeFragment);
                                        ASSERT_require(locationCounter != ASTNodeCollection::unknownIntermediatePositionInScope);
                                        ASSERT_require(unsigned(locationCounter) < returnAttribute.treeFragementListArray.size());

                                     // insert statement into list of statements
                                        returnAttribute.treeFragementListArray[locationCounter].push_back( synthesizedAttributeList[i].treeFragment );
                                        ASSERT_require(returnAttribute.treeFragementListArray[locationCounter].size() > 0);
                                      }
                                     else
                                      {
                                     // Could still be a synthesized attribute from a function
                                     // containing AST fragments so check for that (call the operator+=).
                                        SgStatement* statement = isSgStatement(astNode);
                                        ASSERT_not_null(statement);
                                        returnAttribute.setTreeFragment(statement);
                                      }
                                 }
                            }
                       }
                  }
               break;
             }

          case V_SgVariableDeclaration:
             {
               SgVariableDeclaration* varDeclaration = isSgVariableDeclaration (astNode);
               ASSERT_not_null(varDeclaration);
               SgInitializedNamePtrList & nameList = varDeclaration->get_variables();
               if (nameList.size() == 1)
                  {
                 // The markers we have defined are a single integer variable
                    SgInitializedName* initializedName = *(nameList.begin());
                    SgName name = initializedName->get_name();
                    std::string variableName = name.str();

                 // This could be done in the inherited attribute!!!
                    bool foundMarker = ASTNodeCollection::isAMarkerString(variableName);
                    if ( foundMarker == true )
                       {
                      // Setting the marker here simplifies the test for the marker in the
                      // SgBasicBlock and SgGlobal cases above (though it does make for more 
                      // convoluted code so this implementation might be changed to simplify 
                      // the logic later)
                         returnAttribute.setMarker(variableName);
                       }
                 // error checking
                    returnAttribute.consistancyCheck("Checking the returnAttribute: SgVariableDeclaration case");
                  }

            // NOTE: FALL THROUGH OF CASE!!!
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

          case V_SgCaseOptionStmt:
          case V_SgDefaultOptionStmt:

          case V_SgNamespaceDefinitionStatement:
          case V_SgNullStatement:

          case V_SgUsingDirectiveStatement:
          case V_SgNamespaceDeclarationStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          case V_SgTemplateInstantiationDirectiveStatement:
             {
               SgStatement* statement = isSgStatement(astNode);
               ASSERT_not_null(statement);
            // Can't make this conditional dependent upon if recording is set since the child 
            // synthesised attributes a build before processin the basic block or global scope 
            // attributes.
               if ( inheritedValue.treeFragmentFromCorrectFile == true )
                  {
                 // Attach the current statement to the synthesized attribute so that 
                 // they can be collected in the SgGlobal and SgBasicBlock cases (above).
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

          default:
             {
            // Make sure this is not a SgStatement (they should all be handled 
            // above (so this is error checking)).
               if (isSgStatement(astNode) != nullptr) {
                    printf ("Error Default reached in switch: astNode->sage_class_name = %s \n",astNode->sage_class_name());
               }
               ASSERT_require(isSgStatement(astNode) == nullptr);
             }
        }

  // error checking
     returnAttribute.consistancyCheck("Checking the returnAttribute: default case");

  // Accumulate data from child synthesized attributes into the returnAttribute
     for (unsigned int i = 0; i < synthesizedAttributeList.size(); i++)
        {
       // Can't have both non-empty lists and valid treeFragment pointer
          if (synthesizedAttributeList[i].treeFragment != nullptr)
             {
               synthesizedAttributeList[i].treeFragment = nullptr;
             }

       // error checking
          synthesizedAttributeList[i].consistancyCheck("Checking the child list");
       // Accumulate all the SgStatements in the different lists
          returnAttribute += synthesizedAttributeList[i];
        }

  // error checking
     returnAttribute.consistancyCheck("Checking the returnAttribute");
     
     return returnAttribute;
   }

// endif for AST_REWRITE_FRAGMENT_COLLECTOR_TRAVERSAL_C
#endif





