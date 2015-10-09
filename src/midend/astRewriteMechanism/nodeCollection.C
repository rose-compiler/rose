
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rewrite.h"
#include "AstConsistencyTests.h"
// DQ (5/26/2007): Removed support for older AST merge
// DQ (7/25/2005): Added to support AST Merge (by Milind)
// #include "AstMerge.h"
// #include "AstFixParentTraversal.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// ************************************************************
// ************************************************************

// pmp 08JUN05
//   rewrote the type of the declaration for gcc 3.4
//   was: string AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>::
//        markerStrings [MidLevelCollectionTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG][2] = 
template<>
AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>::MarkerCollectionType 
AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>::markerStrings =
          { { "unknownPositionInScopeStart"       ,  "unknownPositionInScopeEnd"       } ,
            { "GlobalScopePreambleStart"          ,  "GlobalScopePreambleEnd"          } ,
            { "CurrentLocationTopOfScopeStart"    ,  "CurrentLocationTopOfScopeEnd"    } ,
            { "CurrentLocationBeforeStart"        ,  "CurrentLocationBeforeEnd"        } ,
            { "CurrentLocationReplaceStart"       ,  "CurrentLocationReplaceEnd"       } ,
            { "CurrentLocationAfterStart"         ,  "CurrentLocationAfterEnd"         } ,
            { "CurrentLocationBottomOfScopeStart" ,  "CurrentLocationBottomOfScopeEnd" } };

// pmp 08JUN05
//   cmp previous comment
//   was: string AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>::
//        markerStrings [HighLevelCollectionTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG][2] =
template<>
AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>::MarkerCollectionType 
AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>::markerStrings =
          { { "unknownPositionInScopeStart" ,              "unknownPositionInScopeEnd"              } ,
            { "GlobalScopePreambleStart" ,                 "GlobalScopePreambleEnd"                 } ,
            { "GlobalScopeTopOfScopeStart" ,               "GlobalScopeTopOfScopeEnd"               } ,
            { "GlobalScopeTopOfIncludeRegionStart" ,       "GlobalScopeTopOfIncludeRegionEnd"       } ,
            { "GlobalScopeBottomOfIncludeRegionStart" ,    "GlobalScopeBottomOfIncludeRegionEnd"    } ,
            { "GlobalScopeBeforeCurrentPositionStart" ,    "GlobalScopeBeforeCurrentPositionEnd"    } ,
            { "GlobalScopeReplaceCurrentPositionStart" ,   "GlobalScopeReplaceCurrentPositionEnd"   } ,
            { "FunctionScopePreambleStart" ,               "FunctionScopePreambleEnd"               } ,
            { "FunctionScopeTopOfScopeStart" ,             "FunctionScopeTopOfScopeEnd"             } ,
            { "FunctionScopeBeforeCurrentPositionStart" ,  "FunctionScopeBeforeCurrentPositionEnd"  } ,
            { "FunctionScopeReplaceCurrentPositionStart" , "FunctionScopeReplaceCurrentPositionEnd" } ,
            { "FunctionScopeAfterCurrentPositionStart" ,   "FunctionScopeAfterCurrentPositionEnd"   } ,
            { "FunctionScopeBottomOfScopeStart" ,          "FunctionScopeBottomOfScopeEnd"          } ,
            { "GlobalScopeAfterCurrentPositionStart" ,     "GlobalScopeAfterCurrentPositionEnd"     } ,
            { "GlobalScopeBottomOfScopeStart" ,            "GlobalScopeBottomOfScopeEnd"            } };

// ************************************************************
// ************************************************************

// #########################################################
// #########################################################

MidLevelInterfaceNodeCollection::MidLevelInterfaceNodeCollection()
   {
     int i;
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          list<string> tempStringList;
          sortedTransformationStrings.push_back(tempStringList);

          SgStatementPtrList tempStatementList;
          treeFragement.push_back(tempStatementList);
        }

     ROSE_ASSERT (sortedTransformationStrings.size() > 0);
     ROSE_ASSERT (sortedTransformationStrings.size() == 
                  unsigned(LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG));

     ROSE_ASSERT (treeFragement.size() > 0);
     ROSE_ASSERT (treeFragement.size() == 
                  unsigned(LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG));

  // printf ("Inside of MidLevelInterfaceNodeCollection constructor \n");
  // ROSE_ABORT();
   }

MidLevelInterfaceNodeCollection::~MidLevelInterfaceNodeCollection()
   {
     int i;
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          sortedTransformationStrings.pop_back();
          treeFragement.pop_back();
        }

     ROSE_ASSERT (sortedTransformationStrings.size() == 0);
     ROSE_ASSERT (treeFragement.size()               == 0);
   }

MidLevelInterfaceNodeCollection &
MidLevelInterfaceNodeCollection::operator= ( const MidLevelInterfaceNodeCollection & X )
   {
     AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>::operator=(X);

#if 0
     printf ("Inside of MidLevelInterfaceNodeCollection::operator=() \n");
     ROSE_ABORT();
#endif

     return *this;
   }

bool
MidLevelInterfaceNodeCollection::isAMarkerString ( string s )
   {
  // This function uses the staticly defined list of string names
     bool returnResult = false;

     ROSE_ASSERT (int(MidLevelInterfaceNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG) <
                  int(HighLevelInterfaceNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG) );

     int i,j;
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
          for (j = 0; j < 2; j++)
             {
            // printf ("In isAMarkerString(): markerStrings[%d][%d] = %s \n",i,j,markerStrings[i][j].c_str());
               if (s == markerStrings[i][j])
                    returnResult = true;
             }
#if 0
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG+1; i++)
          for (j = 0; j < 4; j++)
             {
               printf ("In isAMarkerString(): markerStrings[%d][%d] = %s \n",i,j,markerStrings[i][j].c_str());
             }

     printf ("Exiting after printing out static array of strings \n");
     ROSE_ABORT();
#endif

  // printf ("Inside of MidLevelInterfaceNodeCollection::isAMarkerString(%s) = %s \n",s.c_str(),(returnResult == true) ? "true" : "false");

     return returnResult;
   }

HighLevelInterfaceNodeCollection::HighLevelInterfaceNodeCollection()
   {
     int i;
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          list<string> tempStringList;
          sortedTransformationStrings.push_back(tempStringList);

          SgStatementPtrList tempStatementList;
          treeFragement.push_back(tempStatementList);
        }

     ROSE_ASSERT (sortedTransformationStrings.size() > 0);
     ROSE_ASSERT (sortedTransformationStrings.size() == 
                  unsigned(LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG));

     ROSE_ASSERT (treeFragement.size() > 0);
     ROSE_ASSERT (treeFragement.size() == 
                  unsigned(LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG));

  // printf ("Inside of HighLevelInterfaceNodeCollection constructor \n");
  // ROSE_ABORT();
   }

HighLevelInterfaceNodeCollection::~HighLevelInterfaceNodeCollection()
   {
     int i;
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          sortedTransformationStrings.pop_back();
          treeFragement.pop_back();
        }

     ROSE_ASSERT (sortedTransformationStrings.size() == 0);
     ROSE_ASSERT (treeFragement.size()               == 0);
   }

HighLevelInterfaceNodeCollection &
HighLevelInterfaceNodeCollection::operator= ( const HighLevelInterfaceNodeCollection & X )
   {
     AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>::operator=(X);

#if 0
     printf ("Inside of HighLevelInterfaceNodeCollection::operator=() \n");
     ROSE_ABORT();
#endif

     return *this;
   }

bool
HighLevelInterfaceNodeCollection::isAMarkerString ( string s )
   {
  // This function uses the staticly defined list of string names
     bool returnResult = false;

#if 0
     ROSE_ASSERT (MidLevelInterfaceNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG <
                  HighLevelInterfaceNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG);
#endif

     int i,j;
     for (i = 0; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
          for (j = 0; j < 2; j++)
               if (s == markerStrings[i][j])
                    returnResult = true;

     return returnResult;
   }


HighLevelInterfaceNodeCollection::IntermediateFileStringPositionEnum
HighLevelInterfaceNodeCollection::
map ( SgNode* astNode, PlacementPositionEnum location )
   {
  // This function maps the enums from one 1D space to the 1D space of enums required to specify the
  // location of the strings in the intermediate file used to build the AST fragments. Eventually,
  // the mapping will not be so trivial and this function allows for that inevitablity.
  // Since the range of the map operator can include both global scope and local function 
  // scope AND the position in each scope, we need the astNode value to indicate if we want 
  // the output value in either global or function scope.

  // If the trasfomation is for global scope make sure it is placed there in the intermediate 
  // file so that it can be checked at compilation of the intermediate file.
     bool useGlobalScope = isInGlobalScope(astNode);

     IntermediateFileStringPositionEnum returnValue = unknownIntermediatePositionInScope;

     switch(location)
        {
       // all possible locations
          case PreamblePositionInScope:
               returnValue = GlobalScopePreamble;
               break;
          case TopOfIncludeRegion:
               returnValue = GlobalScopeTopOfIncludeRegion;
               break;
          case BottomOfIncludeRegion:
               returnValue = GlobalScopeBottomOfIncludeRegion;
               break;
          case TopOfScope:
            // returnValue = (useGlobalScope) ? GlobalScopeBottomOfIncludeRegion : FunctionScopeTopOfScope;
               returnValue = (useGlobalScope) ? GlobalScopeTopOfScope : FunctionScopeTopOfScope;
               break;
          case BeforeCurrentPosition:
            // returnValue = (useGlobalScope) ? GlobalScopeBottomOfIncludeRegion : FunctionScopeBeforeCurrentPosition;
               returnValue = (useGlobalScope) ? GlobalScopeBeforeCurrentPosition : FunctionScopeBeforeCurrentPosition;
               break;
          case ReplaceCurrentPosition:
            // returnValue = (useGlobalScope) ? GlobalScopeBottomOfIncludeRegion : FunctionScopeReplaceCurrentPosition;
               returnValue = (useGlobalScope) ? GlobalScopeReplaceCurrentPosition : FunctionScopeReplaceCurrentPosition;
               break;
          case AfterCurrentPosition:
            // returnValue = (useGlobalScope) ? GlobalScopeBottomOfScope : FunctionScopeAfterCurrentPosition;
               returnValue = (useGlobalScope) ? GlobalScopeAfterCurrentPosition : FunctionScopeAfterCurrentPosition;
               break;
          case BottomOfScope:
               returnValue = (useGlobalScope) ? GlobalScopeBottomOfScope : FunctionScopeBottomOfScope;
               break;

          default:
               printf ("default reached in switch(location): HighLevelInterfaceNodeCollection::map() value = %s \n",
                    getRelativeLocationString(location).c_str());
               ROSE_ABORT();
        }

#if 0
     printf ("In HighLevelInterfaceNodeCollection::map(): case %d (%s) maps to case %d (%s) \n",
          location,getRelativeLocationString(location).c_str(),
          returnValue,getIntermediateFilePositionName(returnValue).c_str());
#endif

#if 0
     printf ("Exiting in map function \n");
     ROSE_ABORT();
#endif

     return returnValue;
   }

MidLevelInterfaceNodeCollection::IntermediateFileStringPositionEnum
MidLevelInterfaceNodeCollection::map ( ScopeIdentifierEnum scope, PlacementPositionEnum location )
   {
  // This map function takes the relative scope and position in scope and maps 
  // that data to a location in the intermediate file generated to translate 
  // the strings into AST nodes.

     IntermediateFileStringPositionEnum returnPosition = unknownIntermediatePositionInScope;

     switch (scope)
        {
          case unknownScope:
             {
               printf ("ERROR: unknownScope not permitted value \n");
               ROSE_ABORT();
             }
          case Preamble:
               returnPosition = GlobalScopePreamble;
               break;
          case SurroundingScope:
             {
            // Within a local scope we can specify several relative positions 
            // (relative to the current statement and two absolute positions 
            // (top and bottom of scope)).
               switch (location)
                  {
                    case unknownPositionInScope:
                       {
                         printf ("ERROR: unknownPositionInScope not permitted value \n");
                         ROSE_ABORT();
                       }
                    case TopOfCurrentScope:
                         returnPosition = CurrentLocationTopOfScope;
                         break;
                    case BeforeCurrentPosition:
                         returnPosition = CurrentLocationBefore;
                         break;
                    case ReplaceCurrentPosition:
                       {
                         returnPosition = CurrentLocationReplace;
                         break;
                       }
                    case AfterCurrentPosition:
                       {
                         returnPosition = CurrentLocationAfter;
                         break;
                       }
                    case BottomOfCurrentScope:
                         returnPosition = CurrentLocationBottomOfScope;
                         break;
                    default:
                       {
                         printf ("ERROR: default reached (location in scope) \n");
                         ROSE_ABORT();
                       }
                  }
               break;
             }
          case StatementScope:
             {
            // Within a local scope we can specify several relative positions 
            // (relative to the current statement and two absolute positions 
            // (top and bottom of scope)).
               switch (location)
                  {
                    case unknownPositionInScope:
                       {
                         printf ("ERROR: unknownPositionInScope not permitted value \n");
                         ROSE_ABORT();
                       }

                    case BeforeCurrentPosition:
                         returnPosition = CurrentLocationBefore;
                         break;
                    case AfterCurrentPosition:
                         returnPosition = CurrentLocationAfter;
                         break;

                    case TopOfCurrentScope:
                    case BottomOfCurrentScope:
                    case ReplaceCurrentPosition:
                       {
                         printf ("ERROR: StatementScope and (BeforeCurrentPosition or ReplaceCurrentPosition or AfterCurrentPosition) not permitted value \n");
                         ROSE_ABORT();
                         break;
                       }

                    default:
                       {
                         printf ("ERROR: default reached (location in scope) \n");
                         ROSE_ABORT();
                       }
                  }
               break;
             }
          default:
             {
               printf ("ERROR: default reached (relativeScope) \n");
               ROSE_ABORT();
             }
        }

     return returnPosition;
   }

void
MidLevelInterfaceNodeCollection::sortInputStrings( SgNode* astNode )
   {
  // This function bins the transformation strings by the output of the map 
  // function given the input of the scope and position in scope recoreded 
  // in the transformation string objects.

     ROSE_ASSERT (inputTransformationStrings.size() > 0);
     ROSE_ASSERT (numberOfTransformationStrings() > 0);

     InputTransformationStringCollectionType::iterator i;
     for (i = inputTransformationStrings.begin(); i != inputTransformationStrings.end(); i++)
        {
          ScopeIdentifierEnum scope = (*i).relativeScope;
          PlacementPositionEnum relativePosition = (*i).relativeLocation;
          int absolutePosition  = map (scope,relativePosition);

          string transformationString = (*i).sourceCode;

//        printf ("transformationString = %s \n",transformationString.c_str());

          ROSE_ASSERT (sortedTransformationStrings.size() == 
                       unsigned(LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG));

          sortedTransformationStrings[int(absolutePosition)].push_back(transformationString);
        }

#if 0
     printf ("Exiting in MidLevelInterfaceNodeCollection::sortInputStrings() ... \n");
     ROSE_ABORT();
#endif
   }

void
HighLevelInterfaceNodeCollection::sortInputStrings ( SgNode* astNode )
   {
  // This function bins the transformation strings by the output of the map 
  // function given the input of the scope and position in scope recoreded 
  // in the transformation string objects.

     ROSE_ASSERT (inputTransformationStrings.size() > 0);
     ROSE_ASSERT (numberOfTransformationStrings() > 0);

     InputTransformationStringCollectionType::iterator i;
     for (i = inputTransformationStrings.begin(); i != inputTransformationStrings.end(); i++)
        {
          PlacementPositionEnum relativePosition = (*i).relativeLocation;
          SgNode* astNode = (*i).associatedASTNode;
          ROSE_ASSERT (astNode != NULL);

          int absolutePosition  = map (astNode,relativePosition);

          string transformationString = (*i).sourceCode;

#if 0
          printf ("sortInputStrings(astNode=%s) at %s : transformationString = %s \n",
               astNode->sage_class_name(),
               getRelativeLocationString(relativePosition).c_str(),
               transformationString.c_str());
#endif

          ROSE_ASSERT (sortedTransformationStrings.size() > 0);
          ROSE_ASSERT (sortedTransformationStrings.size() == 
                       unsigned(LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG));

          sortedTransformationStrings[int(absolutePosition)].push_back(transformationString);
        }

     list<string> preambleStringList = getPreambleStrings(astNode,SurroundingScope,FileScope);
     sortedTransformationStrings[int(GlobalScopePreamble)] = preambleStringList;

#if 0
     printf ("Exiting in HighLevelRewrite::sortInputStrings() ... \n");
     ROSE_ABORT();
#endif
   }


void
MidLevelInterfaceNodeCollection::compileSortedStringsToGenerateNodeCollection( SgNode* astNode, bool prefixIncludesCurrentStatement )
   {
     ROSE_ASSERT (numberOfTransformationStrings() > 0);
     ROSE_ASSERT (numberOfSortedTransformationStrings() > 0);

  // printf ("Inside of MidLevelInterfaceNodeCollection::compileSortedStringsToGenerateNodeCollection() \n");

  // int currentLocation = CurrentLocation;

     ROSE_ASSERT ( (sortedTransformationStrings[CurrentLocationTopOfScope   ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationBefore       ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationReplace      ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationAfter        ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationBottomOfScope].size() > 0) );

  // Generate the string that will become the intermediate file and be compiled 
  // to build the AST fragements we require.
     string transformationFileString = generateIntermediateFileString(astNode,prefixIncludesCurrentStatement);

  // printf ("Debugging MidLevelInterfaceNodeCollection: transformationFileString = \n%s\n",transformationFileString.c_str());

     treeFragement = MidLevelRewrite<MidLevelInterfaceNodeCollection>::fileStringToNodeCollection (astNode,transformationFileString);
  // ROSE_ASSERT (numberOfStatements() > 0);

#if 0
     display("In MidLevelInterfaceNodeCollection::compileSortedStringsToGenerateNodeCollection(): After fileStringToNodeCollection() \n");
     printf ("Exiting in MidLevelInterfaceNodeCollection::compileSortedStringsToGenerateNodeCollection() ... \n");
     ROSE_ASSERT(false);
#endif
   }

void
HighLevelInterfaceNodeCollection::compileSortedStringsToGenerateNodeCollection( SgNode* astNode, bool prefixIncludesCurrentStatement )
   {
     ROSE_ASSERT (numberOfTransformationStrings() > 0);
     ROSE_ASSERT (numberOfSortedTransformationStrings() > 0);

  // printf ("Inside of HighLevelInterfaceNodeCollection::compileSortedStringsToGenerateNodeCollection() \n");

  // Generate the string that will become the intermediate file and be compiled 
  // to build the AST fragements we require.
     string transformationFileString = generateIntermediateFileString(astNode,prefixIncludesCurrentStatement);

  // printf ("Debugging HighLevelInterfaceNodeCollection: transformationFileString = \n%s\n",transformationFileString.c_str());

  // Put the string into a file, compile it and extract the AST fragements 
  // (organized as a list that corresponds to the vector of string lists 
  // in sortedTransformationStrings).
     treeFragement = HighLevelRewrite::fileStringToNodeCollection (astNode,transformationFileString);
  // ROSE_ASSERT (numberOfStatements() > 0);
   }

void
HighLevelInterfaceNodeCollection::
permitedLocationToInsertNewCode (
     SgNode* astNode, 
     bool & insertIntoGlobalScope,
     bool & insertIntoFunctionScope )
   {
  // This function contains the logic for controling which AST fragments from the
  // ASTFragmentContainer will be inserted into the applications AST.  This depends upon
  // the current location in the AST and make intuative sense but has not trivial logic (so
  // we isolate the logic here so that it can be used by different functions:
  //      HighLevelRewrite::ASTFragmentContainer::checkConsistancy()
  //  and AST_Rewrite::insert()).

   // Only SgGlobal and SgBasicBlock scope object count (see also ???)
   // SgScopeStatement* currentScope = dynamic_cast<SgScopeStatement*>(astNode);
      SgScopeStatement* currentScope = NULL;
      if (dynamic_cast<SgGlobal*>(astNode) || dynamic_cast<SgBasicBlock*>(astNode) )
           currentScope = dynamic_cast<SgScopeStatement*>(astNode);
      SgScopeStatement* parentScope  = NULL;

      if ( currentScope == NULL )
         {
        // Some parent scopes are not immediately above the current node 
        // (e.g. functions: SgBasicBlock -> SgFunctionDefinition -> SgFunctionDeclaration -> SgGlobal)
           SgStatement* statement = dynamic_cast<SgStatement*>(astNode);
           while (parentScope == NULL && statement != NULL)
              {
                parentScope  = dynamic_cast<SgScopeStatement*>(statement->get_parent());
             // statement = statement->get_parent();
                statement = isSgStatement(statement->get_parent());
              }

          ROSE_ASSERT (parentScope != NULL);
         }

  // Don't put global scope AST fragment into anything but the global scope
     insertIntoGlobalScope   = ( dynamic_cast<SgGlobal*>(currentScope) != NULL ||
                                 dynamic_cast<SgGlobal*>(parentScope)  != NULL ) ? true : false;

  // Insert more general AST fragments (any SgStatement) into any other scope
     insertIntoFunctionScope = !insertIntoGlobalScope && ( currentScope != NULL ) ? true : false;

#if 0
     printf ("astNode->sage_class_name() = %s \n",astNode->sage_class_name());
     printf ("parentScope->sage_class_name() = %s \n",(parentScope == NULL) ? "NULL POINTER" : parentScope->sage_class_name());

     printf ("insertIntoGlobalScope = %s \n",(insertIntoGlobalScope) ? "true" : "false");
     printf ("insertIntoFunctionScope = %s \n",(insertIntoFunctionScope) ? "true" : "false");
#endif
   }

void
MidLevelInterfaceNodeCollection::
insertContainerOfListsOfStatements ( SgStatement* astNode )
   {
  // Place the AST fragments into the application's AST

     ROSE_ASSERT (astNode != NULL);

     ROSE_ASSERT ( (sortedTransformationStrings[CurrentLocationTopOfScope   ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationBefore       ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationReplace      ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationAfter        ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationBottomOfScope].size() > 0) );

#if 0
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
     display("Called in insertContainerOfListsOfStatements()");
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#endif

#if 0
  // Error checking (turn off to allow simple tests to 
  // pass (else initial test codes become quite complex)).
     checkConsistancy(astNode);
#endif

  // Insert into current position
     int i;
     for (i = CurrentLocationTopOfScope; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
       // printf ("Looping through the different locations in scope (%s) \n",
       //      MidLevelCollectionTypedefs::getRelativeLocationString(PlacementPositionEnum(i)).c_str());

       // Separate out the special case of replace (since replace implies removal of the current statement)
          if (IntermediateFileStringPositionEnum(i) == CurrentLocationReplace)
             {
            // Test if there was anything specified to replace the current node
               if (sortedTransformationStrings[CurrentLocationReplace].size() > 0)
//             if (treeFragement[int(CurrentLocationReplace)].size() > 0)
                  {
                 // If so then call replace.  This allows silly things like "" strings to 
                 // replace a statement which is required to complete the string representation 
                 // of the specification of transformations
                    insertStatementListWithMarkers ( astNode,treeFragement[i],IntermediateFileStringPositionEnum(i) );
                  }
                 else
                  {
                 // printf ("Skipping replacement of current location since nothing was specified as input! \n");
                  }
             }
            else
             {
            // printf ("Calling insertStatementListWithMarkers(astNode = %s) \n",astNode->sage_class_name());
               insertStatementListWithMarkers ( astNode,treeFragement[i],IntermediateFileStringPositionEnum(i) );
             }
        }
   }

void
HighLevelInterfaceNodeCollection::
insertContainerOfListsOfStatements ( SgStatement* astNode )
// insertContainerOfListsOfStatements ( SgNode* astNode, const CollectionType & X )
   {
  // Place the AST fragments into the application's AST

  // Note that some insertions don't make sense and so are not done.  The 
  // function attempts to be smart about this so that it is harder to make 
  // errors. AST fragments targeted to the global scope will only be inserted
  // if the astNode is positioned at and SgGlobal node or a SgDeclarationStatement 
  // in the global scope.  Similarly more general statements targeted at a any scope 
  // other than the global scope are only placed into that scope if the input 
  // astNode is not a SgGlobal or a SgDeclarationStatement in the global scope.

  // Insertions must occur in a specific order to end up with a correct 
  // ordering of the final statements in the application code.

  // The preambles don't get inserted into the final applications AST

     ROSE_ASSERT(astNode != NULL);

#if 0
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
     printf ("HighLevelInterfaceNodeCollection::insertContainerOfListsOfStatements(%s) \n",astNode->sage_class_name());
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
     printf ("astNode = %p = %s \n",astNode,astNode->unparseToString().c_str());
     ROSE_ASSERT(astNode->get_parent() != NULL);
     printf ("astNode->get_parent() = %p = %s \n",astNode->get_parent(),astNode->get_parent()->unparseToString().c_str());
     display("Called in insertContainerOfListsOfStatements()");
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#endif

#if 0
  // Error checking (turn off to allow simple tests to 
  // pass (else initial test codes become quite complex)).
     checkConsistancy(astNode);
#endif

     bool insertIntoGlobalScope,insertIntoFunctionScope;
     permitedLocationToInsertNewCode (astNode,insertIntoGlobalScope,insertIntoFunctionScope);

     ROSE_ASSERT (astNode != NULL);

#if 0
     printf ("In insertContainerOfListsOfStatements(%s) \n",astNode->sage_class_name());
     printf ("     insertIntoGlobalScope   = %s \n",(insertIntoGlobalScope   == true) ? "true" : "false");
     printf ("     insertIntoFunctionScope = %s \n",(insertIntoFunctionScope == true) ? "true" : "false");
#endif

     if (insertIntoGlobalScope == true)
        {
       // Insert new statement on top of the include statements before inserting 
       // statement at the top of the global scope.
          insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeTopOfIncludeRegion],GlobalScopeTopOfIncludeRegion);
          insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeTopOfScope],GlobalScopeTopOfScope);

          insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeBottomOfIncludeRegion],GlobalScopeBottomOfIncludeRegion);

       // Insertions relative the the current statment in the global scope can't be input at 
       // the global scope since they need a pointer to the current statement that they are 
       // relative too the position of.  So these don't make any sense in the global scope.
       // However, they should be included in the generation of the intermediate source 
       // file since might contain declarations used by statementnts inserted at the bottom 
       // of the scope, just that we don't have to insert them here if we are positioned at 
       // a global scope.
          if (isSgGlobal(astNode) == NULL)
             {
            // Insert this before the current function in the global scope
               ROSE_ASSERT (astNode != NULL);
               ROSE_ASSERT (dynamic_cast<SgGlobal*>(astNode) == NULL);

            // Insert code around the current position but in the global scope
               insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeBeforeCurrentPosition],GlobalScopeBeforeCurrentPosition);
               insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeAfterCurrentPosition],GlobalScopeAfterCurrentPosition);
             }

          insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeBottomOfScope],GlobalScopeBottomOfScope);

       // Do all replacements last!
          insertStatementListWithMarkers( astNode,treeFragement[GlobalScopeReplaceCurrentPosition],GlobalScopeReplaceCurrentPosition);
        }
       else
        {
       // Relative positions are in terms of a SgStatement in a SgBasicBlock
       // (or SgScopeStatement in the most general case)
       // printf ("insertIntoFunctionScope = %s \n",(insertIntoFunctionScope) ? "true" : "false");
          if (insertIntoFunctionScope == true)
             {
               insertStatementListWithMarkers( astNode,treeFragement[FunctionScopeTopOfScope],FunctionScopeTopOfScope);
            // Insert statements at the bottom of the function and global scopes in either order
               insertStatementListWithMarkers( astNode,treeFragement[FunctionScopeBottomOfScope],FunctionScopeBottomOfScope);
             }
            else
             {
            // Insert statements in the top of the function scope last
               insertStatementListWithMarkers( astNode,treeFragement[FunctionScopeBeforeCurrentPosition],FunctionScopeBeforeCurrentPosition);
            // Insert the statements before the target statement before any statements that replace the current statement
               insertStatementListWithMarkers( astNode,treeFragement[FunctionScopeAfterCurrentPosition],FunctionScopeAfterCurrentPosition);

            // The replacement case has to be handled a bit special.  We need 
            // to permit empty strings to trigger calls to replace statements.
               if (sortedTransformationStrings[FunctionScopeReplaceCurrentPosition].size() > 0)
                  {
                 // If so then call replace.  This allows silly things like "" strings to 
                 // replace a statement which is required to complete the string representation 
                 // of the specification of transformations
                    insertStatementListWithMarkers( astNode,
                                                    treeFragement[FunctionScopeReplaceCurrentPosition],
                                                    FunctionScopeReplaceCurrentPosition);
                  }
                 else
                  {
                 // printf ("Skipping replacement of current location since nothing was specified as input! \n");
                  }
              }
         }

#if 0
     printf ("Inside of HighLevelInterfaceNodeCollection::insertContainerOfListsOfStatements(): exiting ... \n");
     ROSE_ABORT();
#endif
   }

void
MidLevelInterfaceNodeCollection::
insertStatementList ( 
   SgStatement* target,
   const SgStatementPtrList & X,
   AttachedPreprocessingInfoType & commentsAndDirectives,
   IntermediateFileStringPositionEnum positionInFile )
   {
#if 0
     printf ("In MidLevelInterfaceNodeCollection::insertStatementList(): target = %p target->sage_class_name() = %s \n",
          target,target->sage_class_name());
     printf ("     positionInFile = %d positionName() = %s \n",
          positionInFile,getIntermediateFilePositionName(positionInFile).c_str());
     printf ("     SgStatementPtrList size (X.size() = %" PRIuPTR ") \n",X.size());
     printf ("     commentsAndDirectives.size() = %" PRIuPTR " \n",commentsAndDirectives.size());
#endif

  // If there is nothing to insert or replace with, then exit
     if ( (X.size() == 0) && (commentsAndDirectives.size() == 0) )
          return;

     switch (positionInFile)
        {
          case unknownIntermediatePositionInScope:
               printf ("Error: insertStatementList() unknownPositionInScope case \n");
               ROSE_ABORT();
               break;

          case GlobalScopePreamble:
             {
               SgScopeStatement* scope = isSgScopeStatement(target);
               if (scope == NULL)
                  {
                 // If NULL it could just be a function in the global scope
                 // Get the parent node and check if it is a scope
                    SgStatement* statement = dynamic_cast<SgStatement*>(target);
                    ROSE_ASSERT (statement != NULL);
                    SgStatement* statementParent = isSgStatement(statement->get_parent());
                    scope = isSgScopeStatement(statementParent);
                    ROSE_ASSERT (scope != NULL);
                  }

               ROSE_ASSERT (scope != NULL);
               LowLevelRewrite::insertAtTopOfScope (X,commentsAndDirectives,scope);
               break;
             }

          case CurrentLocationTopOfScope:
             {
            // Current location must be a scope (only the relative rewrite interface permits a more general mechanism)
               SgScopeStatement* scope = isSgScopeStatement(target);
               if (scope == NULL)
                  {
                    printf ("ERROR: Attempt to insert statements/comments/directives at top of scope in non-scope statement \n");
                    printf ("target->sage_class_name() = %s \n",target->sage_class_name());
                    ROSE_ABORT();
                  }

               ROSE_ASSERT (scope != NULL);
               LowLevelRewrite::insertAtTopOfScope (X,commentsAndDirectives,scope);
               break;
             }

          case CurrentLocationBefore:
            // The target should not be a scope, it should be the statement 
            // that we want to insert a new statement list in front of.  This could be
            // a SgBasicBlock in the most general case, but we introduce this test to 
            // help debug the cases where the target is not a SgBasicBlock.  This 
            // constraint will be removed later for handle the most general case.
            // printf ("Calling LowLevelRewrite::insertBeforeCurrentStatement() \n");
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertBeforeCurrentStatement (X,commentsAndDirectives,target);
               break;

       // Not clear yet if we need this yet
          case CurrentLocationReplace:
            // ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
            // printf ("In MidLevelInterfaceNodeCollection::insertStatementList(): target = %s \n",target->sage_class_name());
#if 1
               LowLevelRewrite::insertReplaceCurrentStatement (X,commentsAndDirectives,target);
#else
               if ( dynamic_cast<SgBasicBlock*>(target) == NULL && 
                    dynamic_cast<SgFunctionDefinition*>(target) == NULL &&
                    dynamic_cast<SgGlobal*>(target) == NULL )
                  {
                 // simple case of statement in a scope
                    LowLevelRewrite::insertReplaceCurrentStatement (X,commentsAndDirectives,target);
                  }
                 else
                  {
                 // replace the block held within the parent statement
                    ROSE_ASSERT (target->get_parent() != NULL);
                    SgNode* parentNode = target->get_parent();
                    if (parentNode != NULL)
                       {
                         switch (parentNode->variantT())
                            {
                              case V_SgFunctionDefinition:
                                 {
                                   SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(parentNode);
                                   ROSE_ASSERT (functionDefinition != NULL);
                                   SgBasicBlock* newBasicBlock = isSgBasicBlock(target);
                                   ROSE_ASSERT (newBasicBlock != NULL);
                                   functionDefinition->set_body(newBasicBlock);
                                   ROSE_ASSERT (newBasicBlock->get_parent() != NULL);
                                   break;
                                 }
                              case V_SgFunctionDeclaration:
                                 {
                                   SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentNode);
                                   ROSE_ASSERT (functionDeclaration != NULL);
                                   SgFunctionDefinition* newFunctionDefinition = isSgFunctionDefinition(target);
                                   ROSE_ASSERT (newFunctionDefinition != NULL);
                                   functionDeclaration->set_definition(newFunctionDefinition);
                                   ROSE_ASSERT (newFunctionDefinition->get_parent() != NULL);
                                   break;
                                 }
                           // case V_SgFile:
                              case V_SgSourceFile:
                                 {
                                   SgSourceFile* file = isSgSourceFile(parentNode);
                                   ROSE_ASSERT (file != NULL);
                                   SgGlobal* newGlobalScope = isSgGlobal(target);
                                   ROSE_ASSERT (newGlobalScope != NULL);
                                   file->set_root(newGlobalScope);
                                   ROSE_ASSERT (newGlobalScope->get_parent() != NULL);
                                   break;
                                 }
                              default:
                                   printf ("Error, default reached in switch parentNode = %s \n",parentNode->sage_class_name());
                                   ROSE_ASSERT(false);
                            }
                       }
                      else
                       {
                         printf ("Found a NULL parent pointer at a %s \n",target->sage_class_name());
                         ROSE_ASSERT (false);
                       }
                  }
#endif
               break;

          case CurrentLocationAfter:
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertAfterCurrentStatement (X,commentsAndDirectives,target);
               break;

          case CurrentLocationBottomOfScope:
             {
            // Xurrent location must be a scope (only the relative rewrite interface permits a more general mechanism)
               SgScopeStatement* scope = isSgScopeStatement(target);
               if (scope == NULL)
                  {
                    printf ("ERROR: Attempt to insert statements/comments/directives at bottom of scope in non-scope statement \n");
                    ROSE_ABORT();
                  }

               ROSE_ASSERT (scope != NULL);
               LowLevelRewrite::insertAtBottomOfScope (X,commentsAndDirectives,scope);
               break;
             }

          default:
               printf ("Error: default found in AST_Rewrite::insert(SgStatementPtrList,SgNode*,int) switch statment (positionInFile = %d) \n",positionInFile);
               ROSE_ABORT();
               break;
        }

#if 0
     printf ("Inside of MidLevelInterfaceNodeCollection::insertStatementList() exiting ... \n");
     ROSE_ABORT();
#endif
   }

void
HighLevelInterfaceNodeCollection::
insertStatementList ( 
   SgStatement* target,
   const SgStatementPtrList & X,
   AttachedPreprocessingInfoType & commentsAndDirectives,
   IntermediateFileStringPositionEnum positionInFile )
   {
  // Supporting function for more general (simpler) insert function above.
  // traverse the list and insert the new AST fragments into the AST ast the specified position in scope

#if 0
     printf ("In HighLevelInterfaceNodeCollection::insertStatementList(): target = %p target->sage_class_name() = %s \n",
          target,target->sage_class_name());
     printf ("     positionInFile = %d positionName() = %s \n",
          positionInFile,getIntermediateFilePositionName(positionInFile).c_str());
     printf ("     SgStatementPtrList size (X.size() = %" PRIuPTR ") \n",X.size());
     printf ("target = %s \n",target->unparseToString().c_str());
#endif

#if 0
     SgStatementPtrList::const_iterator transformationStatementIterator;
     for (transformationStatementIterator = X.begin();
          transformationStatementIterator != X.end();
          transformationStatementIterator++)
        {
          string unparsedDeclarationCodeString = (*transformationStatementIterator)->unparseToString();
          printf ("In AST_Rewrite::insert(): unparsedDeclarationCodeString = %s \n",unparsedDeclarationCodeString.c_str());
        }
#endif

     ROSE_ASSERT (X.size() > 0 || commentsAndDirectives.size() > 0);

     switch (positionInFile)
        {
          case unknownIntermediatePositionInScope:
               printf ("Error: Insert() unknownPositionInScope case \n");
               ROSE_ABORT();
               break;

          case GlobalScopePreamble:
          case GlobalScopeTopOfScope:
          case GlobalScopeTopOfIncludeRegion:
          case GlobalScopeBottomOfIncludeRegion:
          case FunctionScopePreamble:
          case FunctionScopeTopOfScope:
             {
//             printf ("Error: GlobalScopeBottomOfScope case not implemented \n");
//             ROSE_ABORT();
               SgScopeStatement* scope = isSgScopeStatement(target);
               if (scope == NULL)
                  {
                 // If NULL it could just be a function in the global scope
                 // Get the parent node and check if it is a scope
                    SgStatement* statement = dynamic_cast<SgStatement*>(target);
                    ROSE_ASSERT (statement != NULL);
                 // SgStatement* statementParent = statement->get_parent();
                    SgStatement* statementParent = isSgStatement(statement->get_parent());
                    scope = isSgScopeStatement(statementParent);
                    ROSE_ASSERT (scope != NULL);
                  }

               ROSE_ASSERT (scope != NULL);
               LowLevelRewrite::insertAtTopOfScope (X,commentsAndDirectives,scope);
               break;
             }
#if 0
          case GlobalScopeBottomOfIncludeRegion:
             {
               ROSE_ASSERT (dynamic_cast<SgGlobal*>(target) == NULL);
               LowLevelRewrite::insertBeforeCurrentStatement (X,commentsAndDirectives,target);
               break;
             }
#endif

          case GlobalScopeBeforeCurrentPosition:
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertBeforeCurrentStatement (X,commentsAndDirectives,target);
               break;

          case GlobalScopeReplaceCurrentPosition:
            // See comment above
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertReplaceCurrentStatement (X,commentsAndDirectives,target);
               break;

          case FunctionScopeBeforeCurrentPosition:
            // The target should not be a scope, it should be the statement 
            // that we want to insert a new statement list in front of.  This could be
            // a SgBasicBlock in the most general case, but we introduce this test to 
            // help debug the cases where the target is not a SgBasicBlock.  This 
            // constraint will be removed later for handle the most general case.
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertBeforeCurrentStatement (X,commentsAndDirectives,target);
               break;

          case FunctionScopeReplaceCurrentPosition:
            // See comment above
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertReplaceCurrentStatement (X,commentsAndDirectives,target);
               break;

          case FunctionScopeAfterCurrentPosition:
            // See comment above
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertAfterCurrentStatement (X,commentsAndDirectives,target);
               break;

          case GlobalScopeAfterCurrentPosition:
            // See comment above
               ROSE_ASSERT (dynamic_cast<SgBasicBlock*>(target) == NULL);
               LowLevelRewrite::insertAfterCurrentStatement (X,commentsAndDirectives,target);
               break;

          case FunctionScopeBottomOfScope:
          case GlobalScopeBottomOfScope:
             {
//             printf ("Error: GlobalScopeBottomOfScope case not implemented \n");
//             ROSE_ABORT();
               SgScopeStatement* scope = isSgScopeStatement(target);
               if (scope == NULL)
                  {
                 // Get the parent node and check if it is a scope
                    SgStatement* statement = dynamic_cast<SgStatement*>(target);
                    ROSE_ASSERT (statement != NULL);
                 // SgStatement* statementParent = statement->get_parent();
                    SgStatement* statementParent = isSgStatement(statement->get_parent());
                    scope = isSgScopeStatement(statementParent);
                    ROSE_ASSERT (scope != NULL);
                  }

               ROSE_ASSERT (scope != NULL);
               LowLevelRewrite::insertAtBottomOfScope (X,commentsAndDirectives,scope);
               break;
             }

          default:
               printf ("Error: default found in AST_Rewrite::insert(SgStatementPtrList,SgNode*,int) switch statment (positionInFile = %d) \n",positionInFile);
               ROSE_ABORT();
               break;
        }

#if 0
     printf ("Inside of HighLevelInterfaceNodeCollection::insertStatementList() exiting ... \n");
     ROSE_ABORT();
#endif
   }

void
MidLevelInterfaceNodeCollection::insertStatementListWithMarkers (
   SgStatement* target,
   SgStatementPtrList statementList, 
   IntermediateFileStringPositionEnum filePosition )
   {
  // List passed by value and modified before passing onto LowLevelRewrite::insert()
  // I can't find a meaningful way to factor this function with the almost identical
  // code for the MidLevelInterfaceNodeCollection and HighLevelInterfaceNodeCollection

  // List should have at least a starting and ending marker variable declarations
     ROSE_ASSERT (statementList.size() >= 2);

  // DQ (1/11/2006): Make the statements as transformations (note not all statements 
  // marked as transformations are output, e.g. statements coming from header files).
     markStatementListWithMarkersAsTransformation(statementList);

  // Handle the comments and CPP directives that might be attached to the edged (marker declarations)
     AttachedPreprocessingInfoType commentsAndDirectives = processCommentsAndDirectives (statementList);

  // printf ("Calling insertStatementList BEFORE stripOffMarkerDeclarations (target=%s,statementList size = %d) \n",
  //      target->sage_class_name(),statementList.size());

  // All comments/directives must have been removed from the marker declarations at this point
  // comments associated with the markers are saved (above) and comments attached to any statements 
  // in the statement list are left in place (to be inserted with the statements).
     stripOffMarkerDeclarations (statementList);
#if 0
     printf ("In MidLevelInterfaceNodeCollection::insertStatementListWithMarkers: calling insertStatementList AFTER stripOffMarkerDeclarations (target=%s,statementList size = %d) \n",
          target->sage_class_name(),statementList.size());
#endif
     insertStatementList ( target, statementList, commentsAndDirectives, filePosition );

  // DQ (9/16/2005): Now update the map of qualified mangled names to IR node pointers
  // as required to keep the map up to date with the evolving primary AST.  This
  // map is required to support knowing when something in the secondary AST has been 
  // inserted into the primary AST via an include directive.
  // This support is not implemented yet!
  // printf ("Put update of map between qualified mangled names to IR nodes HERE! \n");

  // DQ (3/25/2006): exit to enforce error
  // printf ("Exiting here \n");
  // ROSE_ASSERT(false);
   }

void
HighLevelInterfaceNodeCollection::insertStatementListWithMarkers (
   SgStatement* target,
   SgStatementPtrList statementList, 
   IntermediateFileStringPositionEnum filePosition )
   {
  // List should have at least a starting and ending marker variable declarations
     ROSE_ASSERT (target != NULL);
     ROSE_ASSERT (statementList.size() >= 2);

  // printf ("In HighLevelInterfaceNodeCollection::insertStatementListWithMarkers: Before markStatementListWithMarkersAsTransformation: statementList = %" PRIuPTR " \n",statementList.size());

  // DQ (1/11/2006): Make the statements as transformations (note not all statements 
  // marked as transformations are output, e.g. statements coming from header files).
     markStatementListWithMarkersAsTransformation(statementList);

  // printf ("After markStatementListWithMarkersAsTransformation: statementList = %" PRIuPTR " \n",statementList.size());

  // Handle the comments and CPP directives that might be attached to the edged (marker declarations)
     AttachedPreprocessingInfoType commentsAndDirectives = processCommentsAndDirectives (statementList);

  // All comments/directives must have been removed from the marker declarations at this point
     stripOffMarkerDeclarations (statementList);

  // Only insert something if we have something to insert! Note that replacement with nothing is
  // not permitted, remove should be called instead.  A higher level interface might later decide to 
  // implement replace with nothing as replace with ";" which I think would be cleaner and still permit
  // the strict error checking within the lower level interface.
  // printf ("Before insertStatementList(): statementList = %" PRIuPTR " commentsAndDirectives = %" PRIuPTR " \n",statementList.size(),commentsAndDirectives.size());
     if (statementList.size() > 0 || commentsAndDirectives.size() > 0)
        {
          ROSE_ASSERT (statementList.size() > 0 || commentsAndDirectives.size() > 0);
          insertStatementList ( target, statementList, commentsAndDirectives, filePosition );
        }
#if 0
       else
        {
       // For now pass along information about the attempt to the users (for temp debugging)
          printf ("WARNING: Attempt to insert/replace statement with empty list of new statements or comments/directives at line = %d \n",rose::getLineNumber(target));
      // ROSE_ABORT();
        }
#endif
   }

string
MidLevelInterfaceNodeCollection::
wrapMarkersAroundTransformationString (
   string transformationString, 
   IntermediateFileStringPositionEnum filePosition )
   {
  // This function added the marker declarations which we we use in the traversal 
  // of the AST for the intermediate file to locate the AST fragments associated 
  // (built from) the transformation strings.
  // string markerStartDecl = "int " + markerStrings[filePosition][0] + " = 0;\n";
  // string markerEndDecl   = "int " + markerStrings[filePosition][1] + " = 0;\n";
     string markerStartDecl = "int " + markerStrings[filePosition][0] + ";\n";
     string markerEndDecl   = "int " + markerStrings[filePosition][1] + ";\n";

     string returnString = markerStartDecl + transformationString + markerEndDecl;

     return returnString;
   }

string
MidLevelInterfaceNodeCollection::
generateIntermediateFileString ( SgNode* astNode, bool prefixIncludesCurrentStatement )
   {
  // This function generates the string that we will use as the intermediate file.

     ROSE_ASSERT (astNode != NULL);

  // printf ("Inside of MidLevelInterfaceNodeCollection::generateIntermediateFileString \n");

     string prefix,suffix;
     bool generateIncludeDirectives = true;
     MidLevelRewrite<MidLevelInterfaceNodeCollection>::generatePrefixAndSuffix(
          astNode,prefix,suffix,generateIncludeDirectives,prefixIncludesCurrentStatement);

     ROSE_ASSERT ( (sortedTransformationStrings[CurrentLocationTopOfScope   ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationBefore       ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationReplace      ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationAfter        ].size() > 0) ||
                   (sortedTransformationStrings[CurrentLocationBottomOfScope].size() > 0) );

     string transformationString;
  // for (int i = CurrentLocationBefore; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
     for (int i = GlobalScopePreamble; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
        {
          transformationString += wrapMarkersAroundTransformationString(StringUtility::listToString(sortedTransformationStrings[i]),IntermediateFileStringPositionEnum(i));
        }

     string avoidCompilerWarningString = "\n/* Reference marker variables to avoid compiler warnings */ \n";
     ROSE_ASSERT (astNode->get_parent() != NULL);
     if (isSgGlobal(astNode->get_parent()) != NULL)
        {
       // append code which defines a function which uses the marker variables directly
          avoidCompilerWarningString += "void avoidCompilerWarnings() { \n";
          for (int i = GlobalScopePreamble; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
             {
               avoidCompilerWarningString += markerStrings[i][0] + "++;\n";
               avoidCompilerWarningString += markerStrings[i][1] + "++;\n";
             }
          avoidCompilerWarningString += "}\n";
        }
#if 0
       else
        {
       // append code which uses the marker variables directly
          for (int i = GlobalScopePreamble; i < LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
             {
               avoidCompilerWarningString += markerStrings[i][0] + "++;\n";
               avoidCompilerWarningString += markerStrings[i][1] + "++;\n";
             }
        }
#endif

#if 0
     printf ("#####\ntransformationString = \n %s \n#####\n",transformationString.c_str());
     printf ("#####\nprefix = \n %s \n#####\n",prefix.c_str());
     printf ("#####\navoidCompilerWarningString = \n %s \n#####\n",avoidCompilerWarningString.c_str());
     printf ("#####\nsuffix = \n %s \n#####\n",suffix.c_str());
#endif

  // printf ("Note: MidLevelInterfaceNodeCollection::generateIntermediateFileString could use Preamble, but doesn't currently \n");

  // Define markers to search for when we extract AST fragements (as a list of statements)
     string finalSourceCodeString = prefix + transformationString + avoidCompilerWarningString + suffix;

#if 0
     printf ("Error: MidLevelInterfaceNodeCollection::generateIntermediateFileString() not implemented \n");
     ROSE_ABORT();
#endif

     return finalSourceCodeString;
   }

string
HighLevelInterfaceNodeCollection::
generateIntermediateFileString ( SgNode* astNode, bool prefixIncludesCurrentStatement )
   {
  // This function takes the current synthesised attribute and assembles a file representing the
  // strings that are determined to be relevant (given the relative scope associated with the string
  // and the current position in the AST).  Strings with a target for all higher level (parent) scopes
  // are placed into the $PREAMBLE section since they are required for declaration of variables.

  // The template string used to substitute strings into is defined statically
     string intermediateSupportingFileTemplate = "\
int GlobalScopePreambleStart = 0; \n\
$GLOBAL_SCOPE_PREAMBLE \n\
int GlobalScopePreambleEnd = 0; \n\
int GlobalScopeTopOfScopeStart = 0; \n\
$GLOBAL_SCOPE_TOP_OF_SCOPE \n\
int GlobalScopeTopOfScopeEnd = 0; \n\
int GlobalScopeTopOfIncludeRegionStart = 0; \n\
$GLOBAL_SCOPE_TOP_OF_INCLUDE_REGION \n\
int GlobalScopeTopOfIncludeRegionEnd = 0; \n\
$GLOBAL_SCOPE_OPENING_DECLARATIONS \n\
int GlobalScopeBottomOfIncludeRegionStart = 0; \n\
$GLOBAL_SCOPE_BOTTOM_OF_INCLUDE_REGION \n\
int GlobalScopeBottomOfIncludeRegionEnd = 0; \n\
int GlobalScopeBeforeCurrentPositionStart = 0; \n\
$GLOBAL_SCOPE_BEFORE_CURRENT_POSITION \n\
int GlobalScopeBeforeCurrentPositionEnd = 0; \n\
int GlobalScopeReplaceCurrentPositionStart = 0; \n\
$GLOBAL_SCOPE_REPLACE_CURRENT_POSITION \n\
int GlobalScopeReplaceCurrentPositionEnd = 0; \n\
void functionName_1 ( int need_this_to_force_generation_of_synthesized_attribute_for_arguments_in_list ) \n\
     \n\
     $FUNCTION_SCOPE_OPENING_DECLARATIONS_AND_BRACES \n\
     int FunctionScopePreambleStart = 0; \n\
     $FUNCTION_SCOPE_PREAMBLE \n\
     int FunctionScopePreambleEnd = 0; \n\
     int FunctionScopeTopOfScopeStart = 0; \n\
     $FUNCTION_SCOPE_TOP_OF_SCOPE \n\
     int FunctionScopeTopOfScopeEnd = 0; \n\
     int FunctionScopeBeforeCurrentPositionStart = 0; \n\
     $FUNCTION_SCOPE_BEFORE_CURRENT_POSITION \n\
     int FunctionScopeBeforeCurrentPositionEnd = 0; \n\
     int FunctionScopeReplaceCurrentPositionStart = 0; \n\
     $FUNCTION_SCOPE_REPLACE_CURRENT_POSITION \n\
     int FunctionScopeReplaceCurrentPositionEnd = 0; \n\
     int FunctionScopeAfterCurrentPositionStart = 0; \n\
     $FUNCTION_SCOPE_AFTER_CURRENT_POSITION \n\
     int FunctionScopeAfterCurrentPositionEnd = 0; \n\
     int FunctionScopeBottomOfScopeStart = 0; \n\
     $FUNCTION_SCOPE_BOTTOM_OF_SCOPE \n\
     int FunctionScopeBottomOfScopeEnd = 0; \n\
  // Use (reference) marker variables to avoid compiler warnings \n\
     FunctionScopePreambleStart++; \n\
     FunctionScopePreambleEnd++; \n\
     FunctionScopeTopOfScopeStart++; \n\
     FunctionScopeTopOfScopeEnd++; \n\
     FunctionScopeBeforeCurrentPositionStart++; \n\
     FunctionScopeBeforeCurrentPositionEnd++; \n\
     FunctionScopeReplaceCurrentPositionStart++; \n\
     FunctionScopeReplaceCurrentPositionEnd++; \n\
     FunctionScopeAfterCurrentPositionStart++; \n\
     FunctionScopeAfterCurrentPositionEnd++; \n\
     FunctionScopeBottomOfScopeStart++; \n\
     FunctionScopeBottomOfScopeEnd++; \n\
     $FUNCTION_SCOPE_CLOSING_BRACES \n\
     \n\
int GlobalScopeAfterCurrentPositionStart; \n\
$GLOBAL_SCOPE_AFTER_CURRENT_POSITION \n\
int GlobalScopeAfterCurrentPositionEnd; \n\
int GlobalScopeBottomOfScopeStart; \n\
$GLOBAL_SCOPE_BOTTOM_OF_SCOPE \n\
int GlobalScopeBottomOfScopeEnd; \n\
void avoidCompilerWarnings() \n\
   { \n\
     GlobalScopePreambleStart++; \n\
     GlobalScopePreambleEnd++; \n\
     GlobalScopeTopOfScopeStart++; \n\
     GlobalScopeTopOfScopeEnd++; \n\
     GlobalScopeTopOfIncludeRegionStart++; \n\
     GlobalScopeTopOfIncludeRegionEnd++; \n\
     GlobalScopeBottomOfIncludeRegionStart++; \n\
     GlobalScopeBottomOfIncludeRegionEnd++; \n\
     GlobalScopeBeforeCurrentPositionStart++; \n\
     GlobalScopeBeforeCurrentPositionEnd++; \n\
     GlobalScopeReplaceCurrentPositionStart++; \n\
     GlobalScopeReplaceCurrentPositionEnd++; \n\
     GlobalScopeAfterCurrentPositionStart++; \n\
     GlobalScopeAfterCurrentPositionEnd++; \n\
     GlobalScopeBottomOfScopeStart++; \n\
     GlobalScopeBottomOfScopeEnd++; \n\
   } \n\
";

//   string stringTable[numberOfIntermediatFileSourceCodePositions] =
     string stringTable[LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG] =
        { "UnknownPositionInScope",
          "$GLOBAL_SCOPE_PREAMBLE",
          "$GLOBAL_SCOPE_TOP_OF_SCOPE",
          "$GLOBAL_SCOPE_TOP_OF_INCLUDE_REGION",
          "$GLOBAL_SCOPE_BOTTOM_OF_INCLUDE_REGION",
          "$GLOBAL_SCOPE_BEFORE_CURRENT_POSITION",
          "$GLOBAL_SCOPE_REPLACE_CURRENT_POSITION",
          "$FUNCTION_SCOPE_PREAMBLE",
          "$FUNCTION_SCOPE_TOP_OF_SCOPE",
          "$FUNCTION_SCOPE_BEFORE_CURRENT_POSITION",
          "$FUNCTION_SCOPE_REPLACE_CURRENT_POSITION",
          "$FUNCTION_SCOPE_AFTER_CURRENT_POSITION",
          "$FUNCTION_SCOPE_BOTTOM_OF_SCOPE",
          "$GLOBAL_SCOPE_AFTER_CURRENT_POSITION",
          "$GLOBAL_SCOPE_BOTTOM_OF_SCOPE"
        };

  // printf ("Inside of HighLevelInterfaceNodeCollection::generateIntermediateFileString() \n");
  // ROSE_ABORT();

  // function return value
     string returnString;

     int numberOfTransformations = numberOfSortedTransformationStrings();
     bool generateFileString = (numberOfTransformations > 0);

  // if (!emptyListArray)
     if (generateFileString == true)
        {
       // Now place the separated sets of strings that have been accumulated into the 
       // "intermediateSupportingFileTemplate" string that we will return.

       // IntermediateFileStringPositionEnum intermediateFileLocation;
       // int intermediateFileLocation;
          for (int intermediateFileLocation = (int)GlobalScopePreamble; // GlobalScopeTopOfScope;
               intermediateFileLocation < (int)LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG;
               intermediateFileLocation++)
             {
               string oldString = stringTable[intermediateFileLocation];
               string newString;
               if (sortedTransformationStrings[intermediateFileLocation].size() > 0)
                    newString = StringUtility::listToString(sortedTransformationStrings[intermediateFileLocation]);

#if 0
               printf ("oldString = %s \n",oldString.c_str());
               printf ("newString = %s \n",newString.c_str());
#endif

            // replace the old string with the new string within the string template
               ROSE_ASSERT (oldString.length() > 0);
            // ROSE_ASSERT (newString.length() > 0);
               intermediateSupportingFileTemplate =
                    StringUtility::copyEdit(intermediateSupportingFileTemplate,oldString,newString);
             }

       // Put the application's declarations with scopes at the top 
       // of the function used to define the tree framents
          string oldOpeningGlobalDeclarationString = "$GLOBAL_SCOPE_OPENING_DECLARATIONS";
          string oldOpeningLocalDeclarationString  = "$FUNCTION_SCOPE_OPENING_DECLARATIONS_AND_BRACES";
          string oldClosingString                  = "$FUNCTION_SCOPE_CLOSING_BRACES";

          string newOpeningGlobalDeclarationString,newOpeningLocalDeclarationString,newClosingString;
          bool generateIncludeDirectives = true;

          HighLevelRewrite::generatePrefixAndSuffix(
               astNode,newOpeningGlobalDeclarationString,
               newOpeningLocalDeclarationString,newClosingString,
               generateIncludeDirectives,prefixIncludesCurrentStatement);

       // DQ (8/8/2005): We at least need an opening and closing brace!
          if (newOpeningLocalDeclarationString == "")
             {
                newOpeningLocalDeclarationString = " /* default opening string */ {";
             }
          if (newClosingString == "")
             {
               newClosingString = " /* default closing string */ }";
             }
          

#if 1
          printf ("newOpeningGlobalDeclarationString (length = %" PRIuPTR ") = \n%s \n",newOpeningGlobalDeclarationString.size(),newOpeningGlobalDeclarationString.c_str());
          printf ("newOpeningLocalDeclarationString  (length = %" PRIuPTR ") = \n%s \n",newOpeningLocalDeclarationString.size(),newOpeningLocalDeclarationString.c_str());
          printf ("newClosingString                  (length = %" PRIuPTR ") = \n%s \n",newClosingString.size(),newClosingString.c_str());
       // printf ("Exiting after call to HighLevelRewrite::generatePrefixAndSuffix() \n");
       // printf ("Exiting in HighLevelRewrite::SynthesizedAttribute::generateIntermediateFileString() \n");
       // ROSE_ABORT();
#endif

#if 0
          printf ("#1 intermediateSupportingFileTemplate = \n%s\n",intermediateSupportingFileTemplate.c_str());
#endif

          intermediateSupportingFileTemplate =
               StringUtility::copyEdit(intermediateSupportingFileTemplate,
                                       oldOpeningGlobalDeclarationString,newOpeningGlobalDeclarationString);

#if 0
          printf ("#2 intermediateSupportingFileTemplate = \n%s\n",intermediateSupportingFileTemplate.c_str());
#endif

          intermediateSupportingFileTemplate =
               StringUtility::copyEdit(intermediateSupportingFileTemplate,
                                       oldOpeningLocalDeclarationString,newOpeningLocalDeclarationString);

#if 0
          printf ("#3 intermediateSupportingFileTemplate = \n%s\n",intermediateSupportingFileTemplate.c_str());
#endif

       // Put in the closing braces to finish off the scope in which the transformation string is compiled
          intermediateSupportingFileTemplate =
               StringUtility::copyEdit(intermediateSupportingFileTemplate,oldClosingString,newClosingString);

       // debugging code
       // printf ("intermediateSupportingFileTemplate = \n%s\n",intermediateSupportingFileTemplate.c_str());

#if 0
          printf ("#4 intermediateSupportingFileTemplate = \n%s\n",intermediateSupportingFileTemplate.c_str());
#endif

#if 0
          printf ("Exiting in HighLevelRewrite::SynthesizedAttribute::generateIntermediateFileString() \n");
          ROSE_ABORT();
#endif
       // copy to the return string
          returnString = intermediateSupportingFileTemplate;
          ROSE_ASSERT (returnString.length() > 0);
        }
       else
        {
       // If there are no strings that apply within this scope (or around this statement) then
       // return an empty string.
       // printf ("Warning: No AST rewrite required for this scope = %s \n",astNode->sage_class_name());
       // ROSE_ABORT();
        }

#if 0
     printf ("returnString = \n%s\n",returnString.c_str());
     printf ("In HighLevelRewrite::SynthesizedAttribute::generateIntermediateFileString(): Exiting BEFORE return \n");
     ROSE_ABORT();
#endif

     return returnString;
   }

#if 0
// DQ (7/25/2005): Commented out for replacement by Milind's work to fixup pointers 
// in the new AST fragement (secondary AST) to point to only the primary AST.
void
MidLevelInterfaceNodeCollection::writeAllChangeRequests ( SgStatement* target, bool prefixIncludesCurrentStatement )
   {
  // This function forces the modification of the AST for any transformation 
  // requests specified in the request handle (stringAndNodeCollection).

  // printf ("Inside of MidLevelInterfaceNodeCollection::writeAllChangeRequests() \n");

     ROSE_ASSERT(target != NULL);

  // convertTransformationStringsToASTStatementCollection(stringAndNodeCollection);
  // inputStrings --> sorted strings
     sortInputStrings(target);

     display("In MidLevelInterfaceNodeCollection::writeAllChangeRequests:insert(): After sortInputStrings \n");

  // sorted strings --> sorted AST Statement lists
     ROSE_ASSERT (target != NULL);

  // printf ("In writeAllChangeRequests(): Calling compileSortedStringsToGenerateNodeCollection \n");
     compileSortedStringsToGenerateNodeCollection(target,prefixIncludesCurrentStatement);

  // display("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): After compileSortedStringsToGenerateNodeCollection \n");

  // printf ("Exiting before insertContainerOfListsOfStatements(target = %s) \n",target->sage_class_name());
  // ROSE_ASSERT(false);

     insertContainerOfListsOfStatements(target);

     SgScopeStatement* scopeOfTarget = target->get_scope();
     ROSE_ASSERT(scopeOfTarget != NULL);

     AstPostProcessing(scopeOfTarget);
   }
#endif

#if 0
// DQ (7/25/2005): Commented out for replacement by Milind's work to fixup pointers 
// in the new AST fragement (secondary AST) to point to only the primary AST.
void
HighLevelInterfaceNodeCollection::writeAllChangeRequests ( SgStatement* target, bool prefixIncludesCurrentStatement )
   {
  // This function forces the modification of the AST for any transformation 
  // requests specified in the request handle (stringAndNodeCollection).

  // printf ("Inside of HighLevelInterfaceNodeCollection::writeAllChangeRequests() \n");

     ROSE_ASSERT(target != NULL);

  // convertTransformationStringsToASTStatementCollection(stringAndNodeCollection);
  // inputStrings --> sorted strings
     sortInputStrings(target);

  // display("In HighLevelInterfaceNodeCollection::writeAllChangeRequests::insert(): After sortInputStrings \n");

  // sorted strings --> sorted AST Statement lists
     ROSE_ASSERT (target != NULL);
     compileSortedStringsToGenerateNodeCollection(target,prefixIncludesCurrentStatement);

  // display("In HighLevelInterfaceNodeCollection::writeAllChangeRequests(): After compileSortedStringsToGenerateNodeCollection \n");

     insertContainerOfListsOfStatements(target);

  // DQ (7/19/2005): Call fixup on the target's scope to make sure that everything is setup
     SgScopeStatement* scopeOfTarget = target->get_scope();
     ROSE_ASSERT(scopeOfTarget != NULL);

     AstPostProcessing(scopeOfTarget);
   }
#endif


void
MidLevelInterfaceNodeCollection::writeAllChangeRequests ( SgStatement* target, bool prefixIncludesCurrentStatement )
   {
  // This function forces the modification of the AST for any transformation 
  // requests specified in the request handle (stringAndNodeCollection).

  // printf ("Inside of MidLevelInterfaceNodeCollection::writeAllChangeRequests() \n");

  // convertTransformationStringsToASTStatementCollection(stringAndNodeCollection);
  // inputStrings --> sorted strings

     sortInputStrings(target);

  // display("In MidLevelInterfaceNodeCollection::writeAllChangeRequests:insert(): After sortInputStrings \n");

  // sorted strings --> sorted AST Statement lists
     ROSE_ASSERT (target != NULL);

  // printf ("In writeAllChangeRequests(): Calling compileSortedStringsToGenerateNodeCollection \n");
     compileSortedStringsToGenerateNodeCollection(target,prefixIncludesCurrentStatement);

  // display("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): After compileSortedStringsToGenerateNodeCollection \n");

  // printf ("Exiting before insertContainerOfListsOfStatements(target = %s) \n",target->sage_class_name());
  // ROSE_ASSERT(false);

  // This is where we do the merging
  // First, find the project node for target
     SgNode * curr = target;
  // printf("Target: %x (%s)\n", target, target->sage_class_name());
     while (curr->variantT() != V_SgProject)
        {
          curr = curr->get_parent();
          ROSE_ASSERT(curr != NULL);
        }
     SgProject * trunk = isSgProject(curr);
     ROSE_ASSERT(trunk != NULL);

  // printf ("Calling AstTests::runAllTests BEFORE any insertion of new statements by rewrite mechanism \n");

  // DQ (3/25/2006): Commented out these tests since they fail for testReplacementStatements
     AstTests::runAllTests(trunk);

  // Now find the project node for the other statements.
     ROSE_ASSERT(treeFragement.size() != 0);
  // All of the statements in treeFragment should be in the same AST. Thus, we only need to find the project for one
  // get the first SgStatement
  // We also determine which statements are the ones that will be inserted
     SgStatement * newStatement = NULL;
     vector<SgStatement *> toSave;
     for (vector<SgStatementPtrList>::iterator i = treeFragement.begin(); i != treeFragement.end(); i++)
        {
          for (SgStatementPtrList::iterator j = i->begin(); j != i->end(); j++)
             {
               bool toInclude = false;
               SgStatement * stmt = *j;
               if (!isSgVariableDeclaration(stmt))
                  {
                 // since only variable declarations are markers
                    toInclude = true;
                  }
                 else
                  {
                 // if the name of the variable is in the list of "marker" variables, we don't want to include it
                    toInclude = true;
                    ROSE_ASSERT(isSgVariableDeclaration(stmt)->get_variables().size() > 0);
                    SgInitializedName * varname = *(isSgVariableDeclaration(stmt)->get_variables().begin());
                    string name = varname->get_name().getString();
                    for (int k = 0; k < MidLevelCollectionTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; k++)
                       {
                         for (int p = 0; p < 2; p++)
                            {
                              string comp = markerStrings[k][p];
                              if (name == comp)
                                 {
                                   toInclude = false;
                                 }
                            }
                       }
                  }
               if (toInclude)
                  {
                    if (newStatement == NULL)
                       {
                         newStatement = stmt;
                       }
                    toSave.push_back(stmt);
                  }
             }
        }

     if (newStatement != NULL)
        {
       // DQ (9/15/2005): Commented out as a test!
       // ROSE_ASSERT(toSave.size() != 0);
          ROSE_ASSERT(newStatement != NULL);
          SgNode *curr = newStatement;
       // while (curr->variantT() != V_SgFile)
          while (curr->variantT() != V_SgSourceFile)
             {
               SgNode * tmp = curr->get_parent();
               ROSE_ASSERT(tmp != NULL);
               curr = tmp;
             }
          SgFile * branch = isSgFile(curr);
          ROSE_ASSERT(branch != NULL);

       // set up the merge

          vector<SgStatement *> toSaveParents;
          for (vector<SgStatement *>::iterator i = toSave.begin(); i != toSave.end(); i++)
             {
               toSaveParents.push_back(isSgStatement((*i)->get_parent()));
             }

          insertContainerOfListsOfStatements(target);

// DQ (5/26/2007): Commented out older AST merge
#if 0
       // DQ (3/25/2006): The AST Merging seems to be fragile at least when 
       // used in this context so skip it for now until we can work on it more.

       // SgScopeStatement * scopeOfTarget = target->get_scope();
       // ROSE_ASSERT(target != NULL);
       // AstPostProcessing(scopeOfTarget);

          ParentAttribute tmpatt(NULL);
          AstFixParentTraversal afpt;
          afpt.traverse(trunk, tmpatt);
     
          AstCheckParentTraversal acpt;
          acpt.traverse(trunk, tmpatt);

       // DQ (9/15/2005): The problem with this code is that where a new declaration put into 
       // the primary AST and there is a reference to the newly added variable then we save 
       // the reference to the variable but not the variable declaration itself (causing errors 
       // down stream; references to deleted variables for example).
       // So we have to detect from the IR nodes in treeFragement which additional IR nodes 
       // should be added.  The best way to do this is to detect when the declaration was added.
       // If it was added via an include directive then when the include directive is added the 
       // declarations should be transplanted to the primary AST.  This makes clear that just adding
       // include directives as comments is a poor solution, since none of the include files 
       // declarations are added to the primary AST and so any references to such declarations 
       // will not have valid declaration to reference (symbol tables also need to be updated).

       // DQ (3/25/2006): This is now fixed
       // printf ("Warning: declarations added via any include directives are not present in primary AST \n");

          AstMerge am;
          am.addAST(trunk);
          trunk = am.getAST();
     
       // Now removing secondary AST
          for (unsigned int i = 0; i < toSave.size(); i++)
             {
            // printf("In Merge support saving %p (%s) with parent %p (%s)\n",toSave[i],toSave[i]->sage_class_name(),toSaveParents[i],toSaveParents[i]->sage_class_name());

            // Some ugliness here because sometimes our target is actually
            // the child of the statement in the list, so we have to remove
            // it in a certain way.
               if (toSave[i]->get_parent() == toSaveParents[i])
                  {
                    SgStatement * parentNode = toSave[i];
                    switch (parentNode->variantT())
                       {
                         case V_SgFunctionDefinition:
                            {
                              SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(parentNode);
                              ROSE_ASSERT (functionDefinition != NULL);
                              SgBasicBlock* newBasicBlock = new SgBasicBlock();
                              functionDefinition->set_body(newBasicBlock);
                              break;
                            }
                         case V_SgFunctionDeclaration:
                         case V_SgMemberFunctionDeclaration:
                         case V_SgTemplateInstantiationFunctionDecl:
                         case V_SgTemplateInstantiationMemberFunctionDecl:
                            {
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentNode);
                              ROSE_ASSERT (functionDeclaration != NULL);
                              SgFunctionDefinition* newFunctionDefinition = 
                                   new SgFunctionDefinition(functionDeclaration->get_file_info(),functionDeclaration);
                              functionDeclaration->set_definition(newFunctionDefinition);
                              break;
                            }
                      // case V_SgFile:
                         case V_SgSourceFile:
                            {
                              SgSourceFile* file = isSgSourceFile(parentNode);
                              ROSE_ASSERT (file != NULL);
                              SgGlobal* newGlobalScope = new SgGlobal();
                              file->set_root(newGlobalScope);
                              break;
                            }

                      // DQ (1/11/2006): Added extra case
                      // Note: SgTemplateInstantiationDecl should be handled as separate case
                      // so that we can generate a SgTemplatateInstantiationDefn object.
                         case V_SgClassDeclaration:
                            {
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(parentNode);
                              ROSE_ASSERT (classDeclaration != NULL);
                              SgClassDefinition* newClassDefinition = 
                                   new SgClassDefinition(classDeclaration->get_file_info(),classDeclaration);
                              classDeclaration->set_definition(newClassDefinition);
                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgTypedefDeclaration:
                            {
                              SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(parentNode);
                              ROSE_ASSERT (typedefDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgTypedefDeclaration Skipping delete, not clear on what to do here! \n");
                           // ROSE_ASSERT (typedefDeclaration->get_declaration() != NULL);
                              if (typedefDeclaration->get_declaration() != NULL)
                                   printf ("typedefDeclaration->get_declaration() = %s \n",typedefDeclaration->get_declaration()->class_name().c_str());

                           // SgTypedefDefinition* newClassDefinition = new SgClassDefinition(typedefDeclaration->get_file_info(),classDeclaration);
                           // classDeclaration->set_definition(newClassDefinition);
                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgEnumDeclaration:
                            {
                              SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(parentNode);
                              ROSE_ASSERT (enumDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgEnumDeclaration Skipping delete, not clear on what to do here! \n");

                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgPragmaDeclaration:
                            {
                              SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(parentNode);
                              ROSE_ASSERT (pragmaDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgPragmaDeclaration Skipping delete, not clear on what to do here! \n");

                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgNamespaceDeclarationStatement:
                            {
                              SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(parentNode);
                              ROSE_ASSERT (namespaceDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgNamespaceDeclarationStatement Skipping delete, not clear on what to do here! \n");

                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgVariableDeclaration:
                            {
                              SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(parentNode);
                              ROSE_ASSERT (variableDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgVariableDeclaration Skipping delete, not clear on what to do here! \n");

                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgTemplateDeclaration:
                            {
                              SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(parentNode);
                              ROSE_ASSERT (templateDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgTemplateDeclaration Skipping delete, not clear on what to do here! \n");

                              break;
                            }
                      // DQ (1/11/2006): Added extra case
                         case V_SgTemplateInstantiationDecl:
                            {
                              SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(parentNode);
                              ROSE_ASSERT (templateInstantiationDeclaration != NULL);
                              SgTemplateInstantiationDefn* newTemplateInstantiationDefinition = 
                                   new SgTemplateInstantiationDefn(templateInstantiationDeclaration->get_file_info(),templateInstantiationDeclaration);
                              templateInstantiationDeclaration->set_definition(newTemplateInstantiationDefinition);
                              break;
                            }

                      // DQ (1/11/2006): Added extra case
                         case V_SgUsingDeclarationStatement:
                            {
                              SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(parentNode);
                              ROSE_ASSERT (usingDeclaration != NULL);

                           // DQ (1/11/2006): Not clear what do do here!
                              printf ("In MidLevelInterfaceNodeCollection::writeAllChangeRequests(): SgUsingDeclarationStatement Skipping delete, not clear on what to do here! \n");

                              break;
                            }

                         default:
                              printf ("Error, default reached in switch parentNode = %s \n",parentNode->sage_class_name());
                              ROSE_ASSERT(false);
                       }
                  }
                 else
                  {
                    SgStatement * parent = toSaveParents[i];
                    parent->remove_statement(toSave[i]);
                  }

           // printf("DONE: Saving (dead code) %p (%s) with parent %p (%s)\n",toSave[i],toSave[i]->sage_class_name(),toSaveParents[i],toSaveParents[i]->sage_class_name());
             }

          AstDeleteSubtree ads;

       // printf ("Deleting the secondary AST \n");

          ROSE_ASSERT(branch != NULL);
          ads.traverse(branch);

       // printf ("DONE: Deleting the secondary AST \n");
       // ROSE_ASSERT(false);

       // DQ (3/25/2006): Skipping the AST Merge
#else
          printf ("Skipping AST merge in MidLevelInterfaceNodeCollection::writeAllChangeRequests() \n");
#endif
        }
       else
        {
       // Even if there is no AST to inser, there might be comments or CPP directives
          insertContainerOfListsOfStatements(target);
        }
#if 0
  // DQ (3/25/2006): This fails in the tutorial example tauInstrumenter.C
  // But it does work correctly for all the other tests cses in the tests directory
     printf ("Calling AstTests::runAllTests AFTER any insertion of new statements by rewrite mechanism \n");
     AstTests::runAllTests(const_cast<SgProject *>(trunk));
#else
     printf ("Skipping AstTests::runAllTests AFTER any insertion of new statements by rewrite mechanism \n");
#endif
   }

void
HighLevelInterfaceNodeCollection::writeAllChangeRequests ( SgStatement* target, bool prefixIncludesCurrentStatement )
   {
  // This function forces the modification of the AST for any transformation 
  // requests specified in the request handle (stringAndNodeCollection).

  // printf ("Inside of HighLevelInterfaceNodeCollection::writeAllChangeRequests() \n");

  // convertTransformationStringsToASTStatementCollection(stringAndNodeCollection);
  // inputStrings --> sorted strings
     sortInputStrings(target);

  // display("In HighLevelInterfaceNodeCollection::writeAllChangeRequests::insert(): After sortInputStrings \n");

  // sorted strings --> sorted AST Statement lists
     ROSE_ASSERT (target != NULL);
     compileSortedStringsToGenerateNodeCollection(target,prefixIncludesCurrentStatement);

  // display("In HighLevelInterfaceNodeCollection::writeAllChangeRequests(): After compileSortedStringsToGenerateNodeCollection \n");

  // This is where we do the merging
  // First, find the project node for target
     SgNode * curr = target;
  // printf("Target: %x (%s)\n", target, target->sage_class_name());
     while (curr->variantT() != V_SgProject)
        {
          curr = curr->get_parent();
          ROSE_ASSERT(curr != NULL);
        }
     SgProject * trunk = isSgProject(curr);
     ROSE_ASSERT(trunk != NULL);
     
  // Now find the project node for the other statements.
     ROSE_ASSERT(treeFragement.size() != 0);
  // All of the statements in treeFragment should be in the same AST. Thus, we only need to find the project for one
  // get the first SgStatement
  // We also determine which statements are the ones that will be inserted
     SgStatement * newStatement = NULL;
     vector<SgStatement *> toSave;
     for (vector<SgStatementPtrList>::iterator i = treeFragement.begin(); i != treeFragement.end(); i++)
        {
          for (SgStatementPtrList::iterator j = i->begin(); j != i->end(); j++)
             {
               bool toInclude = false;
               SgStatement * stmt = *j;
            // printf("writeAllChangeRequests statement: %s (%p)\n", stmt->sage_class_name(), stmt);
               if (!isSgVariableDeclaration(stmt))
                  {
                 // since only variable declarations are markers
                    toInclude = true;
                  }
                 else
                  {
                 // if the name of the variable is in the list of "marker" variables, we don't want to include it
                    toInclude = true;
                    ROSE_ASSERT(isSgVariableDeclaration(stmt)->get_variables().size() > 0);
                    SgInitializedName * varname = *(isSgVariableDeclaration(stmt)->get_variables().begin());
                    string name = varname->get_name().getString();
                    for (int k = 0; k < HighLevelCollectionTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; k++)
                       {
                         for (int p = 0; p < 2; p++)
                            {
                              string comp = markerStrings[k][p];
                              if (name == comp)
                                 {
                                   toInclude = false;
                                 }
                            }
                       }
                    if (toInclude == true)
                       {
                      // printf("In HighLevelInterfaceNodeCollection::writeAllChangeRequests Saving InitializedName %s (%p)\n", name.c_str(), varname);
                       }
                  }

               if (toInclude == true)
                  {
                    if (newStatement == NULL)
                       {
                         newStatement = stmt;
                       }
                    toSave.push_back(stmt);
                 // printf("In HighLevelInterfaceNodeCollection::writeAllChangeRequests Saving: %s (%p) with parent %s (%p)\n",
                 //      stmt->sage_class_name(),stmt,stmt->get_parent()->sage_class_name(),stmt->get_parent());
                  }
             }
        }

  // DQ (1/10/2006): OK for there to be an empty list (e.g. insert comment).
     if (newStatement != NULL)
        {
          ROSE_ASSERT(toSave.size() != 0);
          ROSE_ASSERT(newStatement != NULL);
          curr = newStatement;
       // while (curr->variantT() != V_SgFile)
          while (curr->variantT() != V_SgSourceFile)
             {
               SgNode * tmp = curr->get_parent();
               ROSE_ASSERT(tmp != NULL);
               curr = tmp;
             }
          SgFile * branch = isSgFile(curr);
          ROSE_ASSERT(branch != NULL);

       // set up the merge

          vector<SgStatement *> toSaveParents;
          for (vector<SgStatement *>::iterator i = toSave.begin(); i != toSave.end(); i++)
             {
               toSaveParents.push_back(isSgStatement((*i)->get_parent()));
             }

#if 0
          SgStatement * stmt = toSave[0];
          printf("Statement: %p\n", stmt);
          if (isSgVariableDeclaration(stmt))
             {
               printf("%s: %s\n", stmt->sage_class_name(), MergeUtils::generateVariableName(isSgVariableDeclaration(stmt)).c_str());
             }
            else
             {
              printf("%s: %s\n", stmt->sage_class_name(),MergeUtils::generateUniqueIdentifier(stmt).c_str());
             }
#endif
       // TEMP CODE
       // printf("\n\nRunning through AST before merging\n\n");
       // AstMerge am_temp;
       // am_temp.addAST(trunk);
       // trunk = am_temp.getAST();
       // printf("\n\nDone with preliminary check\n\n");

          insertContainerOfListsOfStatements(target);

// DQ (5/26/2007): Commented out older AST merge
#if 0
       // DQ (3/25/2006): The AST Merging seems to be fragile at least when 
       // used in this context so skip it for now until we can work on it more.

       // SgScopeStatement * scopeOfTarget = target->get_scope();
       // ROSE_ASSERT(target != NULL);
       // AstPostProcessing(scopeOfTarget);

          ParentAttribute tmpatt(NULL);
          AstFixParentTraversal afpt;
          afpt.traverse(trunk, tmpatt);
     
          AstCheckParentTraversal acpt;
          acpt.traverse(trunk, tmpatt);

          printf("\nMerging\n");

          AstMerge am;
          am.addAST(trunk);
          trunk = am.getAST();

          printf("\nDone Merging\n");
     
       // Now removing secondary AST
          for (unsigned int i = 0; i < toSave.size(); i++)
             {
            // printf("Saving %x (%s) with parent %x (%s)\n",toSave[i],toSave[i]->sage_class_name(),toSaveParents[i],toSaveParents[i]->sage_class_name());

            // Some ugliness here because sometimes our target is actually
            // the child of the statement in the list, so we have to remove
            // it in a certain way.
               if (toSave[i]->get_parent() == toSaveParents[i])
                  {
                    SgStatement * parentNode = toSave[i];
                    switch (parentNode->variantT())
                       {
                         case V_SgFunctionDefinition:
                            {
                              SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(parentNode);
                              ROSE_ASSERT (functionDefinition != NULL);
                              SgBasicBlock* newBasicBlock = new SgBasicBlock();
                              functionDefinition->set_body(newBasicBlock);
                              break;
                            }
                         case V_SgFunctionDeclaration:
                         case V_SgMemberFunctionDeclaration:
                         case V_SgTemplateInstantiationFunctionDecl:
                         case V_SgTemplateInstantiationMemberFunctionDecl:
                            {
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentNode);
                              ROSE_ASSERT (functionDeclaration != NULL);
                              SgFunctionDefinition* newFunctionDefinition = new SgFunctionDefinition(functionDeclaration->get_file_info(),functionDeclaration);
                              functionDeclaration->set_definition(newFunctionDefinition);
                              break;
                            }
                      // case V_SgFile:
                         case V_SgSourceFile:
                            {
                              SgSourceFile* file = isSgSourceFile(parentNode);
                              ROSE_ASSERT (file != NULL);
                              SgGlobal* newGlobalScope = new SgGlobal();
                              file->set_root(newGlobalScope);
                              break;
                            }
                         case V_SgVariableDeclaration:
                            {
                              SgVariableDeclaration* variable = isSgVariableDeclaration(parentNode);
                              ROSE_ASSERT (variable != NULL);
                              printf ("Unclear how to remove this IR node in removal of secondary AST \n");
                              break;
                            }
                         default:
                              printf ("Error, default reached in switch parentNode = %s \n",parentNode->sage_class_name());
                              ROSE_ASSERT(false);
                        }
                  }
                 else
                  {
                     SgStatement * parent = toSaveParents[i];
                     parent->remove_statement(toSave[i]);
                  }
             }

          AstDeleteSubtree ads;
          ads.traverse(branch);

       // DQ (3/25/2006): Skipping the AST Merge
#else
          printf ("Skipping AST merge in HighLevelInterfaceNodeCollection::writeAllChangeRequests() \n");
#endif
        }

     AstTests::runAllTests(const_cast<SgProject *>(trunk));
   }













