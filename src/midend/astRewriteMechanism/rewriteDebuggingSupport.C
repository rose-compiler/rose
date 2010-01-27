// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "nameQuery.h"
#include "rewrite.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

string
MidLevelCollectionTypedefs::getRelativeScopeString( ScopeIdentifierEnum inputRelativeScope )
   {
  // This function is useful for debugging
     string returnString;
     switch (inputRelativeScope)
        {
          case unknownScope:     returnString = "unknownScope";     break;
          case SurroundingScope: returnString = "SurroundingScope"; break;
          case StatementScope:   returnString = "StatementScope";   break;
          case Preamble:         returnString = "Preamble";         break;
          default:
             returnString = "default case";
             printf ("default case found in MidLevelCollectionTypedefs::getRelativeScopeString() switch statement \n");
             ROSE_ASSERT (1 == 2);
             break;
        }

     return returnString;
   }


string
MidLevelCollectionTypedefs::getRelativeLocationString( PlacementPositionEnum inputRelativeLocation )
   {
  // This function is useful for debugging

     string returnString;
     switch (inputRelativeLocation)
        {
          case unknownPositionInScope:
             returnString = "unknownPositionInScope";  break;
          case PreamblePositionInScope:
             returnString = "PreamblePositionInScope"; break;
          case TopOfCurrentScope:
             returnString = "TopOfCurrentScope";       break;
          case BeforeCurrentPosition:
             returnString = "BeforeCurrentPosition";   break;
          case ReplaceCurrentPosition:
             returnString = "ReplaceCurrentPosition";  break;
          case AfterCurrentPosition:
             returnString = "AfterCurrentPosition";    break;
          case BottomOfCurrentScope:
             returnString = "BottomOfCurrentScope";    break;
          default:
             returnString = "default case";
             printf ("default case found in MidLevelCollectionTypedefs::getRelativeLocationString() switch statement \n");
             ROSE_ASSERT(1==2);
             break;
        }

     return returnString;
   }

string
MidLevelCollectionTypedefs::
getIntermediateFilePositionName ( const IntermediateFileStringPositionEnum location )
   {
  // This function is useful for debugging
     string returnString;
     switch (location)
        {
          case unknownIntermediatePositionInScope: returnString = "unknownPositionInScope"; break;
          case GlobalScopePreamble:                returnString = "GlobalScopePreamble";    break;
          case CurrentLocationTopOfScope:          returnString = "CurrentLocationTopOfScope";  break;
          case CurrentLocationBefore:              returnString = "CurrentLocationBefore";  break;
          case CurrentLocationReplace:             returnString = "CurrentLocationReplace"; break;
          case CurrentLocationAfter:               returnString = "CurrentLocationAfter";   break;
          case CurrentLocationBottomOfScope:       returnString = "CurrentLocationBottomOfScope";  break;
          default:
               printf ("Error: default reached in switch (MidLevelInterfaceNodeCollection::getIntermediateFilePositionName()) \n");
               ROSE_ABORT();
               break;
        }

     return returnString;
   }


string
MidLevelCollectionTypedefs::stringAttribute ( ScopeIdentifierEnum scope,
                                    PlacementPositionEnum positionInScope )
   {
  // This function is useful for debugging
     string returnString;
     string relativeScopeString    = getRelativeScopeString(scope);
     string relativeLocationString = getRelativeLocationString(positionInScope);

     returnString = relativeScopeString + " : " + relativeLocationString + " : "; // + sourceCodeClassificationString;
     return returnString;
   }


string
HighLevelCollectionTypedefs::getRelativeScopeString( ScopeIdentifierEnum inputRelativeScope )
   {
  // This function is useful for debugging
     string returnString;
     switch (inputRelativeScope)
        {
          case unknownScope:     returnString = "unknownScope";     break;
          case SurroundingScope: returnString = "SurroundingScope"; break;
          case ParentScope:      returnString = "ParentScope";      break;
          case NestedLoopScope:  returnString = "NestedLoopScope";  break;
          case NestedConditionalScope:
             returnString = "NestedConditionalScope";
             break;
          case FunctionScope:    returnString = "FunctionScope";    break;
          case FileScope:        returnString = "FileScope";        break;
          case GlobalScope:      returnString = "GlobalScope";      break;
          case Preamble:         returnString = "Preamble";         break;
          default:
             returnString = "default case";
             printf ("default case found in HighLevelCollectionTypedefs::getRelativeScopeString() switch statement \n");
             ROSE_ABORT();
             break;
        }

     return returnString;
   }


string
HighLevelCollectionTypedefs::getRelativeLocationString( PlacementPositionEnum inputRelativeLocation )
   {
  // This function is useful for debugging
     string returnString;
     switch (inputRelativeLocation)
        {
          case unknownPositionInScope:
             returnString = "unknownPositionInScope";  break;
          case PreamblePositionInScope:
             returnString = "PreamblePositionInScope"; break;
          case TopOfScope:
             returnString = "TopOfScope";              break;
          case TopOfIncludeRegion:
             returnString = "TopOfIncludeRegion";      break;
          case BottomOfIncludeRegion:
             returnString = "BottomOfIncludeRegion";   break;
          case BeforeCurrentPosition:
             returnString = "BeforeCurrentPosition";   break;
          case ReplaceCurrentPosition:
             returnString = "ReplaceCurrentPosition";  break;
          case AfterCurrentPosition:
             returnString = "AfterCurrentPosition";    break;
          case BottomOfScope:
             returnString = "BottomOfScope";           break;
          default:
             returnString = "default case";
             printf ("default case found in HighLevelCollectionTypedefs::getRelativeLocationString() switch statement \n");
             ROSE_ABORT();
             break;
        }

     return returnString;
   }

string
HighLevelCollectionTypedefs::
getIntermediateFilePositionName ( const IntermediateFileStringPositionEnum location )
   {
  // This function is useful for debugging
     string returnString;
     switch (location)
        {
          case unknownIntermediatePositionInScope:  returnString = "unknownPositionInScope";              break;
          case GlobalScopePreamble:                 returnString = "GlobalScopePreamble";                 break;
          case GlobalScopeTopOfScope:               returnString = "GlobalScopeTopOfScope";               break;
          case GlobalScopeTopOfIncludeRegion:       returnString = "GlobalScopeTopOfIncludeRegion";       break;
          case GlobalScopeBottomOfIncludeRegion:    returnString = "GlobalScopeBottomOfIncludeRegion";    break;
          case GlobalScopeBeforeCurrentPosition:    returnString = "GlobalScopeBeforeCurrentPosition";    break;
          case GlobalScopeReplaceCurrentPosition:   returnString = "GlobalScopeReplaceCurrentPosition";   break;
          case FunctionScopePreamble:               returnString = "FunctionScopePreamble";               break;
          case FunctionScopeTopOfScope:             returnString = "FunctionScopeTopOfScope";             break;
          case FunctionScopeBeforeCurrentPosition:  returnString = "FunctionScopeBeforeCurrentPosition";  break;
          case FunctionScopeReplaceCurrentPosition: returnString = "FunctionScopeReplaceCurrentPosition"; break;
          case FunctionScopeAfterCurrentPosition:   returnString = "FunctionScopeAfterCurrentPosition";   break;
          case FunctionScopeBottomOfScope:          returnString = "FunctionScopeBottomOfScope";          break;
          case GlobalScopeAfterCurrentPosition:     returnString = "GlobalScopeAfterCurrentPosition";     break;
          case GlobalScopeBottomOfScope:            returnString = "GlobalScopeBottomOfScope";            break;

          default:
               printf ("Error: default reached in switch (HighLevelInterfaceNodeCollection::getIntermediateFilePositionName()) \n");
               printf ("IntermediateFileStringPositionEnum = %d \n",int(location));
               ROSE_ABORT();
               break;
        }

     return returnString;
   }

string
HighLevelCollectionTypedefs::stringAttribute ( ScopeIdentifierEnum scope,
                                    PlacementPositionEnum positionInScope )
   {
  // This function is useful for debugging
     string returnString;
     string relativeScopeString    = getRelativeScopeString(scope);
     string relativeLocationString = getRelativeLocationString(positionInScope);

     returnString = relativeScopeString + " : " + relativeLocationString + " : "; // + sourceCodeClassificationString;
     return returnString;
   }







