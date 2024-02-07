
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rewrite.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// ************************************************************
//                 STATIC DATA INITIALIZATION
// ************************************************************

const int HighLevelInterfaceNodeCollection::numberOfScopes                             =
     int(HighLevelInterfaceNodeCollection::LAST_SCOPE_TAG);
const int HighLevelInterfaceNodeCollection::numberOfPositionsInScope                   =
     int(HighLevelInterfaceNodeCollection::LAST_PLACEMENT_TAG);
const int HighLevelInterfaceNodeCollection::numberOfIntermediatFileSourceCodePositions =
     int(HighLevelInterfaceNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG);

// ************************************************************
//               SYNTHESIZED ATTRIBUTE BASE CLASS
// ************************************************************

// ##########################################
//        Constructors and Operators
// ##########################################

#if 1
HighLevelRewrite::SynthesizedAttribute::
SynthesizedAttribute ()
   {
  // This constructor is required by the tree traversal mechanism (it is not used anywhere else!)

  // Specify that the transformation strings must preserve order (to avoid being sorted). All other
  // string lists are to be sorted so that redundent strings (declarations and initializations) can
  // be removed.
     preserveStringOrder = true;
  // workspace           = "";
  // skipConcatination   = false;
//   associated_AST_Node = NULL;
   }
#endif

HighLevelRewrite::SynthesizedAttribute::
SynthesizedAttribute ( SgNode* /*inputASTNode*/ )
   {
  // Initialize the single pointer in the class
  // astNode  = inputASTNode;

  // Specify that the transformation strings must preserve order (to avoid being sorted). All other
  // string lists are to be sorted so that redundent strings (declarations and initializations) can
  // be removed.
     preserveStringOrder = true;
  // workspace           = "";
  // skipConcatination   = false;

//   associated_AST_Node = inputASTNode;
  // ROSE_ASSERT (associated_AST_Node != NULL);
   }

HighLevelRewrite::SynthesizedAttribute::
~SynthesizedAttribute ()
   {
     preserveStringOrder = false;
  // Reset the pointer to NULL (don't delete it since this would corrupt the AST
//   associated_AST_Node = NULL;
   }

HighLevelRewrite::SynthesizedAttribute::
SynthesizedAttribute ( const SynthesizedAttribute & X )
   {
  // Deep copy semantics
     ((SynthesizedAttribute*) this)->operator= (X);
   }

HighLevelRewrite::SynthesizedAttribute &
HighLevelRewrite::SynthesizedAttribute::
operator= ( const SynthesizedAttribute & X )
   {
     preserveStringOrder        = X.preserveStringOrder;
  // astFragmentStringContainer = X.astFragmentStringContainer;
  // associated_AST_Node        = X.associated_AST_Node;

  // Call the derived class's operator= operator
     HighLevelInterfaceNodeCollection::operator=(X);

     return *this;
   }

HighLevelRewrite::SynthesizedAttribute &
HighLevelRewrite::SynthesizedAttribute::
operator+= ( const SynthesizedAttribute & X )
   {
  // This function is called by the derived classes to simplify how synthesized attributes are
  // combined in the assembly process as we traverse the AST (returning from the leaves (of the AST)
  // to the root of the AST. NOTE: the workspace is not manipulated by this function because it's
  // behavior is dictated by the user's use of it within the users evaluateRewriteSynthesizedAttribute()
  // function.

  // for now we want to enforce this
     ROSE_ASSERT (preserveStringOrder == true);

#if 0
     printf ("New strings to add in HighLevelRewrite::SynthesizedAttribute::operator+= \n");
     displayStringList("this");
     X.displayStringList("X");
#endif

  // Remove any possible redundent strings (which would be caught as an error in the
  // consistancyCheck() member function.  Remove the redundnet entries from the input 
  // list since code strings in the current list might already depend upon that code.
//   AST_FragmentStringContainer & thisList    = astFragmentStringContainer;
//   const AST_FragmentStringContainer & XList = X.astFragmentStringContainer;
     InputTransformationStringCollectionType & thisList = inputTransformationStrings;
     const InputTransformationStringCollectionType & XList = X.inputTransformationStrings;

#if 0
     for (list<string>::iterator i = thisList.begin(); i != thisList.end(); i++)
        {
           XList.remove(*i);
        }
#endif

     unsigned int sizeOfThisList = thisList.size();
     unsigned int sizeOfXList    = XList.size();

     InputTransformationStringCollectionType nonMatchingFragmentString;
     for (InputTransformationStringCollectionType::const_iterator i = XList.begin(); i != XList.end(); i++)
        {
          InputTransformationStringCollectionType::iterator location = find(thisList.begin(),thisList.end(),*i);
          if (location == thisList.end())
             {
            // Found a match
            // printf ("Found a match between elements of the two lists in operator+=() \n");
            //      (*i).display("Found a match between elements of the two lists in operator+=()");
               nonMatchingFragmentString.push_back(*i);
             }
        }

  // merge the sets of source code strings and remove any (trivial) replication
  // (more complex identification of psuedo-uniqueness is done later).
     if (preserveStringOrder == true)
        {
       // append the list with the input list
          thisList.insert(
               thisList.end(),
               nonMatchingFragmentString.begin(),
               nonMatchingFragmentString.end());
        }
       else
        {
       // mergeSourceCodeStringList (X.getSourceCodeStringList());
       // sourceStringList.sort();
       // sourceCodeStringList.sort();
       // sourceCodeStringList.merge ( sourceStringList );
       // astFragmentStringContainer.unique();

          printf ("HighLevelRewrite::SynthesizedAttribute::operator+=() called and not implemented for MERGE \n");
          ROSE_ABORT();
        }

#if 0
     printf ("After merging strings in HighLevelRewrite::SynthesizedAttribute::operator+= \n");
     displayStringList("after merge");

     printf ("thisList.size() = %d  sizeOfThisList = %d  sizeOfXList = %d \n",
          thisList.size(),sizeOfThisList,sizeOfXList);
#endif

     if (sizeOfThisList == 0)
        {
          ROSE_ASSERT (thisList.size() == sizeOfXList);
        }
       else
        {
          ROSE_ASSERT (thisList.size() <= sizeOfThisList + sizeOfXList);
        }

     ROSE_ASSERT (thisList.size() == sizeOfThisList + nonMatchingFragmentString.size());

#if 0
     printf ("@@@ In HighLevelRewrite::SynthesizedAttribute::operator+=() skipConcatination = %s Before +=: workspace = %s X.workspace = %s \n",
          skipConcatination ? "true" : "false", workspace.c_str(),X.workspace.c_str());

  // Clearly the effect of this depends upon evaluation order!
     if (!skipConcatination)
          workspace += X.workspace;

     printf ("@@@ In HighLevelRewrite::SynthesizedAttribute::operator+=() After +=: workspace = %s \n",workspace.c_str());
#endif

  // Verify that we have not added redundent copies of strings
     consistancyCheck("At base of HighLevelRewrite::SynthesizedAttribute::operator+=");

     return *this;
   }

void
HighLevelRewrite::SynthesizedAttribute::insert (
   SgNode* target,
   const string & transformationString,
   ScopeIdentifierEnum inputRelativeScope,
   PlacementPositionEnum inputRelativeLocation )
   {
     ROSE_ASSERT (target != NULL);

#if 0
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("In HighLevelRewrite::SynthesizedAttribute::insert() \n");
#endif

     PlacementPositionEnum location = inputRelativeLocation;
     ScopeIdentifierEnum scope      = inputRelativeScope;

     bool buildInNewScope = false;
     TransformationStringTemplatedType<HighLevelCollectionTypedefs> transformation (target,transformationString,scope,location,buildInNewScope);

  // transformation.display("In HighLevelRewrite::SynthesizedAttribute::insert()");

     ROSE_ASSERT (transformation.associatedASTNode     != NULL);
     ROSE_ASSERT (transformation.getRelativeScope()    != HighLevelInterfaceNodeCollection::unknownScope);
     ROSE_ASSERT (transformation.getRelativePosition() != HighLevelInterfaceNodeCollection::unknownPositionInScope);

  // Add the string to the list and return withouth comiling the string
  // (this is the string registration phase)
     addString(target,transformation);

#if 0
     printf ("Leaving HighLevelRewrite::SynthesizedAttribute::insert() \n");
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
   }

void
HighLevelRewrite::SynthesizedAttribute::replace (
   SgNode* target,
   const string & transformationString,
   ScopeIdentifierEnum inputRelativeScope )
   {
  // Semantics of replace is always insert and remove (but it can't always be implemented that 
  // way e.g. for loop initializer statements).  But we have to implement it differently since
  // the lower level mechanisms are used to remove the target statement and also handle any 
  // attached comments/directives.

     insert(target,transformationString,inputRelativeScope,HighLevelInterfaceNodeCollection::ReplaceCurrentPosition);
  // LowLevelRewrite::remove(target);
   }

bool
HighLevelRewrite::SynthesizedAttribute::
isEmpty () const
   {
     bool returnValue = (inputTransformationStrings.size() > 0) ? false : true;
     return returnValue;
   }

bool
HighLevelRewrite::SynthesizedAttribute::
containsStringsForThisNode ( SgNode* astNode )
   {
  // This function determines if there are any strings associated with this synthesised attribute
  // which would be added to the AST.  Only valid strings cound, not strings targeted for the
  // preample location is scope (or the preample scope, if at the global scope).

     bool returnValue = false;
     ROSE_ASSERT (astNode != NULL);

#if 0
     printf ("Inside of SynthesizedAttribute::containsStringsForThisNode(%s): inputTransformationStrings.size() = %d \n",
          astNode->sage_class_name(),
          inputTransformationStrings.size());
//   display("Called from HighLevelRewrite::SynthesizedAttribute::containsStringsForThisNode()");
#endif

     InputTransformationStringCollectionType::const_iterator j;
     for (j = inputTransformationStrings.begin(); j != inputTransformationStrings.end(); j++)
        {
       // ROSE_ASSERT ((*j) != NULL);
          ROSE_ASSERT ((*j).associatedASTNode != NULL);
#if 0
          printf ("Test in loop: (*j).associatedASTNode = %p (%s) == astNode = %p (%s) \n",
               (*j).associatedASTNode,(*j).associatedASTNode->sage_class_name(),astNode,astNode->sage_class_name());
#endif

       // error checking
          ROSE_ASSERT ( (*j).relativeLocation > unknownPositionInScope);
          ROSE_ASSERT ( (*j).relativeLocation < LAST_PLACEMENT_TAG);

       // Only trigger insertion of strings if valid strings exist
       // Ignore strings targeted at the preample position 
       // of any scope or the preample scope
          if ( ( (*j).relativeLocation != PreamblePositionInScope) ||
               ( (*j).relativeScope != Preamble) )
             {
               if ( (*j).associatedASTNode == astNode)
                    returnValue = true;
             }
        }

#if 0
     printf ("return value for containsStringsForThisNode() = %s \n",(returnValue) ? "true" : "false");
#endif

     return returnValue;
   }

void
HighLevelRewrite::SynthesizedAttribute::rewriteAST( SgNode* astNode )
   {
  // Make the changes to the AST as we proceed back through the
  // postorder traversal.  Add all strings specified to be added
  // within the synthesized attribute at the locations in the AST
  // where they were specified to be added.  Since string might be
  // specified to be added in to parent scopes, we have to call this
  // function for each node in the AST (since there my be a string
  // specified at a lower level in the AST for inclusion in a parent
  // scope).  The prefix string is required in we want the
  // intermediate generated file to include specific header files

  // string prefixString = "// Prefix string for intermediate compilation";

  // printf ("isEmpty() == %s \n",(isEmpty() == true) ? "true" : "false");
     if (isEmpty() == false)
        {
       // printf ("SynthesizedAttribute::isEmpty() == false (there are strings present) \n");
          if ( containsStringsForThisNode(astNode) == true )
             {
            // Now we know that there is string data to be inserted so go and insert it into the AST
#if 0
               printf ("Found string(s) for this AST node in this synthesized attribute \n");
               printf ("Calling insertGatheredStringData(astNode) \n");
#endif

#if 1
            // DQ (10/18/2004): This is the code that we should be calling!!!!!
               SgStatement* statement = isSgStatement(astNode);
               ROSE_ASSERT (statement != NULL);
               bool prefixIncludesCurrentStatement = true;
               HighLevelInterfaceNodeCollection::writeAllChangeRequests (statement,prefixIncludesCurrentStatement);
#else
            // DQ (10/18/2004): This following code is present in the writeAllChangeRequests already!!!!!
               printf ("IMPLEMENTATION NOTE: We should be calling HighLevelInterfaceNodeCollection::writeAllChangeRequests()! \n");

            // insertGatheredStringData(project,astNode,declarations);
            // insertGatheredStringData(astNode);

            // New interface (common structure between mid-level and high-level interfaces)
               sortInputStrings(astNode);

            // Not certain what the best value s for the default ???
#if 0
               printf ("In HighLevelRewrite::SynthesizedAttribute::rewriteAST(): Default value of prefixIncludesCurrentStatement == true \n");
#endif
               bool prefixIncludesCurrentStatement = true;
               compileSortedStringsToGenerateNodeCollection(astNode,prefixIncludesCurrentStatement);

#if 0
               printf ("Exiting in HighLevelRewrite::SynthesizedAttribute::rewriteAST() after call to compileSortedStringsToGenerateNodeCollection() \n");
               ROSE_ABORT();
#endif

               SgStatement* statement = isSgStatement(astNode);
               ROSE_ASSERT (statement != NULL);

            // Note that statment lists contain markers
            // insertStatementNodes(statement);
               insertContainerOfListsOfStatements(statement);
#endif
             }
#if 0
            else
             {
               printf ("     Strings are present, but no strings for this AST node found in this synthesized attribute \n");
             }
#endif
        }
#if 0
       else
        {
          printf ("     SynthesizedAttribute::isEmpty() == true (no strings in this synthesized attribute) \n");
        }
#endif
   }







