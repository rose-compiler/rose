// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "nonterminal.h"
#include "grammarString.h"
#include "grammarTreeNode.h"
#include "constraintList.h"
#include "constraint.h"
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

// MS: temporary (TODO: move to processing class)
static string RTIContainerName = "rtiContainer";  // put this into the respective processing class as private member
static string RTIreturnType    = "RTIReturnType"; // typedef in Grammar/Common.code

// Support for output of constructors as part of generated documentation
string Grammar::staticContructorPrototypeString;

// ###############################################################
// # MK: These constants are needed during the generation phase  #
// # of the tree traversal functions                             #
// ###############################################################

#define MAX_ITERATOR_TYPE_STRING_LENGTH 200

// ################################################################
// #                 Grammar Static Data Members                  #
// ################################################################

vector<grammarFile*> Grammar::fileList;

// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

#define WRITE_SEPARATE_FILES_FOR_EACH_CLASS FALSE
#define ALWAYS_BUILD_CLASS_FOR_GRAMMAR TRUE

string replaceString(string patternInInput, string replacePattern, string input) {
  string::size_type posIter = input.find(patternInInput);
  if (posIter != string::npos)
    input.replace(posIter, patternInInput.size(), replacePattern);
  return input;
}

Grammar::~Grammar ()
   {
     
     //cout << "Deleting fileList:\n";
     // BP : 10/23/2001, fix mem.leak
     // MS : 04/25/2002, unfix fix because it breaks
     /*
     list<grammarFile*>::iterator it;
     cout << "Length of fileList: " << fileList.size() << endl;
     for(it=fileList.begin(); it != fileList.end(); it++) {
       cout << "deleting: " << *it << endl;
       delete (*it);
     }
     cout << "done.\n";
     cout << "Clearing file list:\n";
     fileList.clear();
     cout << "done.\n";
     */
}

Grammar::Grammar ()
   {
     printf ("Error: Please use Grammar ( const char* ) constructor \n");
     ROSE_ABORT();
   }

Grammar::Grammar ( const string& inputGrammarName, 
                   const string& inputPrefixName, 
                   const string& inputGrammarNameBaseClass, 
                   const Grammar* inputParentGrammar )
   {
  // The constructor builds a grammar.  The inputs are strings:
  //    inputGrammarName          -- The name of the grammar (used in the construction of filenames etc.)
  //    inputPrefixName           -- The name used for the prefix to all classes generated to implement 
  //                                 the grammar.  This permits each class to have a unique name.  The 
  //                                 prefix for the base level grammar should be "Sg" to match SAGE!
  //    inputGrammarNameBaseClass -- The name of the class representing the grammar!  There can
  //                                 be many grammars defined within a preprocessor.
  //    inputParentGrammar        -- A pointer to the parent grammar (object) representing the base grammar.

  // Intialize some member data 
  // By default the parent grammar is not known
     parentGrammar = NULL;

  // We want to set the parent grammar as early as possible since the specification of terminals/nonterminals is
  // dependent upon the the current grammar being a "RootGrammar" (using the isRootGrammar() member function)
  // and the value of the boolean returned from isRootGrammardepends upon the pointer to the parentGrammar being set!
     bool skipConstructionOfParseFunctions = FALSE;
     if (inputParentGrammar == NULL)
        {
       // We want to skip the construction of parse member function for the C++ grammar
          skipConstructionOfParseFunctions = TRUE;
        }
       else
        {
          setParentGrammar(*inputParentGrammar);
        }
     
  // Principle constructor
     grammarName          = inputGrammarName;
     grammarPrefixName    = inputPrefixName;
     grammarNameBaseClass = inputGrammarNameBaseClass;

     filenameForSupportClasses = "";

  // JJW 2-12-2008 Use a file for this list so the numbers will be more stable
     {
       std::string astNodeListFilename = ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR "/src/ROSETTA/astNodeList";
       std::ifstream astNodeList(astNodeListFilename.c_str());
       size_t c = 1;
       while (astNodeList) {
         std::string name;
         astNodeList >> name;
         if (name == "") continue;
         this->astNodeToVariantMap[name] = c;
         this->astVariantToNodeMap[c] = name;
         ++c;
       }
       ROSE_ASSERT (astNodeList.eof());
       astNodeList.close();
     }
     ROSE_ASSERT (this->astNodeToVariantMap.size() >= 10); // A reasonable count

  // DQ (3/15/2007): Added support for binaries
     setUpBinaryInstructions();

  // Build up the terminals and nonTerminals defined within the default C++ grammar (using SAGE)
     setUpSupport();
     setUpTypes();
     setUpExpressions();
     setUpStatements();
     setUpSymbols();

  // Setup of Node requires previous definition of types, 
  // expressions, statements, symbols within the grammar
     setUpNodes();

  // Specify additional global declarations required for this grammar
     setFilenameForGlobalDeclarations ("../Grammar/Cxx_GlobalDeclarations.macro");

  // We want to skip the construction of parse member function for the C++ grammar
     if ( skipConstructionOfParseFunctions == TRUE)
        {
          NonTerminal & Node = nonTerminalList["Node"];
          Node.excludeSubTreeFunctionPrototype ( "HEADER_PARSER", "../Grammar/Node.code");
          Node.excludeSubTreeFunctionPrototype ( "SOURCE_PARSER", "../Grammar/parserSourceCode.macro");
        }

  // Check the consistency of the data that we just built
     consistencyCheck();
   }

void
Grammar::consistencyCheck() const
   {
#if 0
     int i = 0;

  // check the terminal list for valid objects
     for (i=0; i < terminalList.size(); i++)
        {
       // printf ("terminalList[%d].getName() = %s \n",i,terminalList[i].getName());
          terminalList[i].consistencyCheck();
        }

  // check the terminal list for valid objects
     for (i=0; i < nonTerminalList.size(); i++)
        {
       // printf ("nonTerminalList[%d].getName() = %s \n",i,nonTerminalList[i].getName());
          nonTerminalList[i].consistencyCheck();
        }
#else
  // Call the consistencyCheck function on the list object
     terminalList.consistencyCheck();
     nonTerminalList.consistencyCheck();
#endif

   }


const Grammar*
Grammar::getParentGrammar ()
   {
  // ROSE_ASSERT (parentGrammar != NULL);
     return parentGrammar;
   }

void
Grammar::setParentGrammar ( const Grammar & GrammarPointer )
   {
     parentGrammar = &GrammarPointer;
     ROSE_ASSERT (parentGrammar != NULL);
   }

bool
Grammar::isRootGrammar ()
   {
  // Determine if this is the C++ grammar at the root of the heiarchy of grammars
  // Don't call the getParentGrammar() member function since if the 
  // parentGrammar is false it will trigger an assert (which we want to keep)
     return (parentGrammar == NULL) ? TRUE : FALSE;
   }

void
Grammar::setRootOfGrammar ( GrammarTreeNode* RootNodeForGrammar )
   {
     rootNode = RootNodeForGrammar;
   }

GrammarTreeNode &
Grammar::getRootOfGrammar ()
   {
     ROSE_ASSERT (rootNode != NULL);
     return *rootNode;
   }

NonTerminal &
Grammar::getRootToken ()
   {
     ROSE_ASSERT (rootNode != NULL);
     ROSE_ASSERT (rootNode->token != NULL);
     return *( (NonTerminal*) rootNode->token );
   }

void
Grammar::addGrammarElement ( Terminal & X )
   {
     ROSE_ASSERT (this != NULL);
  // terminalList.display("START of Grammar::addGrammarElement(Terminal)");
     X.setGrammar(this);
     // commented by BP : 10112001
     //     terminalList.push_back ( (Terminal *const &) X );
     const Terminal *const &Y = &X;
     terminalList.push_back ( (Terminal *const &) Y );
  // terminalList.display("END of Grammar::addGrammarElement(Terminal)");
     terminalList.consistencyCheck();
     astVariantToTerminalMap[this->getVariantForTerminal(X)] = &X;
   }

void
Grammar::addGrammarElement ( NonTerminal & X )
   {
     ROSE_ASSERT (this != NULL);
  // nonTerminalList.display("START of Grammar::addGrammarElement(NonTerminal)");
     X.setGrammar(this);
     // commented by BP 10112001
     //     nonTerminalList.push_back ( (Terminal *const &)  X );
     const Terminal *const &Y = (const Terminal *) &X;
     nonTerminalList.push_back ((Terminal *const &) Y );
  // nonTerminalList.display("END of Grammar::addGrammarElement(NonTerminal)");
     nonTerminalList.consistencyCheck();
     astVariantToTerminalMap[this->getVariantForTerminal(X)] = &X;
   }

void
Grammar::deleteGrammarElement ( Terminal & X )
   {
     const Terminal *const &termRef = &X;
     terminalList.remove ( (Terminal *const &) termRef );
   }

void
Grammar::deleteGrammarElement ( NonTerminal & X )
   {
     const Terminal *const &termRef = (const Terminal *)&X;
     nonTerminalList.remove ( (Terminal *const &) termRef );
   }

const std::string&
Grammar::getGrammarPrefixName() const
   {
  // This function returns the name of the grammar and is used to substitute for 
  // "$GRAMMAR_PREFIX_" in the name of the generated classes that represent the
  // terminals and nonterminals of the grammar.
     ROSE_ASSERT(this != NULL);
     return grammarPrefixName;
   }

const std::string&
Grammar::getGrammarName() const
   {
  // This function returns the name of the grammar and is used to substitute for 
  // "$GRAMMAR_PREFIX_" in the name of the generated classes that represent the
  // terminals and nonterminals of the grammar.
     ROSE_ASSERT(this != NULL);
     return grammarName;
   }

std::string
Grammar::getGrammarTagName()
   {
  // This function returns the grammar name to be substituted for "$GRAMMAR_TAG_PREFIX_"
  // so that all tags (in the global enum) are unique.  However since for SAGE the enums
  // are already set (they are used in the EDG code and I would like to avoid changing the 
  // SAGE EDG interface, and generaly I would like to have the base level grammar be 
  // compatable with SAGE) we want to have the "$GRAMMAR_TAG_PREFIX_" be "" in the case of the
  // SAGE grammar.  To do this we recognize if this grammar is the base level grammar (root == NULL)
  // and return "" in this case.  Within the hierarchy of grammars that ROSETTA builds
  // the root grammar (base level grammar) has a NULL pointer for it's parent grammar.
  // This is sufficient for its identification.

     ROSE_ASSERT(this != NULL);
     string returnName = "";
     if (parentGrammar != NULL)
        {
       // returnName = getGrammarName();
          returnName = getGrammarPrefixName();
        }

     return returnName;
   }

/**********************************
 * Terminal/Nonterminal functions *
 **********************************/

bool
Grammar::isTerminal ( const string& terminalName ) const
   {
  // return terminalList.containedInList(terminalName);

     bool returnValue = terminalList.containedInList(terminalName);

#if 0
     if (returnValue == FALSE)
        {
       // print out the names in the terminalList
          terminalList.display("Called from Grammar::isTerminal()");
        }
#endif

     return returnValue;
   }

bool
Grammar::isNonTerminal ( const string& nonTerminalName ) const
   {
     return nonTerminalList.containedInList(nonTerminalName);
   }

#if 0
// This function could only get a terminal (not a nonterminal, 
// since functions can't be overloaded on return type)
Terminal &
Grammar::operator[] ( char* name ) const
   {
  // get a handle to reference to name of the terminal/nonterminal
     Terminal* terminalPtr = NULL;

     ROSE_ASSERT( (isTerminal(name) == TRUE) || ( isNonTerminal(name) == TRUE) );
     if (isTerminal(name) == TRUE)
        {
          terminalPtr = &(terminalList[name]);
        }
       else
        {
          terminalPtr = &(nonTerminalList[name]);
        }

     ROSE_ASSERT(terminalPtr != NULL);

     return *terminalPtr;
   }
#endif

Terminal &
Grammar::getTerminal    ( const string& name ) const
   {
  // get a handle to reference to name of the terminal
     Terminal* terminalPtr = NULL;

  // error checking
     if (isTerminal(name) == FALSE)
        {
       // print out the names in the terminalList
	printf ("ERROR: isTerminal(name) == FALSE: name = %s not found in terminalList \n",name.c_str());
          terminalList.display("Called from Grammar::isTerminal()");
        }

     ROSE_ASSERT (isTerminal(name) == TRUE);

     if (isTerminal(name) == TRUE)
        {
          terminalPtr = &(terminalList[name]);
        }

     ROSE_ASSERT(terminalPtr != NULL);
     ROSE_ASSERT(terminalPtr->associatedGrammar != NULL);

     return *terminalPtr;
   }

NonTerminal &
Grammar::getNonTerminal ( const string& name ) const
   {
  // get a handle to reference to name of the nonterminal
     NonTerminal* nonTerminalPtr = NULL;

  // ROSE_ASSERT (isNonTerminal(name) == TRUE);
     if (isNonTerminal(name) == TRUE)
        {
          nonTerminalPtr = &(nonTerminalList[name]);
        }
       else
        {
          printf ("Error: In Grammar::getNonTerminal ( char* name ) can't find name = %s in nonterminal list \n", name.c_str());
          ROSE_ABORT();
        }

     ROSE_ASSERT(nonTerminalPtr != NULL);
     ROSE_ASSERT(nonTerminalPtr->associatedGrammar != NULL);

     return *nonTerminalPtr;
   }

void
Grammar::remove ( const string& terminalName )
   {
  // This function removes all references of the name matching terminalName
  // from the terminal or nonTerminal lists within the grammar and
  // within the orTerminal or orNonTrminal lists on the nonterminal
  // (if terminalName is a nonTerminal).

     ROSE_ASSERT( (isTerminal(terminalName) == TRUE) || ( isNonTerminal(terminalName) == TRUE) );
     internalRemove(terminalName);

     // Now remove associated terminals and nonterminals (case-by-case)
     /*
     // Example construction
     if (GrammarString::isSameName(terminalName,"") || GrammarString::isSameName(terminalName,""))
        {
          internalRemove("");
          internalRemove("");
        }
     */

     if (terminalName == "TypeBool" || terminalName == "BoolValExp")
        {
          internalRemove("TypeBool");
          internalRemove("BoolValExp");
        }

     if (terminalName == "TypeVoid" || terminalName == "DeleteExp")
        {
          internalRemove("TypeVoid");
          internalRemove("DeleteExp");
        }

     if (terminalName == "FunctionType" || terminalName == "FunctionDeclaration")
        {
          internalRemove("FunctionType");
          internalRemove("FunctionDeclaration");
        }

     if (terminalName == "EnumType" || terminalName == "EnumDeclaration")
        {
          internalRemove("EnumType");
          internalRemove("EnumDeclaration");
        }
   }

void
Grammar::internalRemove ( const string& terminalName )
   {
  // This function removes all references of the name matching terminalName
  // from the terminal or nonTerminal lists within the grammar and
  // within the orTerminal or orNonTerminal lists on the nonterminal
  // (if terminalName is a nonTerminal).

  // We might have already removed this terminal/nonterminal from the grammar and
  // so for this function interface this is not an error
  // ROSE_ASSERT( (isTerminal(terminalName) == TRUE) || ( isNonTerminal(terminalName) == TRUE) );

     if ( (isTerminal(terminalName) == TRUE) && (isNonTerminal(terminalName) == TRUE) )
        {
          if (isTerminal(terminalName) == TRUE)
             {
               Terminal & X = terminalList[terminalName];

               printf ("In Grammar::remove(): remove %s from terminalList \n",X.getName().c_str());
               Terminal *const &termRef = &X;
            // It is sufficient to just remove the terminal from the list
               terminalList.remove(termRef);

            // Check to make sure that this termnal name is no longer a part of this grammar
               ROSE_ASSERT(isTerminal(terminalName) == FALSE);
             }
            else
             {
               ROSE_ASSERT(isNonTerminal(terminalName) == TRUE);

               NonTerminal & X = nonTerminalList[terminalName];
	       
               printf ("In Grammar::remove(): remove %s from nonTerminalList \n",X.getName().c_str());

               removeReferenceFromLists(X);
               Terminal * const &termRef = (Terminal *)&X;
               nonTerminalList.remove( termRef );

               ROSE_ASSERT(isNonTerminal(terminalName) == FALSE);
             }
        }
   }

void
Grammar::removeReferenceFromLists ( Terminal & X )
{
  // Terminals and nonterminals are referenced within some nonterminals through their
  // and/or lists (list built through the execution of & and | operators between 
  // terminals and nonterminals).
  
  printf ("In Grammar::removeReferenceFromLists(): remove %s from all internal lists \n",X.getName().c_str());
  
  list<Terminal *>::const_iterator it;
  
  for( it = nonTerminalList.begin(); it != nonTerminalList.end(); it++ )
    {
      assert( *it != NULL );
      NonTerminal *temp = (NonTerminal *)*it;
      
      list<Terminal *>::const_iterator jt;
      for( jt = temp->andTokenPointerArray.begin(); jt != temp->andTokenPointerArray.end(); jt++ )
	{
	  Terminal *andTemp = (*jt);

	  if (andTemp == &X)
	    {
	      printf ("remove %s from (%s).andTokenPointerArray list \n",X.getName().c_str(),temp->getName().c_str());
	      Terminal *const &tmpRef = andTemp;
	      temp->andTokenPointerArray.remove( tmpRef );
	    }
	}

      list<NonTerminal *>::iterator nonTerminalIterator;
      for( nonTerminalIterator = temp->andNonTerminalPointerArray.begin(); 
	   nonTerminalIterator != temp->andNonTerminalPointerArray.end(); 
	   nonTerminalIterator++ )
	{
	  NonTerminal  *andTemp = *nonTerminalIterator;
	  if (andTemp == &X)
	    {
	      printf ("remove %s from (%s).andNonTerminalPointerArray list \n",X.getName().c_str(),temp->getName().c_str());
	      NonTerminal *const &tmpRef = andTemp;
	      temp->andNonTerminalPointerArray.remove(tmpRef);
	    }
	}
      
      for( jt = temp->orTokenPointerArray.begin(); jt != temp->orTokenPointerArray.end(); jt++ )
	{
	  Terminal  *orTemp = *jt;
	  if (orTemp == &X)
	    {
	      printf ("remove %s from (%s).orTokenPointerArray list \n",X.getName().c_str(),temp->getName().c_str());
	      Terminal *const &tmpRef = orTemp;
	      temp->orTokenPointerArray.remove(tmpRef);
	    }
	}
      
      for( nonTerminalIterator = temp->orNonTerminalPointerArray.begin();
	   nonTerminalIterator != temp->orNonTerminalPointerArray.end();
	   nonTerminalIterator++ )
	{
	  NonTerminal  *orTemp = *nonTerminalIterator;
	  if (orTemp == &X)
	    {
	      printf ("remove %s from (%s).orNonTerminalPointerArray list \n",X.getName().c_str(),temp->getName().c_str());
	      NonTerminal *const &tmpRef = orTemp;
	      temp->orNonTerminalPointerArray.remove( tmpRef );
	    }
	}
      
    }
}

Terminal & 
Grammar::terminalConstructor ( const string& lexeme, Grammar & X, const string& stringVar, const string& tagString )
   {
  // These functions build terminal and nonterminal objects to be associated with this grammar
  // Using a member function to construct these serves several purposes:
  // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
  // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
  //    that the letter will have a scope longer than the envelope

     return *(new Terminal ( lexeme, X, stringVar, tagString ));
   }

NonTerminal &
Grammar::nonTerminalConstructor ( const NonTerminal & X )
   {
  // These functions build terminal and nonterminal objects to be associated with this grammar
  // Using a member function to construct these serves several purposes:
  // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
  // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
  //    that the letter will have a scope longer than the envelope

     return *(new NonTerminal ( X ));
   }

NonTerminal &
Grammar::nonTerminalConstructor ( const Terminal & X )
   {
  // These functions build terminal and nonterminal objects to be associated with this grammar
  // Using a member function to construct these serves several purposes:
  // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
  // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
  //    that the letter will have a scope longer than the envelope.

  // bool originalSetting = X.isTemporary();

  // Warning: casing away const here!!
     // NonTerminal XAsNonterminal = X;
     // XAsNonterminal.setTemporary(TRUE);
     ROSE_ASSERT (X.getParentTerminal() == NULL);

     NonTerminal* newNonTerminal = new NonTerminal ( X );
     ROSE_ASSERT(newNonTerminal != NULL);
     // XAsNonterminal.setTemporary(originalSetting);

     ROSE_ASSERT (newNonTerminal->parentTerminal == NULL);

     return *newNonTerminal;
   }

//////////////////////////////////////////////////////////////////////////////////////
// CREATE DATA STRUCTURE REPRESENTING THE TYPE HIERARCHY OF THE AST (GRAMMAR NODES) //
//////////////////////////////////////////////////////////////////////////////////////
void
Grammar::buildTree()
   {
  // Define the root of the tree as rootOfGrammar  (we will attach all subtrees that
  // we have left to this root at the end).

  // Each nonterminal with more then 1 element in its OR list will generate a GrammarTreeNode
  // and will use each element in the OR list to define children of that GrammarTreeNode.
  // This list of parent Grammar Tree Nodes is put into a list (to be searched).  As each element
  // is put into child GrammarTreeNode, the list of parent grammar tree nodes is searched to see if
  // it is already represented.

     list<GrammarTreeNode *> grammarTreeList;
     list<Terminal *>::const_iterator  it;
     list<GrammarTreeNode *>::const_iterator grammarTreeIterator;

     unsigned int i=0;

#if 1
  // printf ("Process all the Terminals first \n");
  // If we comment out the leafs the tree representing the grammar's implementation is
  // smaller and easier to debug!
     
     for (i=0; i < terminalList.size(); i++)
        {
          ROSE_ASSERT (terminalList[i].getMemberFunctionPrototypeList
               (Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() == 0);

       // Bugfix (7/7/2001) Use the existing GrammarTreeNode if it has already been built for this NonTerminal
       // GrammarTreeNode* temp = new GrammarTreeNode (terminalList[i]);
          GrammarTreeNode* temp = (terminalList[i].grammarSubTree != NULL) ? 
                                  terminalList[i].grammarSubTree : 
                                  new GrammarTreeNode (terminalList[i]);

          ROSE_ASSERT (terminalList[i].getGrammar() != NULL);
          terminalList[i].addGrammarPrefixToName();
          grammarTreeList.push_back(temp);

       // printf ("temp->getName() = %s \n",temp->getName());
	  //	  terminalList[i].display("Terminals in TerminalList in buildTree");
        }
#else
     printf ("WARNING: ####### LEAVES OF GRAMMAR COMMENTED OUT!!! ####### \n");
#endif

  // printf ("Process all the NonTerminals second \n");
  // Build all the tree nodes up front and then connect them together to form the tree

     for( it = nonTerminalList.begin(); it != nonTerminalList.end(); it++)
        {

	  assert( (*it) != NULL );
	  NonTerminal &currentNonTerminal = (NonTerminal &)**it;
	  //	  currentNonTerminal.show();
	  //	  printf ("Build all the tree nodes up front: i = %d \n",i);
	  // nonTerminalList[i].show();

       // ROSE_ASSERT (nonTerminalList[i].subTreeMemberFunctionList.size() == 0);
	  //	  printf ("nonTerminalList[%d] = %s \n",i,nonTerminalList[i].getName());

       // Bugfix (7/7/2001) Use the existing GrammarTreeNode if it has already been built for this NonTerminal
       // GrammarTreeNode* temp = new GrammarTreeNode (nonTerminalList[i]);
          GrammarTreeNode* temp = (currentNonTerminal.grammarSubTree != NULL) ? 
                                  currentNonTerminal.grammarSubTree : 
 	                          new GrammarTreeNode (currentNonTerminal); // bugfix MS:2002
          currentNonTerminal.addGrammarPrefixToName();
          ROSE_ASSERT (temp != NULL);

          grammarTreeList.push_back(temp);
          ROSE_ASSERT (temp->token != NULL);
       // ROSE_ASSERT (temp->token->subTreeMemberFunctionList.size() == 0);
       // ROSE_ASSERT (nonTerminalList[i].subTreeMemberFunctionList.size() == 0);

#if 0  // I think this code does nothing BP : 10/12/2001
	  list<Terminal *>::const_iterator jt;
	  for( jt = currentNonTerminal.orTokenPointerArray.begin(); 
	       jt != currentNonTerminal.orTokenPointerArray.end(); jt++)
             {
               ROSE_ASSERT ((*jt)->name != NULL);
               char* nameInProductionRule = (*jt)->name;
	       printf ("nameInProductionRule = %s \n",nameInProductionRule);
               ROSE_ASSERT (nameInProductionRule != NULL);

#if 0
            // Error checking associated with tracing down Insure++ penpointed error
            // printf ("Checking nonTerminalList[i].orTokenPointerArray[j].grammarSubTree = %p \n",
            //      nonTerminalList[i].orTokenPointerArray[j].grammarSubTree);
               if ((*jt)->grammarSubTree != NULL)
                    printf ("nonTerminalList[%d].orTokenPointerArray[%d].grammarSubTree != NULL name = %s \n",i,j,(*jt)->grammarSubTree->getName());
#endif

            // ROSE_ASSERT (nonTerminalList[i].orTokenPointerArray[j].grammarSubTree == NULL);

            // Bugfix (7/7/2001) Use the existing GrammarTreeNode if it has already been built for this NonTerminal
            // GrammarTreeNode* lowerLevelTreeNode = new GrammarTreeNode (nonTerminalList[i].orTokenPointerArray[j]);
               GrammarTreeNode* lowerLevelTreeNode = ((*jt)->grammarSubTree != NULL) ? 
                                                     (*jt)->grammarSubTree :
                                                     new GrammarTreeNode (**jt);
               ROSE_ASSERT (lowerLevelTreeNode != NULL);
               char* lowerLevelName = lowerLevelTreeNode->getName();
               ROSE_ASSERT (lowerLevelName != NULL);
	     }
#endif
	  //	  currentNonTerminal.display("NonTerminals in nonTerminalList in buildTree");
        }

  // Now connect the tree nodes together to form the tree
     for( it = nonTerminalList.begin(); it != nonTerminalList.end(); it++)
        {
	  list<GrammarTreeNode *> treeNodesToDelete;
	  ROSE_ASSERT(treeNodesToDelete.size()==0);
	  assert( (*it) != NULL );
	  NonTerminal &currentNonTerminal = (NonTerminal &)**it;
          GrammarTreeNode* newNonTerminalTreeNode = currentNonTerminal.grammarSubTree;
          ROSE_ASSERT (newNonTerminalTreeNode != NULL);
	  
#if 0
          printf ("(newNonTerminalTreeNode) nonTerminalList[%d].name = %s (# of Tokens = %d # of Production rules = %d # of subTreeMemberFunctions = %d) \n",
		  i,currentNonTerminal.name,
		  currentNonTerminal.orTokenPointerArray.size(),
		  currentNonTerminal.orNonTerminalPointerArray.size(),
		  currentNonTerminal.subTreeMemberFunctionList.size());
#endif
	  
          ROSE_ASSERT (currentNonTerminal.getMemberFunctionPrototypeList
                       (Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() ==
                       newNonTerminalTreeNode->token->getMemberFunctionPrototypeList
                       (Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size());
	  
       // Search through the tokens to find the grammar elements in the grammarTreeList
	  list<Terminal *>::const_iterator jt;
	  for( jt = currentNonTerminal.orTokenPointerArray.begin(); 
	       jt != currentNonTerminal.orTokenPointerArray.end(); jt++)
             {
               string nameInProductionRule = (*jt)->name;
            // printf ("nameInProductionRule = %s \n",nameInProductionRule);

            // GrammarTreeNode* lowerLevelTreeNode = new GrammarTreeNode (currentNonTerminal.orTokenPointerArray[j]);
               GrammarTreeNode* lowerLevelTreeNode = (*jt)->grammarSubTree;
               ROSE_ASSERT (lowerLevelTreeNode != NULL);
               string lowerLevelName = lowerLevelTreeNode->getName();

            // Now check to see if it is in the list of unconnected subtrees
	       bool foundMatchingString = FALSE;
	       for( grammarTreeIterator = grammarTreeList.begin();
		    grammarTreeIterator != grammarTreeList.end(); grammarTreeIterator++ )
		 {
		   ROSE_ASSERT((*grammarTreeIterator)!=NULL);
                    string grammarTreeListname = (*grammarTreeIterator)->getName();
                    if ( lowerLevelName == grammarTreeListname )
                       {
#if 0
                         printf ("Found Matching Name! name = %s list length = %d \n",
                              grammarTreeListname,grammarTreeList.size());
#endif
			 ROSE_ASSERT (foundMatchingString == FALSE);

                      // newNonTerminalTreeNode->nodeList.addElement(lowerLevelNode);
                         newNonTerminalTreeNode->nodeList.push_back(*grammarTreeIterator);
                      // Remove the lowerLevelNode from the list 
                      // grammarTreeList.remove(lowerLevelNode);

                      // Connect the tree node to its parent in the tree and the token to the tree
                      // This permits us to traverse back through the tree to the root from any node
                      // in the tree.
			 ROSE_ASSERT ((*grammarTreeIterator)->token != NULL);
                      // grammarTreeList[k].token->grammarSubTree = &(grammarTreeList[k]);
                         (*grammarTreeIterator)->parentTreeNode = newNonTerminalTreeNode;

                      // We need to add this to a list of Tree nodes to delete 
                      // and delete them after this loop (or use a while loop!)
			 treeNodesToDelete.push_back(*grammarTreeIterator);
                       }
                  }

	       list<GrammarTreeNode *>::iterator treeNodeIterator;
	       for( treeNodeIterator=treeNodesToDelete.begin();
		    treeNodeIterator!=treeNodesToDelete.end(); treeNodeIterator++)
		 {
		   grammarTreeList.remove(*treeNodeIterator);
		 }

	       treeNodesToDelete.clear();
	       assert(treeNodesToDelete.size()==0);
            // If we didn't find a match then this is a leaf in the 
            // tree representing the organization of the grammar
            // printf ("Exiting after processing of first nonTerminal in the nonTerminalList! \n");
            // ROSE_ABORT();
             }

          ROSE_ASSERT (currentNonTerminal.getMemberFunctionPrototypeList
                       (Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() ==
                       newNonTerminalTreeNode->token->getMemberFunctionPrototypeList
                       (Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size());

          // Search through the nonterminals to find the grammar elements in the grammarTreeList
	  list<NonTerminal *>::const_iterator nonTerminalIterator;
	  for( nonTerminalIterator = currentNonTerminal.orNonTerminalPointerArray.begin(); 
	       nonTerminalIterator != currentNonTerminal.orNonTerminalPointerArray.end(); 
	       nonTerminalIterator++)
	    {
	       ROSE_ASSERT( *nonTerminalIterator != NULL );
       // char* nameInProductionRule = (*nonTerminalIterator)->name;

          GrammarTreeNode* lowerLevelTreeNode = (*nonTerminalIterator)->grammarSubTree;
          ROSE_ASSERT (lowerLevelTreeNode != NULL);
          ROSE_ASSERT (lowerLevelTreeNode->token != NULL);
          string lowerLevelName = lowerLevelTreeNode->getName();

       // Now check to see if it is in the list of unconnected subtrees
	       bool foundMatchingString = FALSE;
	       for( grammarTreeIterator = grammarTreeList.begin();
		    grammarTreeIterator != grammarTreeList.end();
		    grammarTreeIterator++ )
		 {
		   string grammarTreeListname = (*grammarTreeIterator)->getName();
		   
		   if ( lowerLevelName == grammarTreeListname )
		     {
#if 0
		       printf ("Found Matching Name! name = %s list length = %d \n",
			       grammarTreeListname,grammarTreeList.size());
#endif
		       newNonTerminalTreeNode->nodeList.push_back(*grammarTreeIterator);
		       
		       // Connect the tree node to its parent in the tree and the token to the tree
		       // This permits us to traverse back through the tree to the root from any node
		       // in the tree.
		       ROSE_ASSERT ((*grammarTreeIterator)->token != NULL);
		       (*grammarTreeIterator)->parentTreeNode = newNonTerminalTreeNode;
		       
		       // We need to add this to a list of Tree nodes to delete 
		       // and delete them after this loop (or use a while loop!)
		       // Instead we now just delete the element directly!
		       treeNodesToDelete.push_back(*grammarTreeIterator);
		       foundMatchingString = TRUE;
		     }

		 }

	       list<GrammarTreeNode *>::iterator treeNodeIterator;
	       for( treeNodeIterator=treeNodesToDelete.begin();
		    treeNodeIterator!=treeNodesToDelete.end(); treeNodeIterator++)
		 {
		   grammarTreeList.remove(*treeNodeIterator);
		 }

	       treeNodesToDelete.clear();
	       assert(treeNodesToDelete.size()==0);
	    }
        }

     printf ("Remaining disconnected subtrees in our grammar: \n");
     //     printf ("The size of the grammarTreeList is %zu", grammarTreeList.size());

     for( grammarTreeIterator = grammarTreeList.begin();
	  grammarTreeIterator != grammarTreeList.end();
	  grammarTreeIterator++ )
       {
	 string name = (*grammarTreeIterator)->getName();
	 printf ("          %s \n",name.c_str());
       }

  // If we don't need to build a new node in the grammar tree to 
  // represent the root then let's skip doing so and let the
  // single node left after collapsing the tree be the root node.
     GrammarTreeNode* rootNodeForGrammar = NULL;
     if (grammarTreeList.size() > 1)
        {
       // All the grammars we build should have a single root,
       // so it is an error for grammarTreeList.size() > 1

          printf ("Error: too many terminals/nonterminals remaining outside of grammar! \n");
          ROSE_ABORT();
        }
       else
        {
       // Use the single existing node!
          rootNodeForGrammar = *grammarTreeList.begin();
        }

     ROSE_ASSERT (rootNodeForGrammar != NULL);

  // Call member function of Grammar class
     setRootOfGrammar (rootNodeForGrammar);
     ROSE_ASSERT (rootNode != NULL);

  // Setup list of leaves of the grammar tree
     setupLeafNodeList();

  // Setup list of all nodes in the grammar tree
  // traverseTreeToSetupAllNodeList();
   }

// define MAX_NUMBER_OF_LEVELS 10
#define MAX_BUFFER_SIZE 350000

#define GENERATED_CODE_DIRECTORY "generatedCode.headers"

#define OUTPUT_TO_FILE TRUE

#define DEBUG_COPY_EDIT FALSE

#if 0
string
Grammar::readFile ( const string& inputFileName )
   {
     return StringUtility::readFile(inputFileName);
#if 0
  // Reads entire text file and places contents into a single string
  // We implemennt a file cache to improve the performance of this file access

// [DT] 5/10/2000 -- NOTE: Uncommented the following line.
// [DT] 5/11/2000 -- NOTE: Re-commented the following line.
//   printf ("Looking for filename = %s (number of files in cache = %zu) \n",fileName,fileList.size());

     list<grammarFile*>::iterator i;
     for (i = fileList.begin(); i != fileList.end(); i++)
        {
	  //	  printf ("fileList element filename = %s  target filename = %s \n",(*i)->getFilename(), fileName);
          if ( (*i)->getFilename() == inputFileName )
             {
	       return (*i)->getBuffer();
             }
        }

	  //	  printf ("File not found in cache (build it) filename = %s \n",fileName);

     ifstream grammarInputFile(inputFileName.c_str(), ios::binary);
     string result;
     char buffer[1 << 20];
          if (grammarInputFile.good() != TRUE)
             {
          fprintf (stderr, "ERROR: File not found -- %s \n",inputFileName.c_str());
               ROSE_ABORT();
             }
     while (grammarInputFile && !grammarInputFile.eof()) {
       grammarInputFile.read(buffer, 1 << 20);
       result.insert(result.end(), buffer, buffer + grammarInputFile.gcount());
             }
     ROSE_ASSERT (grammarInputFile || grammarInputFile.eof());

   //Now add the file to the list (cache)
     //	  cout << "Did not find file in fileList, reading in grammarFile" << endl;
     //	  cout << "The current length of the file list is " << fileList.size() << endl;
     grammarFile *file = new grammarFile(inputFileName,result);
     ROSE_ASSERT (file != NULL);

     fileList.push_front(file);
     return result;
#endif
   }
#endif

StringUtility::FileWithLineNumbers
Grammar::readFileWithPos ( const string& inputFileName )
   {
  // Reads entire text file and places contents into a single string
  // We implemennt a file cache to improve the performance of this file access

// [DT] 5/10/2000 -- NOTE: Uncommented the following line.
// [DT] 5/11/2000 -- NOTE: Re-commented the following line.
  // printf ("Looking for filename = %s (number of files in cache = %zu) \n",inputFileName.c_str(),fileList.size());

     vector<grammarFile*>::iterator i;
     for (i = fileList.begin(); i != fileList.end(); i++)
        {
	    // printf ("fileList element filename = %s  target filename = %s \n",(*i)->getFilename().c_str(), inputFileName.c_str());
          if ( (*i)->getFilename() == inputFileName )
             {
	       return (*i)->getBuffer();
             }
             }

      //	  printf ("File not found in cache (build it) filename = %s \n",fileName);

   //Now add the file to the list (cache)
	  //	  cout << "Did not find file in fileList, reading in grammarFile" << endl;
	  //	  cout << "The current length of the file list is " << fileList.size() << endl;
     StringUtility::FileWithLineNumbers result = StringUtility::readFileWithPos(inputFileName);
  // printf("result.size() = %zu\n", result.size());

     grammarFile *file = new grammarFile(inputFileName,result);
          ROSE_ASSERT (file != NULL);

     fileList.push_back(file);
     return result;
   }

void
Grammar::writeFile ( const StringUtility::FileWithLineNumbers& outputString, 
                     const string& directoryName, 
                     const string& className, 
                     const string& fileExtension )
   {
  // char* directoryName = GrammarString::stringDuplicate(directoryName);

     string outputFilename = (directoryName == "." ? "" : directoryName + "/") + className + fileExtension;
     // printf ("outputFilename = %s \n",outputFilename.c_str());
     ofstream ROSE_ShowFile(outputFilename.c_str());
     ROSE_ASSERT (ROSE_ShowFile.good() == TRUE);

  // Select an output stream for the program tree display (cout or <filename>.C.roseShow)
  // Macro OUTPUT_SHOWFILE_TO_FILE is defined in the transformation_1.h header file
     ostream & outputStream = (OUTPUT_TO_FILE ? (ROSE_ShowFile) : (cout));
     ROSE_ASSERT (outputStream.good() == TRUE);

     outputStream << StringUtility::toString(outputString, outputFilename);
     ROSE_ASSERT (outputStream.good() == TRUE);

     ROSE_ShowFile.close();
     ROSE_ASSERT (outputStream.good() == TRUE);
   }


string
Grammar::sourceCodeDirectoryName ()
   {
     return "GENERATED_CODE_DIRECTORY_" + getGrammarName();
   }


void 
Grammar::generateStringListsFromSubtreeLists ( GrammarTreeNode & node,
					       list<GrammarString *> & includeList,
					       list<GrammarString *> & excludeList,
					       FunctionPointerType listFunction )
{
  // This function traverses back through the grammar tree to collect the elements in the
  // SUBTREE_LISTs (including the SUBTREE_LISTs of the current node).
  // Since we want the parent node list elements listed first we
  // perform a postorder traversal.
  
  list<GrammarString *>::const_iterator grammarStringIterator;
  list<GrammarString *> &listOfIncludes = (node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST);
  list<GrammarString *> &listOfExcludes = (node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST);

#define PREORDER_TRAVERSAL FALSE

#if (PREORDER_TRAVERSAL == TRUE)

  for( grammarStringIterator = listOfIncludes.begin(); 
       grammarStringIterator != listOfIncludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin(); 
       grammarStringIterator != listOfExcludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
  
  if (node.parentTreeNode != NULL) {
    // Recursive function call
    generateStringListsFromSubtreeLists (*(node.parentTreeNode), includeList, excludeList, listFunction );
  }
  else {
#if 0
    // Debugging code
    int i = 0;
    for (i=0; i < (node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size(); i++)
      printf ("In Grammar::generateStringListsFromSubtreeLists - node = %s: includeList[%d] = %s ",
	      node.getName(),
	      i,
	      ((node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST)[i]).
	      getRawString() );
    for (i=0; i < (node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST).size(); i++)
      printf ("In Grammar::generateStringListsFromSubtreeLists - node = %s: excludeList[%d] = %s ",
	      node.getName(),
	      i,
	      ((node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST)[i]).
	      getRawString() );
#endif
  }
  
#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#if (PREORDER_TRAVERSAL == FALSE)

  for( grammarStringIterator = listOfIncludes.begin(); 
       grammarStringIterator != listOfIncludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin(); 
       grammarStringIterator != listOfExcludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
}


void 
Grammar::generateStringListsFromLocalLists ( GrammarTreeNode & node,
					     list<GrammarString *> & includeList,
					     list<GrammarString *> & excludeList,
					     FunctionPointerType listFunction )
{
  // This function traverses back through the grammar tree to collect the elements in the
  // LOCAL_LISTs (including the LOCAL_LISTs of the current node).
  // Since we want the parent node list elements listed first we
  // perform a postorder traversal.

  list<GrammarString *>::const_iterator grammarStringIterator;
  list<GrammarString *> &listOfIncludes = (node.token->*listFunction)(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
  list<GrammarString *> &listOfExcludes = (node.token->*listFunction)(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);
  
#define PREORDER_TRAVERSAL FALSE

#if (PREORDER_TRAVERSAL == TRUE)

  for( grammarStringIterator = listOfIncludes.begin(); 
       grammarStringIterator != listOfIncludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin(); 
       grammarStringIterator != listOfExcludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
  
  if (node.parentTreeNode != NULL) {
    // Recursive function call
    generateStringListsFromLocalLists (*(node.parentTreeNode), includeList, excludeList, listFunction );
  }
  else {
#if 0
    // Debugging code
    int i = 0;
    for (i=0; i < (node.token->*listFunction)(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST).size(); i++)
      printf ("In Grammar::generateStringListsFromSubtreeLists - node = %s: includeList[%d] = %s ",
	      node.getName(),
	      i,
	      ((node.token->*listFunction)(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST)[i]).
	      getRawString() );
    for (i=0; i < (node.token->*listFunction)(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST).size(); i++)
      printf ("In Grammar::generateStringListsFromSubtreeLists - node = %s: excludeList[%d] = %s ",
	      node.getName(),
	      i,
	      ((node.token->*listFunction)(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST)[i]).
	      getRawString() );
#endif
  }
  
#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#if (PREORDER_TRAVERSAL == FALSE)

  for( grammarStringIterator = listOfIncludes.begin(); 
       grammarStringIterator != listOfIncludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin(); 
       grammarStringIterator != listOfExcludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
}


void 
Grammar::generateStringListsFromAllLists ( GrammarTreeNode & node,
					   list<GrammarString *> & includeList,
					   list<GrammarString *> & excludeList,
					   FunctionPointerType listFunction )
{
  // This function traverses back through the grammar tree to collect the elements in the
  // SUBTREE_LISTs and in the LOCAL_LISTs (including the lists of the current node).
  // Since we want the parent node list elements listed first we perform a postorder traversal.

  list<GrammarString *>::const_iterator grammarStringIterator;

  list<GrammarString *> &listOfSubTreeIncludes = (node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST);
  list<GrammarString *> &listOfSubTreeExcludes = (node.token->*listFunction)(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST);
  list<GrammarString *> &listOfLocalIncludes = (node.token->*listFunction)(Terminal::LOCAL_LIST,  Terminal::INCLUDE_LIST);
  list<GrammarString *> &listOfLocalExcludes = (node.token->*listFunction)(Terminal::LOCAL_LIST,  Terminal::EXCLUDE_LIST);
  
#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

  if (node.parentTreeNode != NULL)
    {
      // Recursive function call
      generateStringListsFromAllLists (*(node.parentTreeNode), includeList, excludeList, listFunction );
    }
  
#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

  // Traverse both include lists & both exclude lists
  for( grammarStringIterator = listOfSubTreeIncludes.begin(); 
       grammarStringIterator != listOfSubTreeIncludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfSubTreeExcludes.begin(); 
       grammarStringIterator != listOfSubTreeExcludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (excludeList, **grammarStringIterator );

  for( grammarStringIterator = listOfLocalIncludes.begin(); 
       grammarStringIterator != listOfLocalIncludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfLocalExcludes.begin(); 
       grammarStringIterator != listOfLocalExcludes.end(); 
       grammarStringIterator++)
    Terminal::addElementToList (excludeList, **grammarStringIterator );
  
#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

}

void Grammar::editStringList ( list<GrammarString *> & targetList, list<GrammarString *> & excludeList )
{
  // Remove the elements in the excludeList from the elements in targetList
  // The match is determined by the use of the operator= on the elements!
  // list is modified!
  list<GrammarString *> deleteList;
  list<GrammarString *>::iterator it;
  list<GrammarString *>::iterator jt;

  // BP 10/22/2001, in the earlier version of this code Dan chose to 
  // abort() if the list contained duplicate entries. This seems to have
  // been related to some internal errors in the old list class. Since
  // we now use STL I use the unique function to remove any duplicate entries

  // targetList.sort();
  // targetList.unique();

#if 1
  // we can also have the case where two different objects contain the same data
  // BP 10/15/2001
  for( it = targetList.begin(); it != targetList.end(); it++) 
    for( jt = it; jt != targetList.end(); jt++) 
      {
	if((*it != *jt)&&( **it==**jt))
	  deleteList.push_back(*jt);
      }

  for( it = deleteList.begin(); it != deleteList.end(); it++) 
    targetList.remove(*it);

  deleteList.clear();
#endif

  // printf ("In Grammar::editStringList: look for exclude strings \n");

  for( it = targetList.begin(); it != targetList.end(); it++) 
    for( jt = excludeList.begin(); jt != excludeList.end(); jt++) 
      {
	if( **jt == **it )  // if both strings contain the same thing
	  {
	    // printf ("Found a string to exclude string = %s ",excludeList[j].getRawString());
	    deleteList.push_back(*it);
	  }
      }

  deleteList.unique(); // really not necessary, but anyway ...

#if 0
     printf ("Number of elements in deleteList = %zu \n",deleteList.size());
     printf ("BEFORE: Number of elements in list = %zu \n",targetList.size());
#endif

  for( it = deleteList.begin(); it != deleteList.end(); it++) 
    targetList.remove( *it );

#if 0
     printf ("AFTER: Number of elements in list = %zu \n",targetList.size());
#endif

  // printf ("At bottom of Grammar::editStringList \n");
}

string
Grammar::buildStringFromLists ( GrammarTreeNode & node,
                                FunctionPointerType listFunction,
                                StringGeneratorFunctionPointerType stringGeneratorFunction )
{
  // This method builds a string (according to the parameter stringGeneratorFunction)
  // from the local lists of the current node, from all of its parents' subtree lists,
  // and from its own subtree lists

  // We use the method defined below which basically corresponds to the
  // code above (which is commented out)
  list<GrammarString *> sourceList= buildListFromLists(node, listFunction);
  list<GrammarString *>::iterator sourceListIterator;

  ROSE_ASSERT (node.token != NULL);

  string editStringMiddle;

  for( sourceListIterator = sourceList.begin();
       sourceListIterator != sourceList.end();
       sourceListIterator++ )
    {
      GrammarString & memberFunction = **sourceListIterator;
      string memberFunctionString = "// Start of memberFunctionString\n" + (memberFunction.*stringGeneratorFunction)() + "\n// End of memberFunctionString\n";
      editStringMiddle += memberFunctionString;
    }

  return editStringMiddle;
}


list<GrammarString *>
Grammar::buildListFromLists ( GrammarTreeNode & node,
			      FunctionPointerType listFunction )
  // This method builds a list from the local lists of the current node,
  // from all of its parents' subtree lists, and from its own subtree lists
{
  list<GrammarString *> includeList;
  list<GrammarString *> excludeList;
  ROSE_ASSERT (includeList.size() == 0);
  ROSE_ASSERT (excludeList.size() == 0);

  // Initialize with local node data
  includeList = (node.getToken().*listFunction)(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
  excludeList = (node.getToken().*listFunction)(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);
  
  // Now generate the additions to the lists from the parent node subtree lists
  // and the subtree lists of the current node
  if (node.parentTreeNode)
    generateStringListsFromSubtreeLists ( *(node.parentTreeNode), includeList, excludeList, listFunction );
  
  // Now edit the list to remove elements appearing within the exclude list
  editStringList ( includeList, excludeList );

  return includeList;
}


string
Grammar::buildStringForPrototypes ( GrammarTreeNode & node )
   {
  // This function adds in the source code specific to a node in the
  // tree that represents the hierachy of the grammer's implementation.

     ROSE_ASSERT (node.token != NULL);
     // BP : 10/09/2001 modified to provide addresses
     return buildStringFromLists ( node,
                                   &Terminal::getMemberFunctionPrototypeList,
                                   &GrammarString::getFunctionPrototypeString );
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForVariantFunctionSource         ( GrammarTreeNode & node )
   {
  // Every node in the grammar has a function that identifies it with a numerical value 
  // (e.g. SCOPE_STMT).

     string variantFunctionTemplateFileName   = "../Grammar/grammarVariantFunctionDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (variantFunctionTemplateFileName);

     returnString = GrammarString::copyEdit (returnString,"$MARKER",node.getToken().getTagName());

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::supportForBuildStringForIsClassNameFunctionSource     ( GrammarTreeNode & node, const StringUtility::FileWithLineNumbers& accumulationStringOrig )
   {
  // This function forms support for the Grammar::buildStringForIsClassNameFunctionSource function.
  // If a node is a part of the subtree represented by this grammar then is is by definition
  // of the type represented by the root of the subtree.

     StringUtility::FileWithLineNumbers accumulationString = accumulationStringOrig;

     string tempString = "               case ";

     tempString += node.getToken().getTagName();
     tempString += ":\n";
     accumulationString.push_back(StringUtility::StringWithLineNumber(tempString, "" /* "<supportForBuildStringForIsClassNameFunctionSource on " + node.getToken().getTagName() + ">" */, 1));

     list<GrammarTreeNode *>::iterator nodeListIterator;
     // Loop through the children 
     for( nodeListIterator = node.nodeList.begin();
	  nodeListIterator != node.nodeList.end();
	  nodeListIterator++)
        {
          ROSE_ASSERT ((*nodeListIterator)->token != NULL);
          ROSE_ASSERT ((*nodeListIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*nodeListIterator)->parentTreeNode != NULL);

          accumulationString = 
               supportForBuildStringForIsClassNameFunctionSource( (GrammarTreeNode &) **nodeListIterator, accumulationString);
        }

#if 0
     if (node.nodeList.size() > 0)
          printf ("In support looking for multiple cases (%s): accumulationString = %s \n",
               node.getName(),accumulationString);
#endif

     return accumulationString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForIsClassNameFunctionSource     ( GrammarTreeNode & node )
   {
  // This function builds the source code for a friend function, each class has
  // a member function that casts a pointer to any derived class to type represented by
  // this level of the subtree  (e.g. at the Statement level of the grammar the function would
  // cast a pointer to any type of function to the Statement base class type).

     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarIsClassNameFunctionDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     StringUtility::FileWithLineNumbers accumulationString = supportForBuildStringForIsClassNameFunctionSource(node,StringUtility::FileWithLineNumbers());
     returnString = GrammarString::copyEdit(returnString,"$ROOT_NODE_OF_GRAMMAR",getRootOfGrammar().getName());
     returnString = GrammarString::copyEdit(returnString,"$ACCUMULATION_STRING",accumulationString);
     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForNewAndDeleteOperatorSource ( GrammarTreeNode & node )
   {
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarNewDeleteOperatorMacros.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());
  // returnString = GrammarString::copyEdit(returnString,"$ROOT_NODE_OF_GRAMMAR",getRootOfGrammar().getName());
  // returnString = GrammarString::copyEdit(returnString,"$ACCUMULATION_STRING",accumulationString);

     return returnString;
   }

void
Grammar::buildNewAndDeleteOperators( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildNewAndDeleteOperators() \n");

  // printf ("Exiting at TOP of Grammar::buildSourceFiles() \n");
  // ROSE_ABORT();

  // printf ("At TOP of Grammar::buildNewAndDeleteOperators(): node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

     StringUtility::FileWithLineNumbers editString = buildStringForNewAndDeleteOperatorSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildNewAndDeleteOperators(**treeNodeIterator,outputFile);
        }
#endif
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForTraverseMemoryPoolSource ( GrammarTreeNode & node )
   {
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarTraverseMemoryPool.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());


     string classSpecificString;
     string classSpecificVisitorPatternString;
     string classSpecificMemoryUsageString;

     string className = node.getName();
#if 0
  // DQ (1/31/2006): commented out how that we have all the IR nodes in the memory pools.
     if (className == "SgTypeUnknown"          || className == "SgTypeChar"        || className == "SgTypeSignedChar"   || 
         className == "SgTypeUnsignedChar"     || className == "SgTypeShort"       || className == "SgTypeSignedShort"  || 
         className == "SgTypeUnsignedShort"    || className == "SgTypeSignedInt"   || className == "SgTypeUnsignedInt"  || 
         className == "SgTypeLong"             || className == "SgTypeSignedLong"  || className == "SgTypeUnsignedLong" ||
         className == "SgTypeVoid"             || className == "SgTypeGlobalVoid"  || className == "SgTypeWchar"        || 
         className == "SgTypeFloat"            || className == "SgTypeDouble"      || className == "SgTypeLongLong"     || 
         className == "SgTypeUnsignedLongLong" || className == "SgTypeLongDouble"  || className == "SgTypeString"       || 
         className == "SgTypeBool"             || className == "SgComplex"         || className == "SgTypeDefault"      || 
         className == "SgTypeEllipse"          || className == "SgUnknownMemberFunctionType" || 
         className == "SgPartialFunctionModifierType" || className == "SgNamedType")
        {
          classSpecificString               = "ROSE_ASSERT(builtin_type != NULL);\n         traversal.visit(builtin_type);";
          classSpecificVisitorPatternString = "ROSE_ASSERT(builtin_type != NULL);\n         builtin_type->executeVisitorMemberFunction(visitor);";
          classSpecificMemoryUsageString    = "count++;";
        }
       else
        {
          if (className == "SgTypeInt")
             {
               classSpecificString               = "for (int i=0; i < SgTypeInt::maxBitLength; i++)\n        {\n          ROSE_ASSERT(builtin_type[i] != NULL);\n          traversal.visit(builtin_type[i]);\n        }\n";
               classSpecificVisitorPatternString = "for (int i=0; i < SgTypeInt::maxBitLength; i++)\n        {\n          ROSE_ASSERT(builtin_type[i] != NULL);\n          builtin_type[i]->executeVisitorMemberFunction(visitor);\n        }\n";
               classSpecificMemoryUsageString    = "count += SgTypeInt::maxBitLength;";
             }
        }
#endif

  // printf ("node.getName() = %s classSpecificString = %s \n",node.getName(),classSpecificString.c_str());
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_USING_ROSE_VISIT",classSpecificString);
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_USING_VISITOR_PATTERN",classSpecificVisitorPatternString);
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_MEMORY_USED",classSpecificMemoryUsageString);

     return returnString;
   }

void
Grammar::buildTraverseMemoryPoolSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildNewAndDeleteOperators() \n");

  // printf ("Exiting at TOP of Grammar::buildSourceFiles() \n");
  // ROSE_ABORT();

  // printf ("At TOP of Grammar::buildNewAndDeleteOperators(): node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

     StringUtility::FileWithLineNumbers editString = buildStringForTraverseMemoryPoolSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildTraverseMemoryPoolSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringToTestPointerForContainmentInMemoryPoolSource ( GrammarTreeNode & node )
   {
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarTestPointerForContainmentInMemoryPool.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     string classSpecificString;
     string classSpecificVisitorPatternString;
     string classSpecificMemoryUsageString;

     string className = node.getName();
#if 0
  // DQ (1/31/2006): commented out how that we have all the IR nodes in the memory pools.
     if (className == "SgTypeUnknown"          || className == "SgTypeChar"        || className == "SgTypeSignedChar"   || 
         className == "SgTypeUnsignedChar"     || className == "SgTypeShort"       || className == "SgTypeSignedShort"  || 
         className == "SgTypeUnsignedShort"    || className == "SgTypeSignedInt"   || className == "SgTypeUnsignedInt"  || 
         className == "SgTypeLong"             || className == "SgTypeSignedLong"  || className == "SgTypeUnsignedLong" ||
         className == "SgTypeVoid"             || className == "SgTypeGlobalVoid"  || className == "SgTypeWchar"        || 
         className == "SgTypeFloat"            || className == "SgTypeDouble"      || className == "SgTypeLongLong"     || 
         className == "SgTypeUnsignedLongLong" || className == "SgTypeLongDouble"  || className == "SgTypeString"       || 
         className == "SgTypeBool"             || className == "SgComplex"         || className == "SgTypeDefault"      || 
         className == "SgTypeEllipse"          || className == "SgUnknownMemberFunctionType" || 
         className == "SgPartialFunctionModifierType" || className == "SgNamedType")
        {
          classSpecificMemoryUsageString  = "     if (found == false) \n";
          classSpecificMemoryUsageString += "        {\n";
          classSpecificMemoryUsageString += "          found = (this == builtin_type);\n";
          classSpecificMemoryUsageString += "        }\n\n";
        }
       else
        {
          if (className == "SgTypeInt")
             {
               classSpecificMemoryUsageString  = "     if (found == false) \n";
               classSpecificMemoryUsageString += "        {\n";
               classSpecificMemoryUsageString += "          int i=0;\n";
               classSpecificMemoryUsageString += "          while (found == false && i < SgTypeInt::maxBitLength)\n";
               classSpecificMemoryUsageString += "               found = (this == builtin_type[i++]);\n";
               classSpecificMemoryUsageString += "        }\n\n";
             }
        }
#endif

  // printf ("node.getName() = %s classSpecificString = %s \n",node.getName(),classSpecificString.c_str());
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_MEMORY_USED",classSpecificMemoryUsageString);

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForCheckingIfDataMembersAreInMemoryPoolSource ( GrammarTreeNode & node )
   {
  // DQ & JH (1/17/2006): Added support for testing data members pointers if they point to IR nodes

     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarCheckingIfDataMembersAreInMemoryPool.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     ROSE_ASSERT(node.token != NULL);
     string dataMemberSpecificString = node.token->buildPointerInMemoryPoolCheck();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

  // Add the associated virtual function to test of a pointer is pointing at an IR node located in the memory pool
     StringUtility::FileWithLineNumbers isInMemoryPoolTestString = buildStringToTestPointerForContainmentInMemoryPoolSource(node);
     returnString = GrammarString::copyEdit(returnString,"$ASSOCIATED_MEMORY_POOL_TEST",isInMemoryPoolTestString);

     return returnString;
   }

void
Grammar::buildStringForCheckingIfDataMembersAreInMemoryPoolSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForCheckingIfDataMembersAreInMemoryPoolSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildStringForCheckingIfDataMembersAreInMemoryPoolSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForReturnDataMemberPointersSource ( GrammarTreeNode & node )
   {
  // DQ & JH (1/17/2006): Added support for testing data members pointers if they point to IR nodes

     //AS Look at this one to see how the code in buildStringToTestPointerForContainmentInMemoryPoolSource is called
     //to generate code for checking the memory pool.
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarReturnDataMemberPointers.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     ROSE_ASSERT(node.token != NULL);
     string dataMemberSpecificString = node.token->buildReturnDataMemberPointers();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

  // Add the associated virtual function to test of a pointer is pointing at an IR node located in the memory pool
  // Commented out because it is not relevant to the returning of data member pointers to IR nodes
  //   string isInMemoryPoolTestString = buildStringToReturnDataMemberPointersSource(node);
   //  returnString = GrammarString::copyEdit(returnString,"$ASSOCIATED_MEMORY_POOL_TEST",isInMemoryPoolTestString.c_str());

     return returnString;
   }

void
Grammar::buildStringForReturnDataMemberPointersSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForReturnDataMemberPointersSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildStringForReturnDataMemberPointersSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForReturnDataMemberReferenceToPointersSource ( GrammarTreeNode & node )
   {
  // DQ & JH (1/17/2006): Added support for testing data members pointers if they point to IR nodes

     //AS Look at this one to see how the code in buildStringToTestPointerForContainmentInMemoryPoolSource is called
     //to generate code for checking the memory pool.
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarReturnDataMemberReferenceToPointers.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     ROSE_ASSERT(node.token != NULL);
     string dataMemberSpecificString = node.token->buildReturnDataMemberReferenceToPointers();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

  // Add the associated virtual function to test of a pointer is pointing at an IR node located in the memory pool
  // Commented out because it is not relevant to the returning of data member pointers to IR nodes
  //   string isInMemoryPoolTestString = buildStringToReturnDataMemberPointersSource(node);
   //  returnString = GrammarString::copyEdit(returnString,"$ASSOCIATED_MEMORY_POOL_TEST",isInMemoryPoolTestString.c_str());

     return returnString;
   }

void
Grammar::buildStringForReturnDataMemberReferenceToPointersSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForReturnDataMemberReferenceToPointersSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildStringForReturnDataMemberReferenceToPointersSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForGetChildIndexSource ( GrammarTreeNode & node )
   {
  // DQ (3/7/2007): Added support for getting the index position associated with the list of IR nodes children in any IR node.

     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarGetChildIndex.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     ROSE_ASSERT(node.token != NULL);
     string dataMemberSpecificString = node.token->buildChildIndex();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

     return returnString;
   }

void
Grammar::buildStringForGetChildIndexSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForGetChildIndexSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildStringForGetChildIndexSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForSource ( GrammarTreeNode & node )
   {
  // This function adds in the source code specific to a node in the
  // tree that represents the hierachy of the grammer's implementation.

     ROSE_ASSERT (node.token != NULL);

  // BP : 10/09/2001, modified to provide addresses
     string beginString = buildStringFromLists ( node, 
                                       &Terminal::getMemberFunctionSourceList, 
                                       &GrammarString::getFunctionPrototypeString );

     StringUtility::FileWithLineNumbers variantFunctionDefinition     = buildStringForVariantFunctionSource      (node);
     StringUtility::FileWithLineNumbers isClassnameFunctionDefinition = buildStringForIsClassNameFunctionSource  (node);

  // DQ (12/23/2005): Move this generated code to seperate source file
  // char* copyMemberFunction            = buildCopyMemberFunctionSource            (node);

  // DQ (12/23/2005): Move this generated code to seperate source file
  // DQ (9/21/2005): Added support for new and delete operators 
  // char* newAndDeleteOperatorSource    = buildStringForNewAndDeleteOperatorSource (node);

#if 0
     if (copyMemberFunction == 0)
        {
       // Build trivial length C string and place a null terminator at the end
          copyMemberFunction = new char[1];
          *copyMemberFunction = '\0';
        }
#endif

     StringUtility::FileWithLineNumbers returnString = StringUtility::FileWithLineNumbers(1, StringUtility::StringWithLineNumber(beginString, "" /* "<buildStringForSource " + node.getToken().getName() + ">" */, 1)) + variantFunctionDefinition + isClassnameFunctionDefinition;

  // printf ("In Grammar::buildStringForSource(node): returnString = \n %s \n",returnString);

     return returnString;
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForDataDeclaration ( GrammarTreeNode & node )
{
  // This function builds the string representing the declaration 
  // of data variables (all of them) in a class.
  ROSE_ASSERT (node.token != NULL);
  // BP : 10/09/2001, modified to provide addresses
  string returnString = buildStringFromLists ( node, 
					      &Terminal::getMemberDataPrototypeList, 
					      &GrammarString::getDataPrototypeString );
  return StringUtility::FileWithLineNumbers(1, StringUtility::StringWithLineNumber(returnString, "" /* "<buildStringForDataDeclaration>" */, 1));
}


StringUtility::FileWithLineNumbers
Grammar::buildStringForDataAccessFunctionDeclaration ( GrammarTreeNode & node )
   {
  // This function builds the strings representing the data access function prototypes
     ROSE_ASSERT (node.token != NULL);

  // Save the original setting
  // bool originalSetting = node.getIncludeInitializerInDataStrings();
  
  // Mark that the formation of data strings should include their initializers
  // (e.g.  int x = 0; where the " = 0" is the initializer).  Sometimes we need these
  // and sometimes it would be an error (in C++) to generate code that included them.
  // node.setIncludeInitializerInDataStrings (includeInitializer);

  // BP : 10/09/2001, modified to provide adddress
     list<GrammarString *> dataMemberList = buildListFromLists ( node, &Terminal::getMemberDataPrototypeList );

     StringUtility::FileWithLineNumbers returnString;
     list<GrammarString *>::iterator dataMemberIterator;

     for( dataMemberIterator = dataMemberList.begin();
          dataMemberIterator != dataMemberList.end();
          dataMemberIterator++ )
        {
          GrammarString & data = **dataMemberIterator;
	  StringUtility::FileWithLineNumbers tempString(1, StringUtility::StringWithLineNumber(data.getDataAccessFunctionPrototypeString(), "" /* "<getDataAccessFunctionPrototypeString>" */, 1));
          returnString += tempString;
        }

     return returnString;
   }


bool
Grammar::buildConstructorParameterList ( GrammarTreeNode & node, list<GrammarString *> & constructorParameterList, ConstructParamEnum config )
   {
  // This function is called by the buildConstructorParameterListString(node) function
  // and builds the list of parameters that are used by a constructor.
  // The data exclusiion mechanism implies that excluded data within parents will not appear
  // in the constructor parameters of child grammar elements.
  
  // The input list shoud be empty
     ROSE_ASSERT(constructorParameterList.size() == 0);
  
  // Now build the constructor using the data specified at the node and its parents!
  // Use an exclusion mechanism to exclude data before being used within the 
  // constructor parameter list (use flag in GrammarString).
  
     list<GrammarString *> includeList;
     list<GrammarString *> excludeList;

  // now generate the additions to the lists from the parent node subtree lists
  // BP : 10/09/2001, modified to provide address
     generateStringListsFromLocalLists ( node, includeList, excludeList, &Terminal::getMemberDataPrototypeList );
  
  // Now edit the list to remove elements appearing within the exclude list
     editStringList ( includeList, excludeList );
  
     list<GrammarString *>::iterator gIt;

     bool complete = true;

     for( gIt = includeList.begin(); gIt != includeList.end(); gIt++)
        {
       // BP : 10/26/2001, tried running with Sun CC and gave the correct results ie generated source correctly
          GrammarString *memberFunctionCopy= *gIt;
          ROSE_ASSERT (memberFunctionCopy != NULL);
#if 0
       // QY 11/9/04 add to constructor parameter list only if matches
       // if(memberFunctionCopy->getIsInConstructorParameterList() == TRUE)
          ConstructParamEnum cur = memberFunctionCopy->getIsInConstructorParameterList();
           if (cur & config )
                constructorParameterList.push_back(memberFunctionCopy);
           else if (cur > config)
                complete = false;
#else
       // DQ (11/7/2006): Rewritten to remove wrap logic (overly complex)
       // if (memberFunctionCopy->getIsInConstructorParameterList() == TRUE)
          if (memberFunctionCopy->getIsInConstructorParameterList() == CONSTRUCTOR_PARAMETER)
             {
                constructorParameterList.push_back(memberFunctionCopy);
             }
#endif
        }

     return complete;
   }

string
Grammar::buildConstructorParameterListString ( GrammarTreeNode & node, bool withInitializers, bool withTypes, ConstructParamEnum config, bool* complete )
   {
  // This function returns the string used to build the parameters within the constructor.  
     int i = 0;
     list<GrammarString *> constructorParameterList;
     list<GrammarString *>::iterator stringListIterator;

     bool r = buildConstructorParameterList (node,constructorParameterList, config);
     if (complete != 0)
          *complete = r;
  
#if ROSE_DEBUG > 0
     for( stringListIterator = constructorParameterList.begin();
          stringListIterator != constructorParameterList.end();
          stringListIterator++ )
        {
          GrammarString & constructorParameter = **stringListIterator;
          string tempConstructorParameterString = constructorParameter.getFunctionNameString();
          printf ("tempConstructorParameterString = %s \n",tempConstructorParameterString.c_str());
        }
#endif
  
  // Build the parameter string for the constructor
     string constructorParameterString;
  
     int listSize = constructorParameterList.size();
  // Put the constructor paramteres into the parameter string
     for( stringListIterator = constructorParameterList.begin();
          stringListIterator != constructorParameterList.end();
          stringListIterator++ )
        {
          GrammarString & constructorParameter = **stringListIterator;
       // char* tempConstructorParameterString = constructorParameter.getConstructorParameterString();
          string tempConstructorParameterString = "";
          if (withTypes == TRUE)
             {
               if (withInitializers == TRUE)
                    tempConstructorParameterString = constructorParameter.getConstructorPrototypeParameterString();
                 else
                    tempConstructorParameterString = constructorParameter.getConstructorSourceParameterString();
             }
            else
             {
               ROSE_ASSERT (withInitializers == FALSE);
               tempConstructorParameterString = constructorParameter.getBaseClassConstructorSourceParameterString();
             }
      
       // printf ("building constructorParameterString tempConstructorParameterString = %s \n",tempConstructorParameterString);

          constructorParameterString += tempConstructorParameterString;

       // If there is another parameter to add in then separate them with a ","
          if (i < listSize-1)
             {
	         // BP : 10/24/2001, delete extra memory
               constructorParameterString += ", ";
             }
          i++;
        }

     return constructorParameterString;
   }


StringUtility::FileWithLineNumbers
Grammar::buildDataMemberVariableDeclarations ( GrammarTreeNode & node )
   {
  // This function builds a single string containing:
  //    1) Data prototype  (e.g. "int data; $Data* someSageData;")

  // This builds the data declaration (the easy part) e.g. "int dataField; \n char* charField; \n"
  // Spaces and CR's have been added to simplify the final formatting
     StringUtility::FileWithLineNumbers result;
     result.push_back(StringUtility::StringWithLineNumber("    protected:", "" /* "<buildDataMemberVariableDeclarations on " + node.getToken().getName() + ">" */, 1));
     result += buildStringForDataDeclaration(node);

     return result;
   }

StringUtility::FileWithLineNumbers
Grammar::buildMemberAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node )
   {
  // This function builds a single string containing:
  //    1) Data Access function prototypes (e.g. "void set_data( int data ); int get_data(void); ..." )
  //    2) Constructor prototype (e.g. "$CLASSNAME ( data = 0, $Data* someSageData = NULL );" )

     StringUtility::FileWithLineNumbers dataAccessFunctionPrototypeString = buildStringForDataAccessFunctionDeclaration(node);

  // printf ("dataAccessFunctionPrototypeString = \n%s\n",dataAccessFunctionPrototypeString.c_str());

     string className = node.getName();

  // Build the constructor prototype and then edit the names!
     string destructorPrototype  = "\n     public: \n         virtual ~" + string(className) +  "();\n";

     if (node.getToken().generateDestructor() == TRUE)
         dataAccessFunctionPrototypeString.push_back(StringUtility::StringWithLineNumber(destructorPrototype, "" /* "<destructor>" */, 1));

     // Now build the constructor and put in the constructorParameterString
     if (node.getToken().generateConstructor() == TRUE)
        {
          bool complete = false;
          ConstructParamEnum cur = (ConstructParamEnum)1;
          string constructorPrototype = "\n     public: \n"; 
#if 1
          bool withInitializers = TRUE;
#else
       // DQ (11/7/2006): Let's try to force use of initializers! So that we 
       // can also define constructors that don't take a Sg_File_Info object.
          bool withInitializers = false;
#endif
          bool withTypes        = TRUE;
#if 0
          while (!complete && (cur < CONSTRUCTOR_PARAMETER)) {
             string constructorParameterString = 
                buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
             constructorPrototype = constructorPrototype + "         " + 
                string(className) + "(" + constructorParameterString + "); \n";
             cur = (ConstructParamEnum)(cur << 1);
             withInitializers = false;
          }
#else
       // Get the SgLocatedNode so that we can set the data member as not being a constructor 
       // parameter so that we can reuse the same code generation source code.
          GrammarTreeNode* parentNode = getNamedNode ( node, "SgLocatedNode" );
          if (parentNode != NULL)
             {
               GrammarString* returnValue = getNamedDataMember ( *parentNode, "startOfConstruct" );
               ROSE_ASSERT(returnValue != NULL);

            // DQ (11/7/2006): Mark it temporarily as NOT a constructor parameter.
               string defaultInitializer = returnValue->getDefaultInitializerString();
               returnValue->defaultInitializerString = "";

               string constructorParameterString_1 = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
               constructorPrototype = constructorPrototype + "         " + string(className) + "(" + constructorParameterString_1 + "); \n";
               withInitializers = false;

            // DQ (11/7/2006): Mark it temporarily as NOT a constructor parameter.
               returnValue->isInConstructorParameterList = NO_CONSTRUCTOR_PARAMETER;

               string constructorParameterString_2 = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
               constructorPrototype = constructorPrototype + "         " + string(className) + "(" + constructorParameterString_2 + "); \n";

            // DQ (11/7/2006): Turn it back on as a constructor parameter (and reset the defaultInitializerString)
               returnValue->isInConstructorParameterList = CONSTRUCTOR_PARAMETER;
               returnValue->defaultInitializerString = defaultInitializer;

             }
            else
             {
            // If not a SgLocatedNode then output the normal constructor prototype (with all the default arguments.
               string constructorParameterString = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
               constructorPrototype = constructorPrototype + "         " + string(className) + "(" + constructorParameterString + "); \n";
               withInitializers = false;
             }
#endif
          dataAccessFunctionPrototypeString.push_back(StringUtility::StringWithLineNumber(constructorPrototype, "" /* "<constructor>" */, 1));
        }

     return dataAccessFunctionPrototypeString;
   }

#if 0
string
Grammar::buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node )
   {
  // This function builds a single string containing:
  //    1) Data prototype  (e.g. "int data; $Data* someSageData;")
  //    2) Data Access function prototypes (e.g. "void set_data( int data ); int get_data(void); ..." )
  //    3) Constructor prototype (e.g. "$CLASSNAME ( data = 0, $Data* someSageData = NULL );" )

  // This builds the data declaration (the easy part) e.g. "int dataField; \n char* charField; \n"
  // Spaces and CR's have been added to simplify the final formatting
     string privateDeclarationString = "\n     protected: \n          ";
     string dataDeclarationString = buildStringForDataDeclaration (node);
     dataDeclarationString = privateDeclarationString + dataDeclarationString + "\n";

  // printf ("dataDeclarationString = \n%s\n",dataDeclarationString.c_str());

     string dataAccessFunctionPrototypeString = buildStringForDataAccessFunctionDeclaration(node);

  // DQ (3/24/2006): Swap the order of these os that the data members are at the top and the 
  // member functions are at the bottom.
  // dataDeclarationString = dataAccessFunctionPrototypeString + dataDeclarationString + "\n";
     dataDeclarationString = dataDeclarationString + "$OPEN_COMMENT_GROUP\n" + dataAccessFunctionPrototypeString + "\n";

  // printf ("dataAccessFunctionPrototypeString = \n%s\n",dataAccessFunctionPrototypeString.c_str());

     char* className = node.getName();

  // Build the constructor prototype and then edit the names!
     string destructorPrototype  = "\n     public: \n         virtual ~" + string(className) +  "();\n";

     if (node.getToken().generateDestructor() == TRUE)
         dataDeclarationString = dataDeclarationString + destructorPrototype;

     // Now build the constructor and put in the constructorParameterString
     if (node.getToken().generateConstructor() == TRUE) {
         bool complete = false;
         ConstructParamEnum cur = (ConstructParamEnum)1;
         string constructorPrototype = "\n     public: \n"; 
         bool withInitializers = TRUE;
         bool withTypes        = TRUE;
         while (!complete && (cur < CONSTRUCTOR_PARAMETER)) {
            string constructorParameterString = 
               buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
            constructorPrototype = constructorPrototype + "         " + 
               string(className) + "(" + constructorParameterString + "); \n";
            cur = (ConstructParamEnum)(cur << 1);
            withInitializers = false;
         }
         dataDeclarationString = dataDeclarationString + constructorPrototype;
      }

     return dataDeclarationString;
   }
#endif

StringUtility::FileWithLineNumbers
Grammar::buildConstructor ( GrammarTreeNode & node )
   {
  // Build the constructors for each class
  // Example:
  // /* this is the generated constructor */
  // ClassDeclaration::ClassDeclaration 
  //    ( File_Info* info , Name name, int class_type, ClassType* type, ClassDefinition* definition)
  //    : DeclarationStatement(info)
  //    {
  //      p_name = name;
  //      p_class_type = class_type;
  //      p_type = type;
  //      p_definition = definition;
  //   /* now a call to the user defined intialization function */
  //      post_construction_initialization();
  //    }

     string className = node.getName();

     StringUtility::FileWithLineNumbers returnString;

     if (node.getToken().generateDestructor() == TRUE)
        {
       // Build the string representing the constructor text (with macro variables)
	  string destructorTemplateFileName  = "../Grammar/grammarDestructorDefinitionMacros.macro";
	  StringUtility::FileWithLineNumbers destructorSourceCodeTemplate = readFileWithPos (destructorTemplateFileName);
	 
       // edit the string to customize it for this node in the grammar!
	  StringUtility::FileWithLineNumbers destructorSource = GrammarString::copyEdit (destructorSourceCodeTemplate,"$CLASSNAME",className);

       // char* constructorFunctionBody = GrammarString::stringDuplicate(node.getToken().buildConstructorBody(withInitializers, config).c_str());
       // constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_BODY",constructorFunctionBody);
       // delete [] constructorFunctionBody;
	 
       // For now make the descructor function body empty
       // AJ (10/27/2004) - Added the destructor body generation
       // char* destructorFunctionBody = "";
          string destructorFunctionBody = node.getToken().buildDestructorBody();
          destructorSource = GrammarString::copyEdit (destructorSource,"$DESTRUCTOR_BODY",destructorFunctionBody);

       // printf ("destructorSource = \n%s\n",destructorSource);

          returnString.insert(returnString.end(), destructorSource.begin(), destructorSource.end());
        }

     if (node.getToken().generateConstructor() == TRUE)
        {
          string constructorTemplateFileName = "../Grammar/grammarConstructorDefinitionMacros.macro";
	  StringUtility::FileWithLineNumbers constructorSourceCodeTemplate = readFileWithPos (constructorTemplateFileName);

          bool complete  = false;
          ConstructParamEnum config = (ConstructParamEnum)1;
          int i = 1;
          if  (node.getToken().getBuildDefaultConstructor())
             {
               config = NO_CONSTRUCTOR_PARAMETER;
               i = 0;
             }

       // DQ (5/22/2006): Why do we have such a complex for loop?
          for ( ; !complete && config < CONSTRUCTOR_PARAMETER; config = (ConstructParamEnum)(1 << i++))
             {
	       StringUtility::FileWithLineNumbers constructorSource = constructorSourceCodeTemplate;
               if (node.hasParent() == TRUE)
                  {
                    string parentClassName = node.getParentName();
                 // printf ("In Grammar::buildConstructor(): parentClassName = %s \n",parentClassName);
                 // printf ("Calling base class default constructor (should call paramtererized version) \n");

                    string baseClassParameterString;
                    if (node.hasParent() == TRUE)
                       {
                         bool withInitializers = FALSE;
                         bool withTypes        = FALSE;
                         baseClassParameterString = buildConstructorParameterListString (node.getParent(),withInitializers,withTypes, config);
                       }
                    string preInitializationString = parentClassName + "($BASECLASS_PARAMETERS)";
                    preInitializationString = ": " + preInitializationString;
                    preInitializationString = GrammarString::copyEdit (preInitializationString,"$BASECLASS_PARAMETERS",baseClassParameterString);
                    constructorSource = GrammarString::copyEdit (constructorSource,"$PRE_INITIALIZATION_LIST",preInitializationString);
                  }
                 else
                  {
                    constructorSource = GrammarString::copyEdit (constructorSource,"$PRE_INITIALIZATION_LIST","");
                  }

               bool withInitializers         = FALSE;
               bool withTypes                = TRUE;
               string constructorParameterString = buildConstructorParameterListString (node,withInitializers,withTypes,config,&complete);
               constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_PARAMETER_LIST",constructorParameterString);
               constructorSource = GrammarString::copyEdit (constructorSource,"$CLASSNAME",className);

               if (config == NO_CONSTRUCTOR_PARAMETER)
                  {
                    constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_BODY","");
                  }
                 else
                  {
                    string constructorFunctionBody = node.getToken().buildConstructorBody(withInitializers, config);
                    constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_BODY",constructorFunctionBody);
                  }

               returnString.insert(returnString.end(), constructorSource.begin(), constructorSource.end());
             }
        }

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildCopyMemberFunctionSource ( GrammarTreeNode & node )
   {
  // This function builds the copy function within each class defined by the grammar
  // return node.getToken().buildCopyMemberFunctionSource();

  // char* returnString = node.getToken().buildCopyMemberFunctionSource().c_str();
  // char* returnString = GrammarString::stringDuplicate(node.getToken().buildCopyMemberFunctionSource().c_str());
     StringUtility::FileWithLineNumbers returnString = node.getToken().buildCopyMemberFunctionSource();

  // printf ("In Grammar::buildCopyMemberFunctionSource(): returnCppString length = %ld \n",returnCppString.length());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());
     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_X_MARKER_","");

  // printf ("In Grammar::buildCopyMemberFunctionSource(node): returnString = \n%s \n",returnString);

     return returnString;
   }

void
Grammar::buildCopyMemberFunctions ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildCopyMemberFunctions() \n");
  // printf ("At TOP of Grammar::buildCopyMemberFunctions(): node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

     StringUtility::FileWithLineNumbers editString = buildCopyMemberFunctionSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildCopyMemberFunctions(**treeNodeIterator,outputFile);
        }
#endif
   }

void
Grammar::buildGrammarClassDeclaration ( StringUtility::FileWithLineNumbers & outputFile )
   {
  // Each grammar has a class declaration (separate from the elements 
  // of the grammar). This function builds that C++ class declaration which
  // represents the grammar's implementation.  The basis for the declaration is
  // found in the "../Grammar/grammarMainClassDeclatationMacros.macro" file.

     printf ("Dead code never called ... \n");
     ROSE_ASSERT(false);

     string marker    = "MEMBER_FUNCTION_DECLARATIONS";
     string fileName  = "../Grammar/grammarMainClassDeclatationMacros.macro";

     StringUtility::FileWithLineNumbers headerBeforeInsertion = buildHeaderStringBeforeMarker(marker,fileName);
     StringUtility::FileWithLineNumbers headerAfterInsertion  = buildHeaderStringAfterMarker (marker,fileName);

     StringUtility::FileWithLineNumbers finalOutputString = headerBeforeInsertion + headerAfterInsertion;

  // The name of the class representing the grammar should be the name of the grammar (not the prefix)!
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$CLASSNAME",getGrammarName());
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$GRAMMAR_BASECLASS",grammarNameBaseClass);

     outputFile += finalOutputString;
   }

void
Grammar::buildGrammarClassSourceCode ( StringUtility::FileWithLineNumbers & outputFile )
   {
     printf ("This should not be called! \n");
     ROSE_ASSERT(false);

     string fileName  = "../Grammar/grammarMainClassSourceCodeMacros.macro";

  // grammarMainClassParseFunctionSourceCode
     string parseFunctionFileName = "../Grammar/grammarMainClassParseFunctionSourceCode.macro";

     StringUtility::FileWithLineNumbers sourceCodeTemplate = readFileWithPos (fileName);
     StringUtility::FileWithLineNumbers parseFunctionSourceCodeTemplate = readFileWithPos (parseFunctionFileName);

#if 0
     char* finalOutputString = new char[1];
     finalOutputString[0] = '\0';

     // BP : 10/24/2001, keep track of memory to free
     char *tmpPtr = finalOutputString;
     finalOutputString = GrammarString::stringConcatenate (finalOutputString,sourceCodeTemplate);
     delete [] tmpPtr;
  // Only include the parse function source code if we are building a non-root grammar (C++ is the root grammar)
     if (!isRootGrammar() == TRUE)
       {
	 tmpPtr = finalOutputString;
	 finalOutputString = GrammarString::stringConcatenate (finalOutputString,parseFunctionSourceCodeTemplate);
	 delete [] tmpPtr;
       }
#else
     // BP : 10/25/2001, rewrote to do only one alloc
     StringUtility::FileWithLineNumbers finalOutputString = sourceCodeTemplate;
     if(!isRootGrammar()==TRUE)
       finalOutputString.insert(finalOutputString.end(), parseFunctionSourceCodeTemplate.begin(), parseFunctionSourceCodeTemplate.end());
#endif

     finalOutputString = GrammarString::copyEdit (finalOutputString,"$CLASSNAME",getGrammarName());
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$GRAMMAR_BASECLASS",grammarNameBaseClass);
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());

#if 0
     printf ("finalOutputString = %s \n",finalOutputString);
     ROSE_ABORT();
#else
     outputFile += finalOutputString;
#endif
   }

#if 0  // BP : 10/10/2001, moved to GrammarString class
char*
Grammar::GrammarString::stringConcatenate ( const char* target, const char* endingString )
   {
     ROSE_ASSERT (target != NULL);
     ROSE_ASSERT (endingString != NULL);

  // int returnStringLength = strlen(target) + strlen(endingString);
     int targetStringLength = (target       != NULL) ? strlen(target)       : 0;
     int endingStringLength = (endingString != NULL) ? strlen(endingString) : 0;
     int returnStringLength = targetStringLength + endingStringLength;

     char* returnString = new char [returnStringLength + 1];
     ROSE_ASSERT (returnString != NULL);

     returnString[0] = '\0';

  // Call the standard C string concat function
     strcat(returnString,target);
     strcat(returnString,endingString);

#if 0
     delete target;
     target = NULL;
#endif

     return returnString;
   }
#endif

string
Grammar::getDerivedClassDeclaration ( GrammarTreeNode & node )
   {
     string derivedClassString;

  // printf ("EDIT className (%s) during copy \n",className);
     if (node.parentTreeNode != NULL)
       derivedClassString = string(": public ") + node.parentTreeNode->getName();
     
  // printf ("EDIT parentClassName (%s) durring copy \n",parentClassName);

     return derivedClassString;
   }


StringUtility::FileWithLineNumbers
Grammar::buildHeaderStringBeforeMarker( const string& marker, const string& fileName )
   {
     string headerFileInsertionSeparator = marker;
     StringUtility::FileWithLineNumbers headerFileTemplate = readFileWithPos (fileName);
     
     for (unsigned int i = 0; i < headerFileTemplate.size(); ++i) {
       std::string::size_type pos = headerFileTemplate[i].str.find(headerFileInsertionSeparator);
       if (pos != string::npos) {
	 headerFileTemplate.erase(headerFileTemplate.begin() + i + 1, headerFileTemplate.end());
	 headerFileTemplate[i].str = headerFileTemplate[i].str.substr(0, pos);
 // headerFileTemplate[i].filename += " before marker " + marker;
	 headerFileTemplate.insert(headerFileTemplate.begin(), StringUtility::StringWithLineNumber("", "" /* "<before output of buildHeaderStringBeforeMarker " + marker + " " + fileName + ">" */, 1));
	 headerFileTemplate.insert(headerFileTemplate.end(), StringUtility::StringWithLineNumber("", "" /* "<after output of buildHeaderStringBeforeMarker " + marker + " " + fileName + ">" */, 1));
	 return headerFileTemplate;
       }
     }
     ROSE_ASSERT (!"Marker not found");
   }

StringUtility::FileWithLineNumbers
Grammar::buildHeaderStringAfterMarker( const string& marker, const string& fileName )
   {
     string headerFileInsertionSeparator = marker;
  // char* headerFileTemplate = readFile ("../Grammar/grammarClassDeclatationMacros.macro");
     StringUtility::FileWithLineNumbers headerFileTemplate = readFileWithPos (fileName);

     for (unsigned int i = 0; i < headerFileTemplate.size(); ++i) {
       std::string::size_type pos = headerFileTemplate[i].str.find(headerFileInsertionSeparator);
       if (pos != string::npos) {
	 headerFileTemplate.erase(headerFileTemplate.begin(), headerFileTemplate.begin() + i);
	 headerFileTemplate[0].str = headerFileTemplate[0].str.substr(pos + headerFileInsertionSeparator.size());
 // headerFileTemplate[0].filename += " after marker " + marker;
	 headerFileTemplate.insert(headerFileTemplate.begin(), StringUtility::StringWithLineNumber("", "" /* "<before output of buildHeaderStringAfterMarker " + marker + " " + fileName + ">" */, 1));
	 headerFileTemplate.insert(headerFileTemplate.end(), StringUtility::StringWithLineNumber("", "" /* "<after output of buildHeaderStringAfterMarker " + marker + " " + fileName + ">" */, 1));
	 return headerFileTemplate;
       }
     }
     ROSE_ASSERT (!"Marker not found");
   }

void
Grammar::buildHeaderFiles( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     string marker   = "MEMBER_FUNCTION_DECLARATIONS";
     string fileName = "../Grammar/grammarClassDeclatationMacros.macro";

     StringUtility::FileWithLineNumbers headerBeforeInsertion = buildHeaderStringBeforeMarker(marker,fileName);
     StringUtility::FileWithLineNumbers headerAfterInsertion  = buildHeaderStringAfterMarker (marker,fileName);

#if 1
  // DQ (3/24/2006): Have this be generated from the CommonCode.code file
  // so that we can better control how the documentation is done.
  // Here is where the virtual copy function is added to the header file!
     StringUtility::FileWithLineNumbers copyString = node.getToken().buildCopyMemberFunctionHeader();

  // printf ("TEMP String Value: copyString = \n%s\n",copyString);
  // ROSE_ASSERT(false);

     headerBeforeInsertion += copyString;
#endif

     list<Constraint *>::const_iterator it;
     int i = 0;
     for ( it = node.getToken().constraintList.begin(); it != node.getToken().constraintList.end(); it++ )
        {
          printf ("node.getName() = %s \n",node.getName().c_str());
          printf ("     node.getToken().constraintList[%d].name = %s \n",
               i, (*it)->getName().c_str());
          printf ("     getConstraintString() = '%s' \n",
               (*it)->getConstraintString().c_str());
          i++;
        }

  // Edit the $CLASSNAME
     string className = node.getName();

     string derivedClassString = getDerivedClassDeclaration(node);

  // Likely this must happen here since the substitution for BASECLASS is different 
  // within the calls to GrammarString::copyEdit() now centralized in editSubstitution()
  // This should be fixed!
     StringUtility::FileWithLineNumbers editStringStart = GrammarString::copyEdit (headerBeforeInsertion,"$BASECLASS",derivedClassString);

  // calls to GrammarString::copyEdit() now centralized in editSubstitution()
  // BP : 10/24/2001, keep track of memory being freed
     editStringStart = GrammarString::copyEdit (editStringStart,"$CLASSNAME",className);
     StringUtility::FileWithLineNumbers editStringEnd   = GrammarString::copyEdit (headerAfterInsertion,"$CLASSNAME",className);

  // int editedStringMiddleLength = 0;
     StringUtility::FileWithLineNumbers editedStringMiddle;

  // Each of these functions should return a null terminated string
  // (even if there are no code strings associated with this node).

     StringUtility::FileWithLineNumbers editStringMiddleNodeMemberFunctions(1, StringUtility::StringWithLineNumber(buildStringForPrototypes(node), "" /* "<buildStringForPrototypes " + node.getToken().getName() + ">" */, 1));

     editedStringMiddle += editStringMiddleNodeMemberFunctions;

  // DQ (3/24/2006): Separated these functions so that we could position the data member 
  // variable declaration at the base of the class and the generated access functions at 
  // the top.  This permist us to present the documentation better using Doxygen.
  // string buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node );

  // Using the data prototypes we also want to build the constructor 
  // call (with the data types and variables as prototypes)
  // string editStringMiddleNodeData = buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype (node);
     StringUtility::FileWithLineNumbers editStringMiddleNodeMemberFunctionsPrototypes = buildMemberAccessFunctionPrototypesAndConstuctorPrototype(node);

  // DQ (3/25/2006): Change the order back because the *.code files often define enum and typdefs that are 
  // required (even though I thought that the order of appearance of such things was unimporant in C++ classes).
  // DQ (3/24/2006): Output the generated code before the code in the *.code files that ROSETTA uses.
  // editedStringMiddle = GrammarString::stringConcatenate (editedStringMiddle,editStringMiddleNodeData.c_str());
  // editedStringMiddle = GrammarString::stringConcatenate (editStringMiddleNodeMemberFunctionsPrototypes.c_str(),editedStringMiddle);
     editedStringMiddle += editStringMiddleNodeMemberFunctionsPrototypes;

  // DQ (3/24/2006): Add the data members to the end of the class in the generated code.
     StringUtility::FileWithLineNumbers editStringMiddleNodeData = buildDataMemberVariableDeclarations(node);
     editedStringMiddle += editStringMiddleNodeData;

#if 0
     printf ("TEMP String Value: editedStringMiddle = \n%s\n",editedStringMiddle);
     ROSE_ASSERT(false);
#endif

  // printf ("editStringMiddleNodeMemberFunctions = %s \n",editStringMiddleNodeMemberFunctions);
  // char* editStringForParserPrototype = buildParserPrototype (node);
  // ROSE_ASSERT (editStringForParserPrototype != NULL);
     
  // char *tmpString = NULL;
  // printf ("editStringMiddleNodeMemberFunctions = %s \n",editStringMiddleNodeMemberFunctions);
  // printf ("editStringMiddleSubTreeCodeMemberFunctions = %s \n",editStringMiddleSubTreeCodeMemberFunctions);
  // printf ("editStringMiddleNodeData = %s \n",editStringMiddleNodeData);

  // increment the final string with the node specific string
     StringUtility::FileWithLineNumbers editedHeaderFileStringTemp = editStringStart + editedStringMiddle + editStringEnd;

  // Specification of declarations that will appear before or after the node associated class
  // (this issue comes up in SAGE where the Name class must have the postdeclaration string
  // "extern Name defaultName;" so that other classes which follow it can provide default
  // initialization of function parameters).
     StringUtility::FileWithLineNumbers predeclarationString(1, StringUtility::StringWithLineNumber(node.getToken().getPredeclarationString (), "" /* "<getPredeclarationString " + node.getToken().getName() + ">" */, 1));

     StringUtility::FileWithLineNumbers editedHeaderFileString = GrammarString::copyEdit (editedHeaderFileStringTemp,"$PREDECLARATIONS" ,predeclarationString);

     StringUtility::FileWithLineNumbers postdeclarationString(1, StringUtility::StringWithLineNumber(node.getToken().getPostdeclarationString(), "" /* "<getPostdeclarationString " + node.getToken().getName() + ">" */, 1));
     editedHeaderFileString = GrammarString::copyEdit (editedHeaderFileString,"$POSTDECLARATIONS",postdeclarationString);     

     editedHeaderFileString = editSubstitution (node,editedHeaderFileString);

#if 0
     printf ("FINAL String Value: editedHeaderFileString = %s \n",editedHeaderFileString);
     ROSE_ASSERT(false);
#endif

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file (each class in its own file)!
     string fileExtension = ".h";
     string directoryName = sourceCodeDirectoryName();
     writeFile ( editedHeaderFileString, directoryName, node.getName(), fileExtension );
#endif

  // Also output strings to single file (this outputs everything to a single file)
     outputFile += editedHeaderFileString;

  // Call this function recursively
  // printf ("In Grammar::buildHeaderFiles(): node.name = %s  (# of subtrees/leaves = %d) \n",
  //      node.getName(),node.nodeList.size());

     list<GrammarTreeNode *>::iterator treeListIterator;
     for( treeListIterator = node.nodeList.begin();
	  treeListIterator != node.nodeList.end();
	  treeListIterator++ )
        {
          ROSE_ASSERT ((*treeListIterator)->token != NULL);
          ROSE_ASSERT ((*treeListIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeListIterator)->parentTreeNode != NULL);	 
          buildHeaderFiles(**treeListIterator,outputFile);
        }
   }

StringUtility::FileWithLineNumbers
Grammar::editSubstitution ( GrammarTreeNode & node, const StringUtility::FileWithLineNumbers& editStringOrig )
   {
  // Setup default edit variables (locate them here to centralize the process)
     string className          = node.getName();
     string derivedClassString = getDerivedClassDeclaration(node);
     string parentClassName    = (node.parentTreeNode != NULL) ? 
                                 node.parentTreeNode->getName() :
				   "//"; //"NO PARENT AVAILABLE";
     string baseClassConstructorParameterString = "";
     string constructorParameterListString      = "";
     string constructorBodyString               = "";

  // printf ("In editSubstitution: className = %s \n",className);

     StringUtility::FileWithLineNumbers editString = editStringOrig;
     // fprintf(stderr, "Original editString: %s\n", editString.c_str());
     editString = GrammarString::copyEdit (editString,"$CLASSNAME",className);
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_NAME",getGrammarName());  // grammarName string defined in Grammar class
     editString = GrammarString::copyEdit (editString,"$BASECLASS",parentClassName);
     editString = GrammarString::copyEdit (editString,"$BASE_CLASS_CONSTRUCTOR_CALL",derivedClassString);
  // Set these to NULL strings if they are still present within the string
     editString = GrammarString::copyEdit (editString,"$BASE_CLASS_CONSTRUCTOR_PARAMETER",baseClassConstructorParameterString);
     editString = GrammarString::copyEdit (editString,"$CONSTRUCTOR_PARAMETER_LIST",constructorParameterListString);
     editString = GrammarString::copyEdit (editString,"$CONSTRUCTOR_BODY",constructorBodyString);
     editString = GrammarString::copyEdit (editString,"$CLASSTAG",node.getToken().getTagName());

  // edit the suffix of the $CLASSNAME (separate from the $GRAMMAR_PREFIX_)
  // printf ("node.getToken().getName() = %s \n",node.getToken().getBaseName());
  // printf ("node.getToken().getName() = %s (%s) \n",node.getToken().getName(),node.getToken().getBaseName());

     editString = GrammarString::copyEdit (editString,"$CLASS_BASE_NAME",node.getToken().getBaseName());

  // Fixup the declaration of pure virtual functions (so that they are defined properly at the leaves)
     std::string emptyString       = "";
     std::string pureVirtualMarker = " = 0";

     if (node.isLeafNode() == TRUE)
        {
          editString = GrammarString::copyEdit (editString,"$PURE_VIRTUAL_MARKER",emptyString);
        }
       else
        {
          editString = GrammarString::copyEdit (editString,"$PURE_VIRTUAL_MARKER",pureVirtualMarker);
        }

  // Now do final editing/substitution as specified by the user
     ROSE_ASSERT (node.token->getEditSubstituteTargetList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST).size() ==
                  node.token->getEditSubstituteSourceList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST).size());
     ROSE_ASSERT (node.token->getEditSubstituteTargetList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() ==
                  node.token->getEditSubstituteSourceList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size());
     ROSE_ASSERT (node.token->getEditSubstituteTargetList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST).size() ==
                  node.token->getEditSubstituteSourceList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST).size());
     ROSE_ASSERT (node.token->getEditSubstituteTargetList(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST).size() ==
                  node.token->getEditSubstituteSourceList(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST).size());

  // Local lists that we will accumulate elements into
  // (traversing up through the parents in the grammar tree)
     list<GrammarString *> targetList;
     list<GrammarString *> targetExcludeList;
     list<GrammarString *> sourceList;
     list<GrammarString *> sourceExcludeList;

  // Initialize with local node data
     targetList        = node.token->getEditSubstituteTargetList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
     targetExcludeList = node.token->getEditSubstituteTargetList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);
     sourceList        = node.token->getEditSubstituteSourceList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
     sourceExcludeList = node.token->getEditSubstituteSourceList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);

  // now generate the additions to the lists from the parent node subtree lists
     // BP : 10/09/2001, modified next two lines to provide addresses
     generateStringListsFromSubtreeLists ( node, targetList, targetExcludeList, &Terminal::getEditSubstituteTargetList );
     generateStringListsFromSubtreeLists ( node, sourceList, sourceExcludeList, &Terminal::getEditSubstituteSourceList );

  // int listLength = targetList.size();
     ROSE_ASSERT (sourceList.size()        == targetList.size());
     ROSE_ASSERT (sourceExcludeList.size() == targetExcludeList.size());

  // printf ("listLength = %d \n",listLength);
     list<GrammarString *>::iterator sourceListIterator, targetListIterator;
     for ( sourceListIterator = sourceList.begin(), targetListIterator = targetList.begin(); 
           sourceListIterator != sourceList.end(), targetListIterator != targetList.end(); 
           sourceListIterator++, targetListIterator++ )
        {
       // These are done in the order in which the user specified them!
	 
          // fprintf (stderr, "targetList[index].getFunctionNameString() = %s \n",(*targetListIterator)->getFunctionPrototypeString().c_str());
          // fprintf (stderr, "sourceList[index].getFunctionNameString() = %s \n",(*sourceListIterator)->getFunctionPrototypeString().c_str());

          editString = GrammarString::copyEdit ( editString,
                                  (*targetListIterator)->getFunctionPrototypeString(),
                                  (*sourceListIterator)->getFunctionPrototypeString() );
	  // fprintf(stderr, "After edit: %s\n", editString.c_str());
        }

  // Finally, Edit into place the name of the grammar
  // printf ("In editSubstitution node name = %s \n",node.getName());
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_PREFIX_",node.getToken().getGrammar()->getGrammarPrefixName());
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_TAG_PREFIX_",node.getToken().getGrammar()->getGrammarTagName());

     string parentGrammarPrefix = "";
     if (isRootGrammar() == TRUE)
        {
       // In the case of a root grammar there is no parent
          parentGrammarPrefix = node.getToken().getGrammar()->getGrammarPrefixName();
        }
       else
        {
       // Some subsitutions are dependent upon the prefix of the lower level grammar
          ROSE_ASSERT(node.getToken().getGrammar() != NULL);
          ROSE_ASSERT(node.getToken().getGrammar()->getParentGrammar() != NULL);
          parentGrammarPrefix = node.getToken().getGrammar()->getParentGrammar()->getGrammarPrefixName();
        }

     editString = GrammarString::copyEdit (editString,"$PARENT_GRAMMARS_PREFIX_",parentGrammarPrefix);
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_BASECLASS",grammarNameBaseClass);

  // We need to be able to substitute the "X" into some variable names etc. 
  // So the following helps to support this feature
     string X_Marker            = "";
     if (node.getToken().isChild() == TRUE)
        {
       // The "X" only appears in child node of the Grammar
          X_Marker = "X_";
        }
       else
        {
          X_Marker = "";
        }

     editString = GrammarString::copyEdit (editString,"$GRAMMAR_X_MARKER_",X_Marker);
     editString = GrammarString::copyEdit (editString,"$CLASSNAME",className);

     return editString;
   }

void
Grammar::buildVariantsStringPrototype ( StringUtility::FileWithLineNumbers & outputFile )
   {
  // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
  // This has been changed to use the newer V_SgNode form of the IR node names.

#if 0
     string startString = "extern struct \n" \
                         "   { \n" \
                         "     VariantT variant; \n" \
                         "     std::string name; \n" \
                         "   } $MARKERTerminalNames[$LIST_LENGTH]; \n";
#else
     string startString = "typedef struct \n" \
                         "   { \n" \
                         "     VariantT variant; \n" \
                         "     std::string name; \n" \
                         "   } TerminalNamesType; \n\n" \
                         "extern TerminalNamesType $MARKERTerminalNames[$LIST_LENGTH]; \n\n";
#endif
#if 1
  // Set the type name using the grammarName variable contained within the grammar
  // startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarPrefixName());
     startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarName());

     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;

  // char* listLengthString = strtoul(nonTerminalList.size());
     char* listLengthString = new char [10];

  // Build a string representing the number of elements (add 1 for the LAST_TAG)
     sprintf (listLengthString,"%zu",maxVariant + 2);

  // COPY the length into the string at "LIST_LENGTH"
     startString = GrammarString::copyEdit (startString,"$LIST_LENGTH",listLengthString);

  // BP : 10/24/2001,free mem.
     delete [] listLengthString;
#endif

     string finalString = startString;

     outputFile.push_back(StringUtility::StringWithLineNumber(finalString, "", 1));
   }


void
Grammar::buildVariantsStringDataBase ( StringUtility::FileWithLineNumbers & outputFile )
   {
#if 0
     string startString = "static struct \n" \
                         "   { \n" \
                         "     $MARKERVariants variant; \n" \
                         "     $MARKERVariants variant; \n" \
                          "     std::string name; \n" \
                         "   } $MARKERTerminalNames[$LIST_LENGTH] = {  \n";
#else
#if 0
     string startString = "struct \n" \
                         "   { \n" \
                         "     VariantT variant; \n" \
                          "    std::string name; \n" \
                         "   } $MARKERTerminalNames[$LIST_LENGTH] = {  \n";
#else
     string startString = "TerminalNamesType $MARKERTerminalNames[$LIST_LENGTH] = {  \n";
#endif
#endif

  // Set the type name using the grammarName variable contained within the grammar
  // startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarPrefixName());
     startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarName());

     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;

  // char* listLengthString = strtoul(nonTerminalList.size());
     char* listLengthString = new char [10];

  // Build a string representing the number of elements (add 1 for the LAST_TAG)
     sprintf (listLengthString,"%zu",(size_t)(maxVariant+2U));

  // COPY the length into the string at "LIST_LENGTH"
     startString = GrammarString::copyEdit (startString,"$LIST_LENGTH",listLengthString);

     // BP : 10/24/2001,free mem.
     delete [] listLengthString;

     // BP : 10/25/2001, this new version of the code only performs one new memory allocation,
     // uses list iterators instead of the operator[], and should be a lot faster
     string openString      = "          {";
     string seperatorString = ", \"";
     string closeString     = "\"}, \n";
     list<Terminal *>::const_iterator  it;

     string middleString;

     vector<string> variantNames;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
       if (i->first + 1 > variantNames.size()) {
         variantNames.resize(i->first + 1, "<ERROR: unknown VariantT>");
       }
       variantNames[i->first] = i->second;
     }
     bool first = true;
     for (size_t i=0; i < variantNames.size(); i++) {
       middleString += openString + "(VariantT)" + StringUtility::numberToString(i) + seperatorString + variantNames[i] + closeString;
       first = false;
     }

  // string endString = "          {$MARKER_LAST_TAG, \"last tag\" } \n   }; \n\n\n";
     string endString = "          {V_SgNumVariants, \"last tag\" } \n   }; \n\n\n";

     endString = GrammarString::copyEdit (endString,"$MARKER",getGrammarName());

     string finalString = startString + middleString + endString;

     outputFile.push_back(StringUtility::StringWithLineNumber(finalString, "", 1));
   }


void
Grammar::buildSourceFiles( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildSourceFiles() \n");
  // printf ("Exiting at TOP of Grammar::buildSourceFiles() \n");
  // ROSE_ABORT();

     string sourceFileInsertionSeparator = "MEMBER_FUNCTION_DEFINITIONS";
     string fileName = "../Grammar/grammarClassDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers sourceFileTemplate = readFileWithPos (fileName);

  // place this string into a separate locations (isolate it from side effects)
     // BP : 10/25/2001, no need to duplicate here
#if 0
     sourceFileTemplate = GrammarString::stringDuplicate (sourceFileTemplate);
#endif

     StringUtility::FileWithLineNumbers sourceBeforeInsertion = buildHeaderStringBeforeMarker(sourceFileInsertionSeparator, fileName);
     StringUtility::FileWithLineNumbers sourceAfterInsertion = buildHeaderStringAfterMarker(sourceFileInsertionSeparator, fileName);

     string derivedClassString;

  // printf ("Exiting to test editing ... \n");

  // int editedStringMiddleLength = 0;
     StringUtility::FileWithLineNumbers editedStringMiddle;
  // Each of these functions should return a null terminated string
  // (even if there are no code strings associated with this node).
  // char* editStringMiddleNodeMemberFunctions = computeNodeSpecificMemberFunctionsSource (node);

  // At this point data access functions have already been built and placed into the source code lists
  // all that is left is the construction of the code specific to the constructor
  // to build the constructor we require all the data variables
  // (which is why we could not have build it with the access functions)

     StringUtility::FileWithLineNumbers editStringMiddleNodeDataMemberFunctions = buildConstructor (node);

  // printf ("editStringMiddleNodeDataMemberFunctions = %s \n",editStringMiddleNodeDataMemberFunctions);

     StringUtility::FileWithLineNumbers editStringMiddleNodeMemberFunctions = buildStringForSource(node);

  // Place the constructor at the top of the node specific code for this element of grammar

     // BP : 10/24/2001, keep track of memory
     editedStringMiddle += editStringMiddleNodeMemberFunctions;
     editedStringMiddle += editStringMiddleNodeDataMemberFunctions;

  // printf ("editStringMiddleNodeMemberFunctions = %s \n",editStringMiddleNodeMemberFunctions);
  // printf ("editedStringMiddle = %s \n",editedStringMiddle);

  // increment the final string with the node specific string

     StringUtility::FileWithLineNumbers editedSourceFileString = sourceBeforeInsertion + editedStringMiddle;
     editedSourceFileString += sourceAfterInsertion;
  // Now apply the edit/subsitution specified within the grammar (by the user)
     editedSourceFileString = editSubstitution (node,editedSourceFileString);

#if 0
     printf ("AFTER GrammarString::copyEdit: editedSourceFileString = %s \n",editedSourceFileString);
#endif

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file!
     string fileExtension = ".C";
     string directoryName = sourceCodeDirectoryName();
     writeFile ( editedSourceFileString, directoryName, node.getName(), fileExtension );
#endif

#if 1
// Also output strings to single file
     outputFile += editedSourceFileString;
#endif

  // printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

#if 1
  // Call this function recursively on the children of this node in the tree
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	  treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator)->token != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->token->grammarSubTree != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->parentTreeNode != NULL);

          buildSourceFiles(**treeNodeIterator,outputFile);
        }
#endif
   }

void
Grammar::printTreeNodeNames ( const GrammarTreeNode & node ) const
{
  list<GrammarTreeNode *>::const_iterator treeNodeIterator;
  int i=0;
  if (node.nodeList.size() > 0)
    {
      printf ("\n");
      printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName().c_str(),(size_t) node.nodeList.size());
      for( treeNodeIterator = node.nodeList.begin();
	   treeNodeIterator != node.nodeList.end();
	   treeNodeIterator++ )
	{
	  printf ("     node.nodeList[%d] = %s (%s) \n",
		  i, (*treeNodeIterator)->getName().c_str(),
		  ((*treeNodeIterator)->nodeList.size() == 0) ? "IS A LEAF" : "IS NOT A LEAF");
	  i++;
	}

      printf ("\n");
      
      for( treeNodeIterator = node.nodeList.begin();
	   treeNodeIterator != node.nodeList.end();
	   treeNodeIterator++ )
	{
	  printTreeNodeNames(**treeNodeIterator);
	}
    }
}

size_t Grammar::getVariantForNode(const std::string& name) const {
  std::map<std::string, size_t>::const_iterator it = this->astNodeToVariantMap.find(name);
  if (it == this->astNodeToVariantMap.end()) {
    it = this->astNodeToVariantMap.find(this->grammarPrefixName + name);
  }
  if (it == this->astNodeToVariantMap.end()) {
    std::cerr << "Could not find variant number for " << name << std::endl;
    std::cerr << "This node name must be added to the list in $(top_srcdir)/src/ROSETTA/astNodeList" << std::endl;
    abort();
  }
  return it->second;
}

size_t Grammar::getVariantForTerminal(const Terminal& t) const {
  return this->getVariantForNode(t.getName());
}

size_t Grammar::getVariantForNonterminal(const NonTerminal& t) const {
  return this->getVariantForNode(t.getName());
}

string Grammar::getNodeForVariant(size_t var) const {
  std::map<size_t, std::string>::const_iterator it = this->astVariantToNodeMap.find(var);
  ROSE_ASSERT (it != this->astVariantToNodeMap.end());
  return it->second;
}

Terminal& Grammar::getTerminalForVariant(size_t var) {
  std::map<size_t, Terminal*>::const_iterator it = this->astVariantToTerminalMap.find(var);
  ROSE_ASSERT (it != this->astVariantToTerminalMap.end());
  ROSE_ASSERT (it->second);
  return *(it->second);
}

StringUtility::FileWithLineNumbers
Grammar::buildVariants ()
   {
     string header = "//! Variants used to identify elements of the grammar used in ROSE \n" \
                          "/*! Each element is assigned a unique value defined by this enumerated type \n" \
                          "    the values are used to generate the casts from one type toanother were permitted. \n" \
                          "    This is a technique borrowed from the design of SAGE II. \n" \
                          "*/ \n" \
                          "enum $MARKERVariants \n" \
                          "   { \n";

     string footer = "     $MARKER_UNKNOWN_GRAMMAR,\n" \
                          "     $MARKER_LAST_TAG \n" \
                          "   }; \n";
     
     string seperatorString = "     ";
     string newlineString   = ",\n";

     unsigned int i=0;

     //     cout << "Grammar::buildVariants (): The current string length is " << stringLength << endl;

     // now allocate the necessary memory
     StringUtility::FileWithLineNumbers returnString;
     returnString.push_back(StringUtility::StringWithLineNumber(header, "" /* "<buildVariants header>" */, 1));

     for (i=0; i < nonTerminalList.size(); i++)
       {
	 returnString.push_back(StringUtility::StringWithLineNumber(seperatorString + nonTerminalList[i].getTagName() + " = " + StringUtility::numberToString(this->getVariantForNonterminal(nonTerminalList[i])) + ", ", "" /* "<variant for nonterminal " + nonTerminalList[i].getTagName() + ">" */, 1));
       }

     for (i=0; i < terminalList.size(); i++)
       {
	 returnString.push_back(StringUtility::StringWithLineNumber(seperatorString + terminalList[i].getTagName() + " = " + StringUtility::numberToString(this->getVariantForTerminal(terminalList[i])) + ", ", "" /* "<variant for terminal " + terminalList[i].getTagName() + ">" */, 1));
       }

     returnString.push_back(StringUtility::StringWithLineNumber(footer, "" /* "<buildVariants footer>" */, 1));
     //     printf ("In Grammar::buildVariants (): returnString = \n%s\n",returnString);

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildForwardDeclarations ()
   {
#if 0
#if 0
     char* returnString = new char [10000];
     ROSE_ASSERT (returnString != NULL);
     returnString[0] = '\0';

     char* header = "\n\n\n//! Forward Declarations used to represent the grammar used in ROSE \n";
     char* footer = "\n\n\n";

     strcat (returnString,GrammarString::stringDuplicate(header));

  // Build all the tree nodes up frount and then connect them together to form the tree
     int i=0;
     for (i=0; i < nonTerminalList.size(); i++)
        {
          strcat (returnString,GrammarString::stringDuplicate("class "));
	  strcat (returnString,GrammarString::stringDuplicate(nonTerminalList[i].name));
          strcat (returnString,GrammarString::stringDuplicate(";\n"));
        }

     for (i=0; i < terminalList.size(); i++)
        {
          strcat (returnString,GrammarString::stringDuplicate("class "));
	  strcat (returnString,GrammarString::stringDuplicate(terminalList[i].name));
          strcat (returnString,GrammarString::stringDuplicate(";\n"));
        }

     strcat (returnString,GrammarString::stringDuplicate(footer));
#else
     // I'm rewriting this code to just allocate the correct amount of memory instead
     // of these very large blocks
     // BP : 10/23/2001
     int stringLength = 0;
     const char* header = "\n\n\n//! Forward Declarations used to represent the grammar used in ROSE \n";
     const char* footer = "\n\n\n";
     const char *classKeyWord = "class ";
     const char *classDeclTerminator = ";\n";
     const int commonStrLen = strlen(classKeyWord)+strlen(classDeclTerminator);

     stringLength = strlen(header)+strlen(footer);
     //     cout << "The current string length is " << stringLength << endl;

     unsigned int i=0;
     for (i=0; i < nonTerminalList.size(); i++)
       stringLength += commonStrLen+strlen(nonTerminalList[i].name);
     for (i=0; i < terminalList.size(); i++)
       stringLength += commonStrLen+strlen(terminalList[i].name);

     stringLength++;  // to account for the terminating \0
     //     cout << "The current string length is " << stringLength << endl;

     // now allocate the necessary memory
     char* returnString = new char[stringLength];
     ROSE_ASSERT( returnString != NULL );

     returnString[0] = '\0';
     strcat (returnString, header);

     for (i=0; i < nonTerminalList.size(); i++)
        {
          strcat (returnString, classKeyWord);
	  strcat (returnString, nonTerminalList[i].name);
          strcat (returnString, classDeclTerminator );
        }

     for (i=0; i < terminalList.size(); i++)
        {
          strcat (returnString, classKeyWord);
	  strcat (returnString, terminalList[i].name);
          strcat (returnString, classDeclTerminator );
        }

     strcat (returnString, footer);
     //     printf ("In Grammar::buildForwardDeclarations (): returnString = \n%s\n",returnString);

     ROSE_ASSERT (returnString != NULL);
     return returnString;
#endif
#else
  // DQ (4/23/2006): Need to add forward declarations of "Sg[CLASSNAME]* isSg[CLASSNAME](SgNode*)" friend functions

     string header = "\n\n\n//! Forward Declarations used to represent the grammar used in ROSE \n";

     StringUtility::FileWithLineNumbers returnString;
     returnString.push_back(StringUtility::StringWithLineNumber(header, "" /* "<buildForwardDeclarations header>" */, 1));

     for (unsigned int i=0; i < nonTerminalList.size(); i++)
        {
	  returnString.push_back(StringUtility::StringWithLineNumber("class " + nonTerminalList[i].name + ";", "" /* "<forward decl for " + nonTerminalList[i].name + ">" */, 1));
        }

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
	  returnString.push_back(StringUtility::StringWithLineNumber("class " + terminalList[i].name + ";", "" /* "<forward decl for " + terminalList[i].name + ">" */, 1));
        }

     returnString.push_back(StringUtility::StringWithLineNumber("\n\n// Forward declaration of \"<classname> is<classname> (SgNode* n)\" friend functions.\n", "" /* "<unknown>" */, 1));
     returnString.push_back(StringUtility::StringWithLineNumber("// GNU g++ 4.1.0 requires these be declared outside of the class (because the friend declaration in the class is not enough).\n\n", "" /* "<unknown>" */, 2));

     for (unsigned int i=0; i < nonTerminalList.size(); i++)
        {
          string className = nonTerminalList[i].name;
	  returnString.push_back(StringUtility::StringWithLineNumber(className + "* is" + className + "(SgNode* node);", "" /* "<downcast function for " + className + ">" */, 1));
	  returnString.push_back(StringUtility::StringWithLineNumber("const " + className + "* is" + className + "(const SgNode* node);", "" /* "<downcast function for " + className + ">" */, 2));
        }

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string className = terminalList[i].name;
	  returnString.push_back(StringUtility::StringWithLineNumber(className + "* is" + className + "(SgNode* node);", "" /* "<downcast function for " + className + ">" */, 1));
	  returnString.push_back(StringUtility::StringWithLineNumber("const " + className + "* is" + className + "(const SgNode* node);", "" /* "<downcast function for " + className + ">" */, 2));
        }

  // printf ("In Grammar::buildForwardDeclarations (): returnString = \n%s\n",returnString.c_str());
  // ROSE_ASSERT(false);

     return returnString;
#endif
   }

string
Grammar::buildTransformationSupport()
   {
  // DQ (11/27/2005): This function builds support text for transformations
  // that change the names of interface and objects as part of a pre-release 
  // effort to fixup many details of ROSE.  The goal is to do it at one time
  // and provide the automate mechanism to ROSE users as well.

  // Goal is to generate: "pair<string,string> array[2] = { pair<string,string>("a1","a2"), pair<string,string>("b1","b2") };"

  // int arrayOfStringsSize = nonTerminalList.size() + terminalList.size();
     const string header = "Text to be use in the development of automated translation of interfaces. \n" \
                           "string arrayOfStrings[] \n" \
                           "   { \n";
     const string footer = "   }; \n";

     const string seperatorString = "          pair<string,string>(";
     const string newlineString   = "),\n";

     unsigned int i=0;

  // now allocate the necessary memory
     string returnString = header;

     for (i=0; i < nonTerminalList.size(); i++)
        {
          returnString += seperatorString;
          returnString += string("\"") + nonTerminalList[i].getTagName() + string("\"");
          returnString += string(", \"V_") + nonTerminalList[i].name + string("\"");
          returnString += newlineString;
       }

     for (i=0; i < terminalList.size(); i++)
        {
          returnString += seperatorString;
          returnString += string("\"") + terminalList[i].getTagName() + string("\"");
          returnString += string(", \"V_") + terminalList[i].name + string("\"");
          returnString += newlineString;
        }

     returnString += footer;

#if 0
  // printf ("In Grammar::buildTransformationSupport(): returnString = \n%s\n",returnString.c_str());
     printf ("Exiting after construction of transformation support string \n");
     ROSE_ASSERT(false);
#endif

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::extractStringFromFile ( 
   const string& startMarker, const string& endMarker, 
   const string& filename, const string& directory )
   {
  // Open file
     StringUtility::FileWithLineNumbers fileString = Grammar::readFileWithPos (filename);

  // search for starting marker string
     bool found = false;
     for (unsigned int i = 0; i < fileString.size(); ++i)
        {
          std::string::size_type pos = fileString[i].str.find(startMarker);
          if (pos != string::npos)
             {
               fileString.erase(fileString.begin(), fileString.begin() + i);
               fileString[0].str = fileString[0].str.substr(pos + startMarker.size());
               found = true;
               break;
             }
        }

  // If this is false then the MARKER_*_START strings were not located in the file
     if (found == false)
        {
          printf ("Error: could not locate startMarker = %s in file = %s \n",startMarker.c_str(),filename.c_str());
        }
     ROSE_ASSERT (found);

     found = false;
     for (unsigned int i = 0; i < fileString.size(); ++i)
        {
          std::string::size_type pos = fileString[i].str.find(endMarker);
          if (pos != string::npos)
             {
               fileString.erase(fileString.begin() + i + 1, fileString.end());
               fileString[i].str = fileString[i].str.substr(0, pos);
               found = true;
               break;
             }
        }

  // If this is false then the MARKER_*_END strings were not located in the file
     ROSE_ASSERT (found);

     return fileString;
   }


void
Grammar::setupLeafNodeList ()
   {
     traverseTreeToSetupLeafNodeList(getRootOfGrammar());
   }

void
Grammar::traverseTreeToSetupLeafNodeList ( GrammarTreeNode & node )
   {
  // printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());
     if (node.isLeafNode() == TRUE)
        {
          ROSE_ASSERT (node.token != NULL);
          leafNodeList.push_back( node.token);
        }

     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin();
	  treeNodeIterator != node.nodeList.end();
	   treeNodeIterator++ )
       {
	 traverseTreeToSetupLeafNodeList(**treeNodeIterator);
       }
   }

string
Grammar::getFilenameForGlobalDeclarations()
   {
     return filenameForGlobalDeclarations;
   }

void
Grammar::setFilenameForGlobalDeclarations( const string& filename )
   {
     filenameForGlobalDeclarations = filename;
   }


StringUtility::FileWithLineNumbers
Grammar::buildMiscSupportDeclarations()
   {
  // This function allows the introduction of support classes for the grammar
     StringUtility::FileWithLineNumbers returnString;

  // char* fileName  = "../Grammar/grammarConstructorDeclarationMacros.macro";
  // char* fileName  = getFilenameForSupportClasses();
     string fileName  = getFilenameForGlobalDeclarations();
     returnString = readFileWithPos (fileName);

  // printf ("In (BEFORE EDITING) Grammar::buildMiscSupportDeclarations(): returnString = \n %s \n",returnString);

  // Finally, Edit into place the name of the grammar
     returnString = GrammarString::copyEdit (returnString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());

  // printf ("In (AFTER EDITING) Grammar::buildMiscSupportDeclarations(): returnString = \n %s \n",returnString);

     return returnString;
   }

// MS: the Coco scanner requires Scanner Symbols built such
// 1) an underscore in the name is replaced with the string "Underscore"
// 2) the string is truncated to 28 characters.
string 
Grammar::cocoScannerDefineName(string s)
   {
  // 1) replace "_" with "Underscore"
     string::size_type posIter;
     while((posIter = s.find("_")) != string::npos)
        {
          if (posIter != string::npos)
               s.replace(posIter, 1, "Underscore");
        }

  // DQ (3/23/2004): Truncated names were not being found properly (not ssure why they had to be truncated)
  // return s.substr(0,28)+"Sym";
     return s + "Sym";
   }
  
string
Grammar::cocoScannerDefines() {
  ostringstream os;
  string prefix="case ";
  string postfix="break;\n";

  unsigned int nextCode=1;
  for (unsigned int i=0; i < terminalList.size(); i++) {
    os << prefix << "V_" << terminalList[i].name << " : code = "
       << cocoScannerDefineName(terminalList[i].name) << "; "
       << postfix;
  }
  nextCode+=terminalList.size();
#if 0
os << prefix << "LparenSym" << nextCode++ << " /* \"(\" */"+postfix;
  os << prefix << "RparenSym" << nextCode++ << " /* \")\" */"+postfix;
  os << prefix << "No_Sym" << nextCode++ << "/ * not */"+postfix;
  os << prefix << "MAXT    No_Sym /* Max Terminals */"+postfix;
#endif

  return os.str(); // returns string
}


// MS: new automatically generated variant. Replaces variant().
// used in variantT()
string
Grammar::buildVariantEnums() {
  string s=string("enum VariantT {\n");
  unsigned int i;
  bool notFirst=false;
  for (i=0; i < nonTerminalList.size(); i++) {
    if(notFirst) {
      s+=string(",\n");
    }
    notFirst=true;
    size_t varNum = this->getVariantForNode(nonTerminalList[i].name);
    s+=(string("V_")+nonTerminalList[i].name+" = "+StringUtility::numberToString(varNum));
  }
  for (i=0; i < terminalList.size(); i++) {
    if(notFirst) {
      s+=string(",\n");
    }
    notFirst=true;
    size_t varNum = this->getVariantForNode(terminalList[i].name);
    s+=(string("V_")+terminalList[i].name+" = "+StringUtility::numberToString(varNum));
  }
  // add an ENUM to get the number of enums declared.
  s+=string(", V_SgNumVariants = ")+StringUtility::numberToString(this->astNodeToVariantMap.size() + 1);
  s+="};\n";  
  return s;
}


// AS: new automatically generated variant. Replaces variant().
// used in variantT()
string
Grammar::buildClassHierarchySubTreeFunction() {

  //The first function which takes a vector reference which is used to 
  //return the result.
  string s="void SgNode::getClassHierarchySubTreeFunction( VariantT v, std::vector<VariantT>& subTreeVariants){\n";
	
  //s+=string("std::vector<VariantT> subTreeVariants;\n");
  s+="switch(v){\n ";
  unsigned int i;
  for (i=0; i < nonTerminalList.size(); i++) {
    s+="case " + string("V_")+string(nonTerminalList[i].name)+":\n";

	s+="{\n";

	GrammarTreeNode* grammarSubTree = nonTerminalList[i].grammarSubTree;
	for(list<GrammarTreeNode*>::iterator iItr = grammarSubTree->nodeList.begin();
		iItr != grammarSubTree->nodeList.end(); ++iItr)
	{
     	s+= "subTreeVariants.push_back(V_"+ string((*iItr)->getName()) + ");\n"; 
 
	}
	s+="break;\n";
	s+="}\n";
  }
  //Add default case
  s+="default:\n{ }\n";
  s+="}\n\n";

  s+="};\n";  

  //Building second function which return a vector. This is a slower call
  //due to creation and destruction of vectords
  s+="\n\n\n\n";

  s+="std::vector<VariantT> SgNode::getClassHierarchySubTreeFunction( VariantT v){\n";
  s+="std::vector<VariantT> subTreeVariants;\n";
  s+="getClassHierarchySubTreeFunction(v, subTreeVariants);\n";
  s+="return subTreeVariants;\n";
  s+="}\n";

  return s;
}

// AS: new automatically generated variant. Replaces variant().
// used in variantT()
string
Grammar::buildMemoryPoolBasedVariantVectorTraversalSupport() {

  //The first function which takes a vector reference which is used to 
  //return the result.
  string s="template <class FunctionalType>\n";
	     s+="void AstQueryNamespace::queryMemoryPool(AstQuery<ROSE_VisitTraversal,FunctionalType>& astQuery,";
		 s+=" VariantVector* variantsToTraverse)\n";
		 s+="  {\n";
 
  //s+=string("std::vector<VariantT> subTreeVariants;\n");
   s+="for (VariantVector::iterator it = variantsToTraverse->begin(); it != variantsToTraverse->end(); ++it)\n";
   s+="  {\n";
   s+="switch(*it){\n ";

  unsigned int i;
  for (i=0; i < nonTerminalList.size(); i++) {
    s+="case " + string("V_")+string(nonTerminalList[i].name)+":\n";
	s+="{\n";
    s+=nonTerminalList[i].name+"::traverseMemoryPoolNodes(astQuery);\n";
	s+="break;\n";
	s+="}\n";
  }

  for (i=0; i < terminalList.size(); i++) {
    s+="case " + string("V_")+terminalList[i].name+":\n";
	s+="{\n";
    s+=terminalList[i].name+"::traverseMemoryPoolNodes(astQuery);\n";
	s+="break;\n";
	s+="}\n";
  }

  //Add default case
  s+="default:\n{\n";
  s+="// This is a common error after adding a new IR node (because this function should have been automatically generated).\n";
  s+="std::cout << \"Case not implemented in queryMemoryPool(..). Exiting.\" << std::endl;\n";
  s+="ROSE_ASSERT(false);\n";
  s+="break;\n";
  s+="}\n";
  s+="}\n";
  s+="}\n\n";
  s+="};\n";  

  return s;
}

// MS: new automatically generated variantnames as variantEnum->string mapping
string
Grammar::buildVariantEnumNames() {
  vector<string> variantNames;
  for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
    if (i->first + 1 > variantNames.size()) {
      variantNames.resize(i->first + 1, "<ERROR: unknown VariantT>");
    }
    variantNames[i->first] = i->second;
  }
  bool first = true;
  string s = "";
  for (size_t i=0; i < variantNames.size(); i++) {
    s+=(first ? "" : string(",\n"))+"\"" + variantNames[i]+string("\"");
    first = false;
  }
  return s;
}

void
Grammar::buildCode ()
   {
  // Build tree representing the type hierarchy
     buildTree();
     ROSE_ASSERT (rootNode != NULL);
     ROSE_ASSERT (rootNode->parentTreeNode == NULL);
  // printTreeNodeNames(*rootNode);

  // HEADER FILES GENERATION
     StringUtility::FileWithLineNumbers ROSE_ArrayGrammarHeaderFile;

  // Put in comment block for Doxygen (so that autogenerated 
  // grammars can be automatically documented).
     string headerString = "// MACHINE GENERATED HEADER FILE --- DO NOT MODIFY! \n\n\n" \
                          "//! AST implementation generated by ROSETTA \n" \
                          "//    (in most cases based upon Sage II's implementation (Gannon et. al.). \n\n\n" \
       "#include \"C++_include_files.h\" \n\n\n" \
       "#ifndef $IFDEF_MARKER_H \n" \
       "#define $IFDEF_MARKER_H \n\n" \
       "#include <sstream>\n" \
       "#include \"AstAttributeMechanism.h\"\n\n";
     string footerString = "\n\n\n#endif // ifndef IFDEF_MARKER_H \n\n\n";

  // Get the strings onto the heap so that copy edit can process it (is this poor design? MS: yes)
     headerString = GrammarString::copyEdit (headerString,"$IFDEF_MARKER",getGrammarName());
     footerString = GrammarString::copyEdit (footerString,"$IFDEF_MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile << headerString;

  // Build (old) ast node variants and write them to the header file ...
  // BP 10/19/2001, bugfix to prevent memory leak
  // OLD variants
     StringUtility::FileWithLineNumbers variantsString = buildVariants();
     variantsString = GrammarString::copyEdit ( variantsString,"$MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile += variantsString;

  // MS: NEW variants
  // build new variants (access with variantT())
     string variantEnumString = buildVariantEnums();
     ROSE_ArrayGrammarHeaderFile << variantEnumString;

  // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
     buildVariantsStringPrototype ( ROSE_ArrayGrammarHeaderFile );

  // char *forwardDeclString = buildForwardDeclarations ();
     StringUtility::FileWithLineNumbers forwardDeclString = buildForwardDeclarations();
  // forwardDeclString = GrammarString::copyEdit (forwardDeclString,string("$MARKER"),string(getGrammarName()));
     forwardDeclString = StringUtility::copyEdit (forwardDeclString,"$MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile += forwardDeclString;
  // delete [] forwardDeclString;

  // JH (01/09/2006) : Adding the declaration of the ParentStorageClass: above!
     ROSE_ArrayGrammarHeaderFile << buildStorageClassDeclarations();

     ROSE_ArrayGrammarHeaderFile << "\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, SgName&);\n\n";

  // DQ (12/6/2003): Added output function for SgBitVector objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, std::vector<bool>&);\n\n";

  // DQ (8/3/2005): Added output function for STL set objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, std::set<int>&);\n\n";

  // DQ (10/4/2006): Added output function for STL map objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, std::map<SgNode*,int>&);\n\n";

  // DQ (10/6/2006): Added output function for STL map objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, std::map<SgNode*,std::string>&);\n\n";

// Support for binaries must be turned on via the configure command line.
#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
  // DQ (3/31/2007): Modified to be a list instead of a set (and added case for list of SgAsmExpression),
  //                 though I am not certain these are even required to be specified.
  // DQ (3/15/2007): Added output function for STL list objects
  // ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, std::set<SgAsmStatement*>&);\n\n";

     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgAsmStatement*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgAsmExpression*>&);\n\n";
#endif

  // DQ (11/20/2007): Part of support for the Fortran data statement
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgDataStatementObject*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgDataStatementValue*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgCommonBlockObject*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgDimensionObject*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgLabelSymbol*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, Rose_STL_Container<SgFormatItem*>&);\n\n";

  // traversal order
  // ROSE_ArrayGrammarHeaderFile << "typedef enum {preorder, postorder} t_traverseOrder;\n\n";
  // GB (7/6/2007): Changed these values so we can build bitmasks. This makes
  // it possible to define somewhat more general traversals that have both pre
  // and post order components. The user doesn't notice this change.
     ROSE_ArrayGrammarHeaderFile << "typedef enum {preorder = 1, postorder = 2, preandpostorder = preorder | postorder} t_traverseOrder;\n\n";

  // Now declare the classes representing the terminals and nonterminals within the grammar
     ROSE_ASSERT (rootNode != NULL);
     buildHeaderFiles(*rootNode,ROSE_ArrayGrammarHeaderFile);

#if 0
  // Now declare the class representing the grammar (not the elements of the grammar)
#if ALWAYS_BUILD_CLASS_FOR_GRAMMAR
     buildGrammarClassDeclaration(ROSE_ArrayGrammarHeaderFile);
#else
  // if ( !(GrammarString::isSameName(grammarNameBaseClass,"")) )
     if ( !isRootGrammar() )
        {
       // The lowest level grammar does not have a grammar base class (I think)
          buildGrammarClassDeclaration(ROSE_ArrayGrammarHeaderFile);
        }
#endif
#endif

  // DQ (11/26/2005): Support for visitor pattern.     
     string visitorSupport = buildVisitorBaseClass();
     ROSE_ArrayGrammarHeaderFile.push_back(StringUtility::StringWithLineNumber(visitorSupport, "", 1));

     ROSE_ArrayGrammarHeaderFile.push_back(StringUtility::StringWithLineNumber(footerString, "", 1));

     // Now place all global declarations at the base of the 
     // header file after all classes have been defined
     StringUtility::FileWithLineNumbers miscSupport = buildMiscSupportDeclarations ();
     ROSE_ArrayGrammarHeaderFile += miscSupport;

     Grammar::writeFile(ROSE_ArrayGrammarHeaderFile, ".", getGrammarName(), ".h");
#if 1
     // AST SOURCE FILE GENERATION
     StringUtility::FileWithLineNumbers ROSE_ArrayGrammarSourceFile;
     // Now build the source files
     
     string includeHeaderFileName = "sage3.h";
     string includeHeaderString =
  // GrammarString::stringConcatenate("// MACHINE GENERATED SOURCE FILE --- DO NOT MODIFY!\n\n"
  // "#ifdef HAVE_CONFIG_H\n#include <config.h>\n#endif\n\n#include \"",includeHeaderFileName);
       "// MACHINE GENERATED SOURCE FILE --- DO NOT MODIFY!\n\n #include \"" + includeHeaderFileName + "\"\n\n";

  // DQ (10/18/2007): These have been moved to the src/frontend/SageIII directory
  // to provde greater parallelism to the make -jn parallel make feature.
  // JH (01/09/2006)
  // string includeHeaderAstFileIO ="#include \"astFileIO/AST_FILE_IO.h\"\n\n";
     string includeHeaderAstFileIO ="#include \"AST_FILE_IO.h\"\n\n";
     includeHeaderString += includeHeaderAstFileIO;
     includeHeaderString += "\nusing namespace std;\n";

     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));

  // DQ (12/31/2005): Insert "using namespace std;" into the source file (but never into the header files!)
     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber("\n// Simplify code by using std namespace (never put into header files since it effects users) \nusing namespace std;\n\n", "", 1));

  // Setup the data base of names (linking name strings to grammar element tags)
     buildVariantsStringDataBase(ROSE_ArrayGrammarSourceFile);
     
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);
     buildSourceFiles(*rootNode,ROSE_ArrayGrammarSourceFile);
     cout << "DONE: buildSourceFiles()" << endl;

  // DQ (5/24/2005): Support for evaluation of memory sizes of IR nodes
     string memoryStorageEvaluationSupport = buildMemoryStorageEvaluationSupport();
     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber(memoryStorageEvaluationSupport, "", 1));

  // DQ (12/23/2005): Build the visitor pattern traversal code (to call the traveral 
  // of the memory pools for each IR node)
     string memoryPoolTraversalSupport = buildMemoryPoolBasedTraversalSupport();
     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber(memoryPoolTraversalSupport, "", 1));

#if 0
 // We are not ready to worry about building the source for the grammar class itself!
#if ALWAYS_BUILD_CLASS_FOR_GRAMMAR
     buildGrammarClassSourceCode (ROSE_ArrayGrammarSourceFile);
#else
     if ( !isRootGrammar() )
        {
          buildGrammarClassSourceCode (ROSE_ArrayGrammarSourceFile);
	}
#endif
#endif
     Grammar::writeFile(ROSE_ArrayGrammarSourceFile, ".", getGrammarName(), ".C");

#endif

#if 1
   //-----------------------------------------------
   // generate code for the new and delete operators
   //-----------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_NewAndDeleteOperatorSourceFile;

     ROSE_NewAndDeleteOperatorSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildNewAndDeleteOperators(*rootNode,ROSE_NewAndDeleteOperatorSourceFile);
     cout << "DONE: buildNewAndDeletOperators()" << endl;

  // printf ("Exiting after building new and delete operators \n");
  // ROSE_ASSERT(false);

  // DQ(10/22/2007): fixed missed variable renaming.
  // Grammar::writeFile(ROSE_ArrayGrammarSourceFile, ".", getGrammarName() + "NewAndDeleteOperators", ".C");
     Grammar::writeFile(ROSE_NewAndDeleteOperatorSourceFile, ".", getGrammarName() + "NewAndDeleteOperators", ".C");
#endif


#if 1
   //--------------------------------------------
   // generate code for the memory pool traversal
   //--------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_TraverseMemoryPoolSourceFile;

     ROSE_TraverseMemoryPoolSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildTraverseMemoryPoolSupport(*rootNode,ROSE_TraverseMemoryPoolSourceFile);
     cout << "DONE: buildTraverseMemoryPoolSupport()" << endl;

  // printf ("Exiting after building traverse memory pool functions \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_TraverseMemoryPoolSourceFile, ".", getGrammarName() + "TraverseMemoryPool", ".C");
#endif


#if 1
  // --------------------------------------------
  // generate code for the memory pool traversal
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile;

     ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildStringForCheckingIfDataMembersAreInMemoryPoolSupport(*rootNode,ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile);
     cout << "DONE: buildStringForCheckingIfDataMembersAreInMemoryPoolSupport()" << endl;

  // printf ("Exiting after building code to check data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile, ".", getGrammarName() + "CheckingIfDataMembersAreInMemoryPool", ".C");
#endif

#if 1
	 // AS (10/22/07)
  // --------------------------------------------
  // generate code for return a list of variants in the class hierarchy subtree
  // --------------------------------------------
     string returnClassHierarchySubTreeFileName = string(getGrammarName()) + "ReturnClassHierarchySubTree.C";
     fstream ROSE_returnClassHierarchySubTreeSourceFile(returnClassHierarchySubTreeFileName.c_str(),ios::out);
     ROSE_ASSERT (ROSE_returnClassHierarchySubTreeSourceFile.good() == true);

     ROSE_returnClassHierarchySubTreeSourceFile << includeHeaderString;
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     ROSE_returnClassHierarchySubTreeSourceFile << buildClassHierarchySubTreeFunction();
     cout << "DONE: buildClassHierarchySubTreeFunction()" << endl;
     ROSE_returnClassHierarchySubTreeSourceFile.close();

  // printf ("Exiting after building code to return data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
#endif

#if 1
	 // AS (10/22/07)
  // --------------------------------------------
  // generate code for return a list of variants in the class hierarchy subtree
  // --------------------------------------------
     string memoryPoolTraversalFileName = "AstQueryMemoryPool.h";
     fstream ROSE_memoryPoolTraversalSourceFile(memoryPoolTraversalFileName.c_str(),ios::out);
     ROSE_ASSERT (ROSE_memoryPoolTraversalSourceFile.good() == true);

  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     ROSE_memoryPoolTraversalSourceFile << buildMemoryPoolBasedVariantVectorTraversalSupport();
     cout << "DONE: buildMemoryPoolBasedVariantVectorTraversalSupport()" << endl;
     ROSE_memoryPoolTraversalSourceFile.close();

  // printf ("Exiting after building code to return data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
#endif



#if 1
  // --------------------------------------------
  // generate code for returning data member pointers to IR nodes
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_ReturnDataMemberPointersSourceFile;

     ROSE_ReturnDataMemberPointersSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildStringForReturnDataMemberPointersSupport(*rootNode,ROSE_ReturnDataMemberPointersSourceFile);
     cout << "DONE: buildStringForReturnDataMemberPointersSupport()" << endl;

  // printf ("Exiting after building code to return data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_ReturnDataMemberPointersSourceFile, ".", getGrammarName() + "ReturnDataMemberPointers", ".C");
#endif

#if 1
  // --------------------------------------------
  // generate code for returning data member pointers to IR nodes
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_ReturnDataMemberReferenceToPointersSourceFile;

     ROSE_ReturnDataMemberReferenceToPointersSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildStringForReturnDataMemberReferenceToPointersSupport(*rootNode,ROSE_ReturnDataMemberReferenceToPointersSourceFile);
     cout << "DONE: buildStringForReturnDataMemberReferenceToPointersSupport()" << endl;

  // printf ("Exiting after building code to return references data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_ReturnDataMemberReferenceToPointersSourceFile, ".", getGrammarName() + "ReturnDataMemberReferenceToPointers", ".C");
#endif

#if 1
  // --------------------------------------------
  // generate code for getChildIndex at IR nodes
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_GetChildIndexSourceFile;

     ROSE_GetChildIndexSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

  // DQ(10/22/2007): fixed missed variable renaming.
  // buildStringForGetChildIndexSupport(*rootNode,ROSE_ReturnDataMemberReferenceToPointersSourceFile);
     buildStringForGetChildIndexSupport(*rootNode,ROSE_GetChildIndexSourceFile);
     cout << "DONE: buildStringForGetChildIndexSupport()" << endl;

  // printf ("Exiting after building code to get the child index from any IR node \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_GetChildIndexSourceFile, ".", getGrammarName() + "GetChildIndex", ".C");
#endif

#if 0
   //--------------------------------------------
   // generate code for the file I/O support
   //--------------------------------------------
     string traverseMemoryPoolSourceFileName = string(getGrammarName()) + "TraverseMemoryPool.C";
     ofstream ROSE_TraverseMemoryPoolSourceFile(traverseMemoryPoolSourceFileName.c_str());
     ROSE_ASSERT (ROSE_TraverseMemoryPoolSourceFile.good() == true);

     ROSE_TraverseMemoryPoolSourceFile << includeHeaderString;
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildTraverseMemoryPoolSupport(*rootNode,ROSE_TraverseMemoryPoolSourceFile);
     cout << "DONE: buildTraverseMemoryPoolSupport()" << endl;
     ROSE_TraverseMemoryPoolSourceFile.close();

  // printf ("Exiting after building traverse memory pool functions \n");
  // ROSE_ASSERT(false);
#endif



#if 1
  // --------------------------------------------
  // generate code for the copy member functions
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_CopyMemberFunctionsSourceFile;

     ROSE_CopyMemberFunctionsSourceFile << includeHeaderString;
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildCopyMemberFunctions(*rootNode,ROSE_CopyMemberFunctionsSourceFile);
     cout << "DONE: buildCopyMemberFunctions()" << endl;

  // printf ("Exiting after copy member functions \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_CopyMemberFunctionsSourceFile, ".", getGrammarName() + "CopyMemberFunctions", ".C");
#endif


  //---------------------------------------------------------------------------------------------
  // generate a function for each node in the AST to return the node's successors of the traversal   
  //---------------------------------------------------------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_treeTraversalFunctionsSourceFile;
     cout << "Calling buildTreeTraversalFunctions() ..." << endl;
  // Write header string to file (it's the same string as above, we just reuse it)
     ROSE_treeTraversalFunctionsSourceFile << includeHeaderString;

  // DQ (12/31/2005): Insert "using namespace std;" into the source file (but never into the header files!)
     ROSE_treeTraversalFunctionsSourceFile << "\n// Simplify code by using std namespace (never put into header files since it effects users) \nusing namespace std;\n\n";

  // Generate the implementations of the tree traversal functions
     buildTreeTraversalFunctions(*rootNode, ROSE_treeTraversalFunctionsSourceFile);
     cout << "DONE: buildTreeTraversalFunctions()" << endl;
     Grammar::writeFile(ROSE_treeTraversalFunctionsSourceFile, ".", getGrammarName() + "TreeTraversalSuccessorContainer", ".C");

     //---------------------------------------------------------------------------------------------
     // generate what is necessary for SAGE support in AstProcessing classes
     //---------------------------------------------------------------------------------------------
     cout << "building TreeTraversalAccessEnums ... ";
     string treeTraversalClassHeaderFileName = getGrammarName();
     treeTraversalClassHeaderFileName += "TreeTraversalAccessEnums.h";
     ofstream ROSE_treeTraversalClassHeaderFile(treeTraversalClassHeaderFileName.c_str());
     ROSE_ASSERT (ROSE_treeTraversalClassHeaderFile.good() == TRUE);
     ROSE_treeTraversalClassHeaderFile << "// GENERATED HEADER FILE --- DO NOT MODIFY!"
				       << endl << endl;
     ROSE_treeTraversalClassHeaderFile <<  naiveTraverseGrammar(*rootNode, &Grammar::EnumStringForNode);
     cout << "finished." << endl;

  // MS: not really needed because of typeid(node).name()
  // MS: generation of VariantName Strings
     string variantEnumNamesFileName = string(getGrammarName())+"VariantEnumNames.C";
     ofstream variantEnumNamesFile(variantEnumNamesFileName.c_str());
     ROSE_ASSERT(variantEnumNamesFile.good() == TRUE);     
     string  variantEnumNames=buildVariantEnumNames();

#if 0
  // DQ (4/8/2004): Changes this to make it a global variable instead of a member variable 
  //                of a templated class which limits it's use elsewhere (since it is a 
  //                generally useful mechanism to translate variants to strings in debugging).
     variantEnumNamesFile << "\n template<class InheritedAttributeType, class SynthesizedAttributeType>\n"
			  << "const char* " << getGrammarPrefixName() << "TreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::\n"
			  << "variant_name[]={ " << variantEnumNames << "};\n\n";
#else
  // DQ (4/8/2004): Maybe we need a more obscure name to prevent global name space pollution?
     variantEnumNamesFile << "\n const char* roseGlobalVariantNameList[] = { \n" << variantEnumNames << "\n};\n\n";
#endif

  // MS: 2002: Generate the grammar that defines the set of all ASTs as dot and latex file
     ofstream GrammarDotFile("grammar.dot");
     ROSE_ASSERT (GrammarDotFile.good() == TRUE);
     buildGrammarDotFile(rootNode, GrammarDotFile);
     cout << "DONE: buildGrammarDotFile" << endl;
     ofstream GrammarLatexFile("generated_abstractcppgrammar.atg");
     ROSE_ASSERT (GrammarLatexFile.good() == TRUE);
     buildGrammarLatexFile(rootNode, GrammarLatexFile);
     cout << "DONE: buildGrammarLatexFile" << endl;
     string rtiFunctionsSourceFileName = string(getGrammarName())+"RTI.C";
     StringUtility::FileWithLineNumbers rtiFile;
     rtiFile << includeHeaderString;

  // DQ (12/31/2005): Insert "using namespace std;" into the source file (but never into the header files!)
     rtiFile << "\n// Simplify code by using std namespace (never put into header files since it effects users) \nusing namespace std;\n\n";

  // turn this into a code pattern
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgName& n) {\n "
       << "  return os << \"\\\"\" << n.str() << \"\\\"\";\n}\n";

#if 1
  // DQ (3/10/2007): Added output function for SgNodeSet objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, std::set<SgNode*>& s) \n   {\n "
       << "     for (set<SgNode*>::iterator i = s.begin(); i != s.end(); i++) os << *i;\n"
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (9/0/2006): Added output function for Sgxxx objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgAsmStmt::AsmRegisterNameList & bv) \n   {\n "
       << "     for (unsigned int i=0; i < bv.size(); i++) os << ((long)(bv[i]));\n"
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (12/6/2003): Added output function for SgBitVector objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, std::vector<bool>& bv) \n   {\n "
       << "     for (unsigned int i=0; i < bv.size(); i++) os << ((bv[i] == true) ? \"T\" : \"F\");\n"
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (12/6/2003): Added output function for SgBitVector objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, std::set<int>& s) \n   {\n "
       << "     for (set<int>::iterator i = s.begin(); i != s.end(); i++) os << *i;\n"
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (12/20/2005): Added output function for support of map<int, string> in Sg_File_Info objects
     rtiFile
       << "std::ostream& operator<<(std::ostream& os, std::map<int, std::string>& s) \n   {\n "
       << "     for (std::map<int, std::string>::iterator i = s.begin(); i != s.end(); i++) os << i->first;\n"
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (12/20/2005): Added output function for support of map<string, int> in Sg_File_Info objects
     rtiFile
       << "std::ostream& operator<<(std::ostream& os, std::map<std::string, int>& s) \n   {\n "
       << "     for (std::map<std::string, int>::iterator i = s.begin(); i != s.end(); i++) os << i->first;\n"
       << "     return os;\n   }\n";
#endif
#if 0
  // DQ (4/13/2004): Added output function for SgTypeModifier objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgTypeModifier& tm) \n   {\n "
       << "     os << tm.get_modifiers(); \n"
       << "     return os;\n   }\n";

  // DQ (4/13/2004): Added output function for SgStorageModifier objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgStorageModifier& sm) \n   {\n "
       << "     os << sm.get_modifier(); \n"
       << "     return os;\n   }\n";

  // DQ (4/13/2004): Added output function for SgAccessModifier objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgAccessModifier& am) \n   {\n "
       << "     os << am.get_modifier(); \n"
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/20/2007): Added output function for SgDataStatementObject objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgDataStatementObjectPtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/20/2007): Added output function for SgDataStatementValue objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgDataStatementValuePtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/21/2007): Added output function for SgCommonBlockObject objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgCommonBlockObjectPtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/21/2007): Added output function for SgDimensionObject objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgDimensionObjectPtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/21/2007): Added output function for SgLabelSymbol objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgLabelSymbolPtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/21/2007): Added output function for SgFormatItem objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgFormatItemPtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
#endif
#if 1
  // DQ (11/21/2007): Added output function for SgAsmSectionHeader objects
     rtiFile 
       << "#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT \n "
       << "std::ostream& operator<<(std::ostream& os, SgAsmSectionHeaderPtrList& bv) \n   {\n "
       << "     return os;\n   }\n";
  // DQ (11/21/2007): Added output function for SgAsmProgramHeader objects
     rtiFile 
       << "std::ostream& operator<<(std::ostream& os, SgAsmProgramHeaderPtrList& bv) \n   {\n "
       << "     return os;\n   }\n"
       << "#endif \n";
#endif


     buildRTIFile(rootNode, rtiFile);
     cout << "DONE: buildRTIFile" << endl;
     Grammar::writeFile(rtiFile, ".", getGrammarName() + "RTI", ".C");

     ofstream CocoScannerSymbolsFile((string(getGrammarName())+"CocoSymbolsMapping.C").c_str());
     ROSE_ASSERT (GrammarDotFile.good() == TRUE);
     CocoScannerSymbolsFile << cocoScannerDefines();

  // DQ (11/27/2005): Support for renaming transformations for ROSE project 
  // part of pre-release work to fixup interface and names of objects within ROSE. 
     string transformationSupportFileName = "translationSupport.code";
     ofstream ROSE_TransformationSupportFile(transformationSupportFileName.c_str());
     ROSE_ASSERT(ROSE_TransformationSupportFile.good() == TRUE);
     string transformationSupportString = buildTransformationSupport();
     ROSE_TransformationSupportFile << transformationSupportString;
     ROSE_TransformationSupportFile.close();

#if 1
   // JH (01/18/2006)
   //--------------------------------------------
   // generate IR node constructor that takes a
   // storage class object
   //--------------------------------------------

     StringUtility::FileWithLineNumbers ROSE_ConstructorTakingStorageClassSourceFile;

     ROSE_ConstructorTakingStorageClassSourceFile << includeHeaderString;
  // JH (01/18/2006) Adding additionally the header of StorageClasses
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildIRNodeConstructorOfStorageClassSource(*rootNode,ROSE_ConstructorTakingStorageClassSourceFile);
     cout << "DONE: buildConstructorTakingStorageClass()" << endl;

  // printf ("Exiting after building traverse memory pool functions \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_ConstructorTakingStorageClassSourceFile, ".", "SourcesOfIRNodesAstFileIOSupport", ".C");
#endif

  /////////////////////////////////////////////////////////////////////////////////////////////
  // JH(10/26/2005): Build files for ast file io
  //   * AST_FILE_IO.h
  //   * AST_FILE_IO.C
  //   * StorageClasses.h
  //   * StorageClasses.C
                                                                                                                                                                                                                          
     Grammar::generateAST_FILE_IOFiles();
     Grammar::generateStorageClassesFiles();
  /////////////////////////////////////////////////////////////////////////////////////////////



#if 1
  // -----------------------------------------------------------------------------------------------------------------------
  // generate code for new form of constructor without source position information (this code generation must be done LAST!)
  // -----------------------------------------------------------------------------------------------------------------------

     StringUtility::FileWithLineNumbers ROSE_NewConstructorsSourceFile;

     ROSE_NewConstructorsSourceFile << includeHeaderString;

  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

  // Modify the tree to mark the Sg_File_Info* in the SgLocatedNode to NOT 
  // be a constructor parameter. Then regenerate the code.
     markNodeForConstructorWithoutSourcePositionInformationSupport(*rootNode);

     buildConstructorWithoutSourcePositionInformationSupport (*rootNode,ROSE_NewConstructorsSourceFile);
  // cout << "DONE: buildConstructorWithoutSourcePositionInformationSupport()" << endl;

  // printf ("Exiting after building code for new constructors without source position information \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_NewConstructorsSourceFile, ".", getGrammarName() + "NewConstructors", ".C");
#endif

#if 1
     string outputClassesAndFieldsSourceFileName = string(getGrammarName()) + "ClassesAndFields.txt";
     ofstream ROSE_outputClassesAndFieldsSourceFile(outputClassesAndFieldsSourceFileName.c_str());
     ROSE_ASSERT (ROSE_outputClassesAndFieldsSourceFile.good() == true);

     printf ("Calling outputClassesAndFields() \n");
  // outputClassesAndFields ( *rootNode, ROSE_outputClassesAndFieldsSourceFile);
     ROSE_outputClassesAndFieldsSourceFile << outputClassesAndFields ( *rootNode );
#endif

     return;
   }

//////////////////////////////////
// GRAMMAR OUTPUT/VISUALIZATION //
//////////////////////////////////
// MS: 2002
Grammar::GrammarSynthesizedAttribute 
Grammar::CreateGrammarDotString(GrammarTreeNode* grammarnode,
				  vector<GrammarSynthesizedAttribute> v) {
  GrammarSynthesizedAttribute saDot;
  string s;

  if(grammarnode->isNonTerminal()) {
    // (traversed) data member information for current grammar node
    list<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(list<GrammarString*>::iterator stringListIterator = includeList.begin();
      stringListIterator != includeList.end();
	stringListIterator++) {
      if ((*stringListIterator)->getToBeTraversed()) {
	string type = (*stringListIterator)->getTypeNameString();
	type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
	type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
	type=GrammarString::copyEdit (type,"*","");
	s+=string(grammarnode->getName())+" -> "+type
	  +" [label="+(*stringListIterator)->getVariableNameString()+"];\n";
      }
    }
    // approximation: if the node has at least one successor that is a non-terminal then
    //                create successor information
    // unfortunately GrammarString does not contain this information
    bool createSuccessorInfo=false;
    for(list<GrammarTreeNode *>::iterator succiter=grammarnode->nodeList.begin();
	succiter!=grammarnode->nodeList.end();
	succiter++) {
      if((*succiter)->isNonTerminal())
	createSuccessorInfo=true;
    }

    // inheritance hierarchy information
    // create data for current node (edges to subtree nodes) (only edges for inner nodes)
    if(createSuccessorInfo) {
      for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
	s+=string(grammarnode->getName())+" -> "+(*viter).grammarnode->getName()+" [color=\"blue\"];\n";
      }
    }
  }
  // union data of subtree nodes
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    s+=(*viter).text;
  }
  saDot.grammarnode=grammarnode;
  saDot.text=s;
  return saDot;
}


bool 
Grammar::isAbstractGrammarSymbol(string s) {
  //set<string>::iterator posIter = traversedTerminals.find(s);
  //if (posIter != traversedTerminals.end())
  //  return true;
  //compute all basetypes of the above set and you are done!

  // hack: exclude symbols explicitely which are not traversed but are included in the type hierarchy
  //"SgFunctionType","SgNamedType","SgUnparse_Info"};
  list<string> excl;
  excl.push_back("SgType");
  excl.push_back("SgFunctionType");
  excl.push_back("SgNamedType");
  excl.push_back("SgMemberFunctionType");
  excl.push_back("SgClassType");
  excl.push_back("SgPointerType");
  excl.push_back("SgUnparse_Info");
  excl.push_back("Sg_File_Info");
  for(list<string>::iterator i=excl.begin();i!=excl.end();i++) {
    string::size_type posIter = s.find(*i);
    if (posIter != string::npos)
      return false;
  }
  return true;
}


// MS: 2003
// MS: We compute the set of traversed terminals to restrict the abstract grammar to traversed nodes only.
Grammar::GrammarSynthesizedAttribute
Grammar::CreateMinimalTraversedGrammarSymbolsSet(GrammarTreeNode* grammarnode,
				     vector<Grammar::GrammarSynthesizedAttribute> v) {
  if(grammarnode->isTerminal()) {
    list<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(list<GrammarString*>::iterator stringListIterator = includeList.begin();
	stringListIterator != includeList.end(); // ", " only between the elements of the list
	stringListIterator++) {
      if ( (*stringListIterator)->getToBeTraversed()) {
	traversedTerminals.insert(restrictedTypeStringOfGrammarString(*stringListIterator,grammarnode,"",""));
  
      }
    }  
  }
  GrammarSynthesizedAttribute dummy;
  return dummy;
}

// MS: 2003
string
Grammar::restrictedTypeStringOfGrammarString(GrammarString* gs, GrammarTreeNode* grammarnode, string grammarSymListOpPrefix, string grammarSymListOpPostfix) {
  string type=typeStringOfGrammarString(gs);
  string::size_type posIter = type.find("*");
  if (posIter != string::npos)
    type.replace(posIter, 1, "");
  GrammarNodeInfo gInfo=getGrammarNodeInfo(grammarnode); // MS: should be a member function of GrammarNode
  if(gInfo.numContainerMembers>0) { // there can be only one container member!
    //cout << "ContainerMembers>0: " << type << endl;
    type = replaceString("PtrList","",type);
    type = replaceString("List","",type); // only SgInitializedNameList as of 05/20/03, MS
    type = grammarSymListOpPrefix+type+grammarSymListOpPostfix; // EBNF notation for lists
  } else {
    type = replaceString("PtrListPtr","",type);
    //cout << "ContainerMembers<=0: " << type << endl;
  }
  return type;
}


// MS: 2002,2003
Grammar::GrammarSynthesizedAttribute 
Grammar::CreateGrammarLatexString(GrammarTreeNode* grammarnode,
				  vector<GrammarSynthesizedAttribute> v) {
  //cout << "Creating grammar latex string:" << endl;
  GrammarSynthesizedAttribute saLatex;
  string s;

  // EBNF generated Grammar symbols (this can be parameterized in future)
  // tree grammar
  string grammarSymTreeLB=" \"(\" ";
  string grammarSymTreeRB=" \")\" ";
  string GrammarSymTreeElSep=""; // "\",\" "; comma no longer necessary
  // kontext free grammar
  string grammarSymArrow=" = ";
  string grammarSymOr     ="    | ";
  string grammarSymEndRule="    .\n";
  string grammarSymListOpPrefix=" { ";
  string grammarSymListOpPostfix=" } ";

    if(grammarnode->isTerminal()) {
      string rhsTerminalSuccessors;
      /*
	bool containermembers=0; // (non-pointer)
	bool singledatamembers=0;      // (pointer or non-pointer)
    */
      list<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
      for(list<GrammarString*>::iterator stringListIterator = includeList.begin();
	  stringListIterator != includeList.end(); // ", " only between the elements of the list
	  stringListIterator++) {
	if ( (*stringListIterator)->getToBeTraversed()) {
	  if(rhsTerminalSuccessors!="") // before each element but not the first one
	    rhsTerminalSuccessors+=GrammarSymTreeElSep;
	  /*
	    char* type =new char[300];type[0]='\0';
	    (void) strcat( type, (*stringListIterator)->getTypeNameString());
	    type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
	    type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
	    //type=GrammarString::copyEdit (type,"*","");
	    */
	  string type=restrictedTypeStringOfGrammarString(*stringListIterator,grammarnode, grammarSymListOpPrefix, grammarSymListOpPostfix);
	  rhsTerminalSuccessors+=type+string(" "); // MS: NO VARNAME: +string((*stringListIterator)->getVariableNameString());
	  
	  // based on the type*name* used it is infered whether it is a
	  // a container or a single data member (single pointer or single obj)
	  // this is just a "heuristic" test. Changing the typenames invalidates
	  // it
	  //string stype=string(type);
	  /*
	    if( (stype.find("*") == string::npos) // not found, not a pointer
	    && (stype.find("List") == stype.size()-4) ) // postfix
	    containermembers++;
	    else 
	    singledatamembers++;
	  */
	}
      }
      // assert: s2=="" means that no members are traversed of this terminal 'grammarnode'
      /*
	for(list<GrammarString*>::iterator stringListIterator = includeList.begin();
	stringListIterator != includeList.end(); // ", " only between the elements of the list
	stringListIterator++) {
	if ( !(*stringListIterator)->getToBeTraversed()) {
	if(s3!="") // before each element but not the first one
	s3+=",\n";
	char* type =new char[300];type[0]='\0';
	(void) strcat( type, (*stringListIterator)->getTypeNameString());
	type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
	type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
	type=GrammarString::copyEdit (type,"*","");
	s3+=string("          ")+type+string(" "); //MS: NO VARNAME: +string((*stringListIterator)->getVariableNameString());
	}
	}
	
	if(false) {
	saLatex.nodetext=string(grammarnode->getName())+grammarSymTreeLB+s2+grammarSymTreeRB//+"\n"
	//+"{\n" s3 + "\n}\n"
	;
	}
	else {
      */
      if(isAbstractGrammarSymbol(string(grammarnode->getName())) )
	saLatex.nodetext=string(grammarnode->getName())+" "+grammarSymTreeLB+rhsTerminalSuccessors+grammarSymTreeRB;
      else {
// DQ (5/24/2005): debugging code (not generally required)
// cout << "Terminal " << grammarnode->getName() << " excluded." << endl;
	saLatex.nodetext="";
      }
    } // end of terminal handling
    else {
      if(isAbstractGrammarSymbol(string(grammarnode->getName())) )
	saLatex.nodetext=grammarnode->getName();
      else
	saLatex.nodetext="";
    }
  // create BNF grammar rule for current grammar node and its successors
  string grammarRule;
  bool first=true;
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    if((*viter).nodetext!="" && isAbstractGrammarSymbol(string(grammarnode->getName())) ) {
      if(first) {
	grammarRule+=string(grammarnode->getName()) + grammarSymArrow + (*viter).nodetext+"\n";
	first=false;
      } else {
	grammarRule+=grammarSymOr + (*viter).nodetext+"\n";
      }
    }
  }
  if(v.size()>0 && isAbstractGrammarSymbol(string(grammarnode->getName())) )
    grammarRule+=grammarSymEndRule;

  // union data of subtree nodes
  saLatex.grammarnode=grammarnode;
  saLatex.text=grammarRule;
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    saLatex.text+=(*viter).text;
  }
  
  // create problematic node info
  GrammarNodeInfo gInfo=getGrammarNodeInfo(grammarnode); // MS: should be a member function of GrammarNode
  if(gInfo.numSingleDataMembers>0 && gInfo.numContainerMembers>0) 
    saLatex.problematicnodes+=string(grammarnode->getName())+"\n";

  // ------------------------------------------------------------
  // create terminal and nonterminal (and problematic node) lists 
  // ------------------------------------------------------------

  // create terminal or non-terminal entry
  if(grammarnode->isTerminal()) {
    saLatex.terminalsbunch+=string(grammarnode->getName())+"\n";
  } else {
    saLatex.nonterminalsbunch+=string(grammarnode->getName())+"\n";
  }
  // union non-terminal, terminal, and problematic nodes data of subtree nodes
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    // union subtrees
    saLatex.nonterminalsbunch+=(*viter).nonterminalsbunch;
    saLatex.terminalsbunch+=(*viter).terminalsbunch;
    saLatex.problematicnodes+=(*viter).problematicnodes;
  }
  return saLatex;
}

void Grammar::buildGrammarDotFile(GrammarTreeNode* rootNode, ostream& GrammarDotFile) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::CreateGrammarDotString);
  GrammarDotFile << "digraph G {\n";
  GrammarDotFile << a.text;
  GrammarDotFile << "\n}" << endl;
}

// MS:2002 (should be member function of GrammarString)
string Grammar::typeStringOfGrammarString(GrammarString* gs)
   {
     string type = gs->getTypeNameString();

     type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
     type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");

     return type;
   }

// MS: this function should be a member function of GrammarNode (but this requires several
// other functions to be moved there as well. If we need more functions this will be done.
Grammar::GrammarNodeInfo Grammar::getGrammarNodeInfo(GrammarTreeNode* grammarnode) {
  GrammarNodeInfo info;
  list<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
  for(list<GrammarString*>::iterator stringListIterator = includeList.begin();
      stringListIterator != includeList.end();
      stringListIterator++) {
    if ( (*stringListIterator)->getToBeTraversed()) {
      string stype=typeStringOfGrammarString(*stringListIterator);
   // GB (8/16/2007): Fixed this condition. It did not count SgProject::p_fileList, which is a pointer to a container, and
   // possibly other pointers to containers.
   // if( (stype.find("*") == string::npos) // not found, not a pointer
   // && (stype.find("List") == stype.size()-4) ) // postfix
      if (isSTLContainerPtr(stype.c_str()) || isSTLContainer(stype.c_str())) {
        info.numContainerMembers++;
      } else {
        info.numSingleDataMembers++;
     // GB (8/16/2007): added sanity check
        if (info.numContainerMembers > 0) {
          cout << "Error: in grammar tree node " << grammarnode->getName()
              << ": single member " << (*stringListIterator)->variableNameString
              << " marked for traversal follows a container also marked "
              << "for traversal, that's not allowed" << endl;
          ROSE_ASSERT((info.numSingleDataMembers > 0 ?  info.numContainerMembers == 0 : true));
        }
      }
    }
  }
  if (info.numSingleDataMembers > 0 && info.numContainerMembers > 0) {
 // GB (9/11/2007): After a discussion with Dan and Markus we decided that
 // having both single and container members will be allowed temporarily, but
 // only for SgVariableDeclaration. (SgTypedefDeclaration was also involved
 // in the traversal island issue, but it does not have a container member,
 // so we need not mention it in this code.)
    std::string nodeName = grammarnode->getName();
    std::cout << "both single and container members in node " << nodeName << std::endl;
    ROSE_ASSERT(nodeName == "SgVariableDeclaration");
  }
  return info;
}

// MS:2002
void Grammar::buildGrammarLatexFile(GrammarTreeNode* rootNode, ostream& GrammarLatexFile) {
  GrammarSynthesizedAttribute dummy=BottomUpProcessing(rootNode, &Grammar::CreateMinimalTraversedGrammarSymbolsSet);
#if 0
// DQ (5/24/2005): debugging code (not required generally)
  for(set<string>::iterator i=traversedTerminals.begin();i!=traversedTerminals.end();i++)
     {
       cout << "traversed Terminal: " << *i << endl;
     }
#endif
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::CreateGrammarLatexString);
  /*
  GrammarLatexFile << "Grammar G=<NonTerminals, Terminals, Rules, SgNode>\n\n";
  GrammarLatexFile << "NonTerminals:\n" << a.nonterminalsbunch;
  GrammarLatexFile << "\nTerminals:\n" << a.terminalsbunch;
  GrammarLatexFile << "\nRules:\n" << a.text;
  GrammarLatexFile << "\nProblematic Nodes:\n" << a.problematicnodes;
  GrammarLatexFile << endl;
  */
  //GrammarLatexFile << "$CX /* Generate main module */ \n";
  GrammarLatexFile << "/* Abstract C++ Attribute Grammar */\n";
  GrammarLatexFile << "$CX /* Generate C++ code */\n";
  GrammarLatexFile << "COMPILER SgNode\n #include \"slangs.hpp\"\n\n";
  GrammarLatexFile << "\nTOKENS\n\n" << a.terminalsbunch;
  GrammarLatexFile << "\nPRODUCTIONS\n\n" << a.text;
  GrammarLatexFile << "\nEND SgNode.\n";
  GrammarLatexFile << endl;
  
}

/////////////////////////
// RTI CODE GENERATION //
/////////////////////////
//MS: 2002
void Grammar::buildRTIFile(GrammarTreeNode* rootNode, StringUtility::FileWithLineNumbers& rtiFile) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::generateRTIImplementation);
  string result;
  result += "// generated file\n";
  // container in file scope to avoid multiple (200) template instantiation
  result += "static " + RTIreturnType + " " + RTIContainerName + ";\n\n";
  result += a.text; // synthesized attribute
  rtiFile.push_back(StringUtility::StringWithLineNumber(result, "", 1));
}

Grammar::GrammarSynthesizedAttribute
Grammar::generateRTIImplementation(GrammarTreeNode* grammarnode, vector<GrammarSynthesizedAttribute> v)
   {
     GrammarSynthesizedAttribute sa;

  // MS: 2002
  // simply traverse includeList and generate the same code as for traversalSuccessorContainer
  // start: generate roseRTI() method
     ostringstream ss;
     ss << RTIreturnType << endl
        << grammarnode->getName() << "::roseRTI() {" << endl;
     // << RTIreturnType << " " << RTIContainerName << ";" << endl
     ss << RTIContainerName << ".clear();" << endl;
  // if(grammarnode->isNonTerminal()) {
  // (traversed) data member information for current grammar node
     list<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
     for(list<GrammarString*>::iterator stringListIterator = includeList.begin(); stringListIterator != includeList.end(); stringListIterator++)
        {
       // QY 11/9/04 skip indirect members
          if ((*stringListIterator)->automaticGenerationOfDataAccessFunctions == BUILD_INDIRECT_ACCESS_FUNCTIONS)
               continue; 
       // do it for all data members
          string type = (*stringListIterator)->getTypeNameString();
          type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
          type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
          type=GrammarString::copyEdit (type,"*","");
       // s += string(grammarnode->getName())+" -> "+type
       //   +" [label="+(*stringListIterator)->getVariableNameString()+"];\n";
#if COMPLETERTI
          ss << generateRTICode(*stringListIterator, RTIContainerName);
#endif
        }

     ss << "return "<< RTIContainerName << ";\n}" << endl; // end of function
  // end: roseRTI generation

     string s = string(ss.str());
  // union data of subtree nodes
     for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++)
        {
          s+=(*viter).text;
        }

     sa.grammarnode = grammarnode;
     sa.text = s;

     return sa;
   }

// MS: 2002: generate source for adding RTI information to node (more detailed than C++ RTI info!)
// this info is used in PDF and dot output
string Grammar::generateRTICode(GrammarString* gs, string dataMemberContainerName) {
  string memberVariableName=gs->getVariableNameString();
  string typeString=string(gs->getTypeNameString());
  {
    // REPLACE $GRAMMAR_PREFIX_ in typeString by getGrammarPrefixName() (returns char* const)
    StringUtility::copyEdit(typeString, "$GRAMMAR_PREFIX_", getGrammarPrefixName());
    StringUtility::copyEdit(typeString, "$GRAMMAR_X_MARKER_", "");
  }
  ostringstream ss;
  
  ss << "{ostringstream _ss; ";
#if 0
  if(isSTLContainer(const_cast<char*>(&(typeString.c_str()[string(getGrammarPrefixName()).length()])))
     || ( string(typeString)=="SgSymbolTable" )
     //|| ( string(typeString)=="SgName" )
     || ( string(typeString)=="SgSymbolHashBase::iterator" )
     )
    ss << "_ss << \"<no output operator defined for this type>\";";
  else
    ss << "_ss << p_" << memberVariableName << ";";
#else
// JW (inserted by DQ) (7/23/2004): Allows STL containers to be output and permits 
// more information to be generated for variables.
   if (isSTLContainer(const_cast<char*>(&(typeString.c_str()[string(getGrammarPrefixName()).length()])))) 
     {
       ss << "_ss << \"[\";\n";
       ss << "for (" << typeString.c_str() << "::const_iterator i = p_" << memberVariableName << ".begin(); i != p_" << memberVariableName << ".end(); ++i) {if (i != p_" << memberVariableName << ".begin()) _ss << \", \"; _ss << &(*i);}\n";
       ss << "_ss << \"]\";\n";
     }
    else
     { 
       if(( string(typeString)=="SgVariableSymbol*" || string(typeString) == "SgFunctionSymbol*" || string(typeString) == "SgMemberFunctionSymbol*")) 
          {
            ss << "_ss << p_" << memberVariableName << ";\n";
            ss << "if (p_" << memberVariableName << ")\n";
         // DQ (1/20/2006): This fails for 64 bit machines (removed the cast of pointer to int)
         // ss << "  _ss << \": varsym \" << p_" << memberVariableName << "->get_name().str() << \" declared at 0x\" << std::hex << (int)(p_" << memberVariableName << "->get_declaration());\n";
            ss << "  _ss << \": varsym \" << p_" << memberVariableName << "->get_name().str() << \" declared at 0x\" << std::hex << (p_" << memberVariableName << "->get_declaration());\n";
          }
         else 
          { 
            if(( string(typeString)=="SgSymbolTable" )
            // || ( string(typeString)=="SgName" )
               || ( string(typeString)=="SgSymbolHashBase::iterator" ))
               ss << "_ss << \"<no output operator defined for this type>\";";
            else
               ss << "_ss << p_" << memberVariableName << ";";
          }
     }
#endif
     ss << dataMemberContainerName << ".push_back(new RTIMemberData(" 
     << "\"" << typeString << "\"" << ", "
     << "\"p_" << memberVariableName << "\"" << ", "
     << "_ss.str()"
     << "));}" << endl;
     string s=ss.str();
  return s;
}


//======================================================================
// BUILD TRAVERSAL SUCCESSOR CONTAINER CREATION CODE
//======================================================================
// MS: This method is used to write the individual tree traversal functions to
// the specified output file. It only generates the code for creating a
// container of successors (of AST nodes) at run time.
// The C++ inheritance mechanism allows us to only use the local lists for
// introducing data members. Consequently this function only calls
// generateStringListsFromLocalLists() since this is enough.
void
Grammar::buildTreeTraversalFunctions(GrammarTreeNode& node, StringUtility::FileWithLineNumbers& outputFile)
   {
     string successorContainerName="traversalSuccessorContainer";

     if (isAstObject(node))
        {
       // Determine the data members to be investigated (starting at the root of the grammar)
          list<GrammarString *> includeList;
          list<GrammarString *> excludeList;
          list<GrammarString *>::iterator stringListIterator;

          ROSE_ASSERT(includeList.size() == 0);
          ROSE_ASSERT(excludeList.size() == 0);
       // See the note at the beginning of this member function!
       // BP : 10/09/2001, modified to give address of function
          generateStringListsFromLocalLists(node,includeList,excludeList, &Terminal::getMemberDataPrototypeList);

       // Now edit the lists to remove elements appearing within the exclude list
          editStringList(includeList,excludeList);

       // MS: generate the reduced list of traversed data members
          list<GrammarString*> traverseDataMemberList;
          for(stringListIterator = includeList.begin(); stringListIterator != includeList.end(); stringListIterator++)
             {
               if ((*stringListIterator)->getToBeTraversed())
                  {
                    traverseDataMemberList.push_back(*stringListIterator);
                  }
             }
       // start: generate get_traversalSuccessorContainer() method
          outputFile << "     vector<" << grammarPrefixName << "Node*>\n" 
                     << node.getName() << "::get_traversalSuccessorContainer() {\n"
                     << "vector<" << grammarPrefixName << "Node*> " << successorContainerName << ";\n";
       // GB (8/1/2007): Preallocating the memory needed for the traversal successors to avoid frequent reallocations on
       // push_back. This makes things a little more efficient.
          if (traverseDataMemberList.size() > 0)
             {
               outputFile << successorContainerName << ".reserve("
                          << generateNumberOfSuccessorsComputation(traverseDataMemberList, successorContainerName)
                          << ");\n";
             }
          for(list<GrammarString*>::iterator iter=traverseDataMemberList.begin(); iter!=traverseDataMemberList.end(); iter++)
             {
            // GB (8/13/2007): When generating traversal successors, the right thing is almost always a call to
            // generateTraverseSuccessor(), but there are a few cases where we need extra logic. At the moment
            // these are the type definitions that may occur in typedef or variable declarations.
               GrammarString *gs = *iter;
               string nodeName = node.getName();
               string memberVariableName = gs->getVariableNameString();
               if (nodeName == "SgTypedefDeclaration" && memberVariableName == "declaration")
                  {
                    outputFile << successorContainerName << ".push_back(compute_baseTypeDefiningDeclaration());\n";
                  }
               else if (nodeName == "SgVariableDeclaration" && memberVariableName == "baseTypeDefiningDeclaration")
                  {
                    outputFile << successorContainerName << ".push_back(compute_baseTypeDefiningDeclaration());\n";
                  }
            // GB (09/26/2007): This case used to be handled by AstSuccessorsSelectors, but that's no good with the
            // index based traversals. Only traverse a class declaration's definition member if the isForward flag is
            // false.
               else if ((nodeName == "SgClassDeclaration" || nodeName == "SgTemplateInstantiationDecl") && memberVariableName == "definition")
                  {
                    outputFile << successorContainerName << ".push_back(compute_classDefinition());\n";
                  }
               else
                  {
                 // normal case
                    outputFile << generateTraverseSuccessor(*iter, successorContainerName);
                  }
             }
          outputFile << "return "<< successorContainerName << ";\n}\n";
       // end: generate get_traversalSuccessorContainer() method


       // start: generate get_traversalSuccessorNamesContainer() method
          outputFile << "vector<string>\n" 
                     << node.getName() << "::get_traversalSuccessorNamesContainer() {\n"
                     << "vector<string> " << successorContainerName << ";\n";
       // GB (8/16/2007): Moved the variable i here. It is initialized to the number of single traversal successors and
       // will be the starting index for the numbering of container successors (if any).
          GrammarNodeInfo info = getGrammarNodeInfo(&node);
          if (info.numContainerMembers)
             {
               outputFile << "int i = " << StringUtility::numberToString(info.numSingleDataMembers) << ";\n";
             }
          for(list<GrammarString*>::iterator iter=traverseDataMemberList.begin(); iter!=traverseDataMemberList.end(); iter++)
             {
               outputFile << generateTraverseSuccessorNames(*iter, successorContainerName);
             }
          outputFile << "return "<< successorContainerName << ";\n}\n"; // end of function
       // end: generate get_traversalSuccessorNamesContainer() method


       // start: generate get_numberOfTraversalSuccessors() method
       // GB (09/25/2007): Added this method.
          outputFile << "size_t\n"
                     << node.getName() << "::get_numberOfTraversalSuccessors() {\n";
          if (traverseDataMemberList.size() > 0)
             {
               outputFile << "return "
                          << generateNumberOfSuccessorsComputation(traverseDataMemberList, successorContainerName)
                          << ";\n";
             }
          else
             {
               outputFile << "return 0;\n";
             }
          outputFile << "}\n";
       // end: generate get_numberOfTraversalSuccessors() method


       // start: generate get_traversalSuccessorByIndex() method
       // GB (09/25/2007): Added this method.
          outputFile << "SgNode *\n"
                     << node.getName() << "::get_traversalSuccessorByIndex(size_t idx) {\n";
          if (traverseDataMemberList.size() > 0)
             {
               GrammarString *gs = traverseDataMemberList.front();
               string typeString = gs->getTypeNameString();
            // Exceptional case first: SgVariableDeclaration, which has a fixed member (that we compute using a special
            // function) followed by a container.
               if (string(node.getName()) == "SgVariableDeclaration")
                  {
                    outputFile << "if (idx == 0) return compute_baseTypeDefiningDeclaration();\n"
                               << "else return p_variables[idx-1];\n";
                  }
               else if (isSTLContainerPtr(typeString.c_str()))
                  {
                    outputFile << "ROSE_ASSERT(idx < p_" << gs->getVariableNameString() << "->size());\n";
                    outputFile << "return (*p_" << gs->getVariableNameString() << ")[idx];\n";
                  }
               else if (isSTLContainer(typeString.c_str()))
                  {
                    outputFile << "ROSE_ASSERT(idx < p_" << gs->getVariableNameString() << ".size());\n";
                    outputFile << "return p_" << gs->getVariableNameString() << "[idx];\n";
                  }
               else
                  {
                 // Fixed members, generate a switch.
                    outputFile << "switch (idx) {\n";
                    list<GrammarString*>::iterator iter;
                    size_t counter = 0;
                    for (iter = traverseDataMemberList.begin(); iter != traverseDataMemberList.end(); ++iter)
                       {
                         string memberVariableName = (*iter)->getVariableNameString();
                      // Special case: SgTypedefDeclaration has a member that is computed using a special function.
                         if (string(node.getName()) == "SgTypedefDeclaration" && memberVariableName == "declaration")
                            {
                              outputFile << "case " << StringUtility::numberToString(counter++) << ": "
                                         << "return compute_baseTypeDefiningDeclaration();\n";
                            }
                      // Special case: SgClassDeclaration has a member that is computed using a special function. That
                      // member is inherited by SgTemplateInstantiationDecl!
                         else if ((string(node.getName()) == "SgClassDeclaration" || string(node.getName()) == "SgTemplateInstantiationDecl") && memberVariableName == "definition")
                            {
                              outputFile << "case " << StringUtility::numberToString(counter++) << ": "
                                         << "return compute_classDefinition();\n";
                            }
                         else
                            {
                              outputFile << "case " << StringUtility::numberToString(counter++) << ": " << "return p_" << memberVariableName << ";\n";
                            }
                       }
                 // Reaching the default case is an error.
                    outputFile << "default: cout << \"invalid index \" << idx << "
                        << "\" in get_traversalSuccessorByIndex()\" << endl;\n"
                        << "ROSE_ASSERT(false);\n";
                 // Close the switch.
                    outputFile << "}\n";
                  }
             }
          else
             {
            // There are no successors, so calling this function was an error. Complain.
               outputFile << "cout << \"error: get_traversalSuccessorByIndex called on node of type \" << \""
                          << node.getName() << "\" << \" that has no successors!\" << endl;\n"
                          << "ROSE_ASSERT(false);\n";
             }
          outputFile << "}\n";
       // end: generate get_traversalSuccessorByIndex() method


       // start: generate get_childIndex() method
       // GB (09/25/2007): Added this method.
          outputFile << "size_t\n"
                     << node.getName() << "::get_childIndex(SgNode *child) {\n";
          if (traverseDataMemberList.size() > 0)
             {
               GrammarString *gs = traverseDataMemberList.front();
               string typeString = gs->getTypeNameString();
            // Exceptional case first: SgVariableDeclaration, which has a fixed member (that we compute using a special
            // function) followed by a container.
               if (string(node.getName()) == "SgVariableDeclaration")
                  {
                    outputFile << "if (child == compute_baseTypeDefiningDeclaration()) return 0;\n"
                               << "else {\n"
                               << "SgInitializedNamePtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);\n"
                               << "if (itr != p_variables.end()) return (itr - p_variables.begin()) + 1;\n"
                               << "else return (size_t) -1;\n"
                               << "}\n";
                  }
               else if (isSTLContainerPtr(typeString.c_str()))
                  {
                    string memberVariableName = gs->getVariableNameString();
                    string begin = "p_" + memberVariableName + "->begin()";
                    string end = "p_" + memberVariableName + "->end()";
                    outputFile << getIteratorString(typeString.c_str()) << " itr = find(" << begin << ", " << end << ", child);\n"
                               << "if (itr != " << end << ") return itr - " << begin << ";\n"
                               << "else return (size_t) -1;\n";
                  }
               else if (isSTLContainer(typeString.c_str()))
                  {
                    string memberVariableName = gs->getVariableNameString();
                    string begin = "p_" + memberVariableName + ".begin()";
                    string end = "p_" + memberVariableName + ".end()";
                    outputFile << getIteratorString(typeString.c_str()) << " itr = find(" << begin << ", " << end << ", child);\n"
                               << "if (itr != " << end << ") return itr - " << begin << ";\n"
                               << "else return (size_t) -1;\n";
                  }
               else
                  {
                 // Fixed members, generate an if-else ladder.
                    list<GrammarString*>::iterator iter;
                    size_t counter = 0;
                    for (iter = traverseDataMemberList.begin(); iter != traverseDataMemberList.end(); ++iter)
                       {
                         string memberVariableName = (*iter)->getVariableNameString();
                      // Special case: SgTypedefDeclaration has a member that is computed using a special function.
                         if (string(node.getName()) == "SgTypedefDeclaration" && memberVariableName == "declaration")
                            {
                              outputFile << "if (child == compute_baseTypeDefiningDeclaration()) return " << StringUtility::numberToString(counter++) << ";\n"
                                         << "else ";
                            }
                      // Special case: SgClassDeclaration has a member that is computed using a special function.
                         if ((string(node.getName()) == "SgClassDeclaration" || string(node.getName()) == "SgTemplateInstantiationDecl") && memberVariableName == "definition")
                            {
                              outputFile << "if (child == compute_classDefinition()) return " << StringUtility::numberToString(counter++) << ";\n"
                                         << "else ";
                            }
                         else
                            {
                              outputFile << "if (child == p_" << memberVariableName << ") return " << StringUtility::numberToString(counter++) << ";\n"
                                         << "else ";
                            }
                       }
                 // If execution reaches this point, it's not my child.
                    outputFile << "return (size_t) -1;\n";
                  }
             }
          else
             {
            // There are no successors, so calling this function was an error. Complain.
               outputFile << "cout << \"error: get_childIndex called on node of type \" << \""
                          << node.getName() << "\" << \" that has no successors!\" << endl;\n"
                          << "ROSE_ASSERT(false);\n";
             }
          outputFile << "}\n";
       // end: generate get_childIndex() method
        }
       else
        {
       // *** The tree traversal function for the current class issues an error message
       // *** and causes the program to abort. Build this code string and write it to
       // *** the outputFile

          outputFile << "vector<" << grammarPrefixName << "Node*>\n" << node.getName() << "::get_traversalSuccessorContainer() {\n"
                     << "vector<" << grammarPrefixName << "Node*> " << successorContainerName << ";\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return " << successorContainerName << ";\n }\n\n";

          outputFile << "vector<string>\n" << node.getName() << "::get_traversalSuccessorNamesContainer() {\n"
                     << "vector<string> " << successorContainerName << ";\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return " << successorContainerName << ";\n }\n\n";

       // GB (09/25/2007): Added implementations for the new methods get_numberOfTraversalSuccessors, get_traversalSuccessorByIndex, and get_childIndex.
          outputFile << "size_t\n" << node.getName() << "::get_numberOfTraversalSuccessors() {\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return 42;\n }\n\n";

          outputFile << "SgNode*\n" << node.getName() << "::get_traversalSuccessorByIndex(size_t) {\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return NULL;\n }\n\n";

          outputFile << "size_t\n" << node.getName() << "::get_childIndex(SgNode *) {\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return 42;\n }\n\n";
        }

  // Traverse all nodes of the grammar recursively and build the tree traversal function
  // for each of them
     list<GrammarTreeNode *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.nodeList.begin(); treeNodeIterator != node.nodeList.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT((*treeNodeIterator)->token!=NULL);
          ROSE_ASSERT((*treeNodeIterator)->token->grammarSubTree!=NULL);
          ROSE_ASSERT((*treeNodeIterator)->parentTreeNode!=NULL);
          buildTreeTraversalFunctions(**treeNodeIterator, outputFile);
        }

  // return;
   }

/////////////////////////////////////////////////
// traversalSuccessorContainer Code Generation //
/////////////////////////////////////////////////

// MS: 06/28/02: factored out the loop code generation
string Grammar::generateTraverseSuccessorForLoopSource(string typeString, 
						       string memberVariableName, 
						       string successorContainerName,
						       string successorContainerAccessOperator)
   {
     string travSuccSource="";
     travSuccSource += "   {\n";
  // Build the declaration of the STL iterator
     travSuccSource += "     " + string(getIteratorString(typeString.c_str())) + " iter;\n";
  // Build the loop for iterating on the container

  // DQ (3/22/2007): Added error checking on pointer data members.
     if (successorContainerAccessOperator == "->")
          travSuccSource += "     ROSE_ASSERT(p_" + string(memberVariableName) + " != NULL);\n";
     travSuccSource += "     for (iter = p_" + string(memberVariableName) + successorContainerAccessOperator+"begin();" 
                    +  " iter != p_" + string(memberVariableName) + successorContainerAccessOperator+"end(); iter++)\n";

  // Check whether the STL container contains pointers or not
     if (typeString.find("PtrList") != string::npos || typeString.find("PtrVector") != string::npos)
          travSuccSource += "          " + successorContainerName + ".push_back(*iter);\n"; // It contains pointers to AST objects
       else
          travSuccSource += "          " + successorContainerName + ".push_back(&(*iter));\n";  // It contains AST objects
     travSuccSource += "        }\n";
     return travSuccSource;
   }

// GB (8/16/2007): Generate the pre-allocation of the traversal successor
// container. We know the size beforehand, so calling reserve saves some time
// as it avoids repeated reallocations on push_back. The size of the container
// is the sum of the number of single members and the size of the optional
// container member.
string Grammar::generateNumberOfSuccessorsComputation(
        list<GrammarString*>& traverseDataMemberList, string successorContainerName)
{
    stringstream travSuccSource;
    if (!traverseDataMemberList.empty())
    {
        list<GrammarString *>::iterator iter;
        int singleSuccessors = 0, containerSuccessors = 0;
        for (iter = traverseDataMemberList.begin(); iter != traverseDataMemberList.end(); ++iter)
        {
            string typeString = (*iter)->getTypeNameString();
            string memberVariableName = (*iter)->getVariableNameString();
            if (isSTLContainerPtr(typeString))
            {
                containerSuccessors++;
                travSuccSource << "p_" << memberVariableName << "->size()";
            }
            else if (isSTLContainer(typeString))
            {
                containerSuccessors++;
                travSuccSource << "p_" << memberVariableName << ".size()";
            }
            else
            {
                singleSuccessors++;
                // If this is a single successor, no container may come before
                // it as that would break the traversal successor enums.
                if (containerSuccessors > 0)
                {
                    cout << "Error: traversal successor " << memberVariableName
                        << " is preceded by a container that is also "
                        << "traversed; this is not allowed";
                    ROSE_ASSERT((singleSuccessors > 0 ? containerSuccessors == 0 : true));
                }
            }
            if (containerSuccessors > 1)
            {
                cout << "Error: traversal successor " << memberVariableName
                    << " is a container preceded by another container that is "
                    << "also traversed; this is not allowed";
                ROSE_ASSERT(containerSuccessors <= 1);
            }
        }

        // In general, the result of this function will be something like 'p_foo.size()+42' or '+23'.
        // The + is unary or binary depending on context, no need to worry about it. It is forbidden to have more than
        // one container.
        travSuccSource << "+" << singleSuccessors;
    }
    return travSuccSource.str();
}

// MS: 03/11/02: new TraversalMechanism
// generate source for adding successors of a node to the successors container.
string Grammar::generateTraverseSuccessor(GrammarString* gs, string successorContainerName)
   {
     string memberVariableName=gs->getVariableNameString();
     string typeString=gs->getTypeNameString();

  // MS: sstream should be used here in future
     string travSuccSource="";

#if 1
  // DQ (5/8/2005): Debugging code introduced to debug enclusion of SgDirectory concept!
     travSuccSource += string("/* typeString = ") + string(typeString) + string(" */\n");
     travSuccSource += string("/* isSTLContainer   (typeString) = ") + 
                       string((isSTLContainer   (typeString) == true ? "true" : "false")) + string(" */\n");
     travSuccSource += string("/* isSTLContainerPtr(typeString) = ") + 
                       string((isSTLContainerPtr(typeString) == true ? "true" : "false")) + string(" */\n");
#endif

     if (isSTLContainerPtr(typeString))
        {
          travSuccSource += generateTraverseSuccessorForLoopSource(typeString,memberVariableName,successorContainerName,"->");
        }
       else
        {
          if (isSTLContainer(typeString))
             {
               travSuccSource += generateTraverseSuccessorForLoopSource(typeString,memberVariableName,successorContainerName,".");
             }
            else
             {
            // ***********************************************************************
            // The data member to be visited is not a container (it is a single object)
            // ***********************************************************************
            // Check if the data member has a pointer type in which case
            // we need the "->" operator. Otherwise we need the "." operator
               if (typeString.find('*') != string::npos)
                  {
                    travSuccSource += successorContainerName + ".push_back(p_"+ memberVariableName + ");\n"; // It is a pointer to an AST object
                  }
                 else
                  {
                 // Does this ever occur?
                    travSuccSource += successorContainerName + ".push_back(&p_" + memberVariableName + ");\n"; // It is an AST object
                  }
             }
        }

     return travSuccSource;
   }

// -------------------------------------------------------------------------------------
// generate a container with Names of the traversed members for a better output (DOT/PDF)
// -------------------------------------------------------------------------------------
// MS: 06/28/02
string Grammar::generateTraverseSuccessorNamesForLoopSource(string typeString, 
						       string memberVariableName, 
						       string successorContainerName,
						       string successorContainerAccessOperator)
   {
     string travSuccSource="";
     travSuccSource += "   {\n";
  // Build the declaration of the STL iterator
     travSuccSource += "     " + string(getIteratorString(typeString.c_str())) + "  iter;\n";
  // GB (8/16/2007): Moved this declaration up to the beginning of the
  // function because we do not necessarily want to count from 0, depending on
  // the other members.
  // travSuccSource += "  int i=0;\n";

  // Build the loop for iterating on the container
  // DQ (3/22/2007): Added error checking on pointer data members.
     if (successorContainerAccessOperator == "->")
          travSuccSource += "     ROSE_ASSERT(p_" + string(memberVariableName) + " != NULL);\n";
     travSuccSource += "     for (iter = p_" + string(memberVariableName) + successorContainerAccessOperator+"begin();" 
                    +  " iter != p_" + string(memberVariableName) + successorContainerAccessOperator+"end(); (iter++,i++)) {\n";

  // Check whether the STL container contains pointers or not
     travSuccSource+="char buf[20];\n";
     if (typeString.find("PtrList") != string::npos || typeString.find("PtrVector") != string::npos)
        {
          travSuccSource+="sprintf(buf,\"*[%d]\",i);\n"; // pointers are represented as '*'
          travSuccSource += successorContainerName + ".push_back(buf);\n"; // It contains pointers to AST objects
        }
       else
        {
          travSuccSource+="sprintf(buf,\"[%d]\",i);\n";
          travSuccSource += successorContainerName + ".push_back(buf);\n";  // It contains AST objects
        }

     travSuccSource += "        }\n   }\n";
     return travSuccSource;
   }

// MS: 06/28/02:
// generate source for adding successor names of a node to the successornames container.
string Grammar::generateTraverseSuccessorNames(GrammarString* gs, string successorContainerName) {
  string memberVariableName=gs->getVariableNameString();
  string typeString=gs->getTypeNameString();

  // MS: sstream should be used here in future
  string travSuccSource="";
  if (isSTLContainerPtr(typeString)) {
    travSuccSource=generateTraverseSuccessorNamesForLoopSource(typeString,memberVariableName,successorContainerName,"->");
  } else if (isSTLContainer(typeString)) {
    travSuccSource=generateTraverseSuccessorNamesForLoopSource(typeString,memberVariableName,successorContainerName,".");
  } else {
    // ***********************************************************************
    // The data member to be visited is not a container (it is a single object)
    // ***********************************************************************
    // Check if the data member has a pointer type in which case
    // we need the "->" operator. Otherwise we need the "." operator
    if (typeString.find('*') != string::npos) {
      travSuccSource = successorContainerName + ".push_back(\"p_"+ memberVariableName + "\");\n"; // It is a pointer to an AST object
    } else {
      // Does this ever occur?
      travSuccSource = successorContainerName + ".push_back(\"&p_" + memberVariableName + "\");\n"; // It is an AST object
    }
  }
  return travSuccSource;
}

void 
Grammar::buildEnumForNode(GrammarTreeNode& node, string& allEnumsString) {
  GrammarNodeInfo info=getGrammarNodeInfo(&node);
// GB (8/16/2007): The distinction between container and non-container nodes
// has been dropped, and so has this code. Instead, we now generate enums
// even for nodes that contain containers; the enum for the container member
// is then the index of the first element of that container, which is neat!
// It also means that we can only allow at most one container per node,
// since the enums for further containers would not correspond to their
// first elements.
#if 0
  if(info.numContainerMembers>0) {
    // do not generate enum for a container node
    strcat(allEnumsString,"// ");
    assert(info.numSingleDataMembers==0); // must hold, or the node is not correct
  }
#endif
  if (info.numContainerMembers > 1) {
    cout << "Error: grammar node " << node.getName() << " has more than one container member" << endl;
    ROSE_ASSERT(info.numContainerMembers <= 1);
  }
  list<GrammarString*> includeList=classMemberIncludeList(node);
  list<GrammarString*>::iterator stringListIterator;
  if (!includeList.empty()) {
    bool isFirst=true;
    for(stringListIterator = includeList.begin();
	stringListIterator != includeList.end();
	stringListIterator++) {
      if ( (*stringListIterator)->getToBeTraversed()) {
	if (isFirst) {
	  allEnumsString += string("enum E_") + node.getName() + " {";
	} else {
	  allEnumsString += ", ";
	}
	isFirst=false;
	allEnumsString += string(node.getName()) + "_" + (*stringListIterator)->getVariableNameString();
      }
    }
    if(!isFirst) {
      allEnumsString += "};\n";
    }
  }
}

string Grammar::EnumStringForNode(GrammarTreeNode& node, string s) {
  // let's reuse the old function for now
  string source;
  buildEnumForNode(node,source);
  return s + source;
}

//////////////////////////////////////////////////////////////////////////////////////////
// GRAMMAR TRAVERSAL
// MS: build a vector of synth attributes for each node of the grammar
// this is a (strongly) simplified version of the BottomUpProcessing class functionality
// Arguments: 1. GrammarNode, 
//            2. a function like evaluateSynthesizedAttribute,
//               with string being the synthesized attribute type
// (can be replaced by MSTL/DSProcessing.C (when finished))
//////////////////////////////////////////////////////////////////////////////////////////
Grammar::GrammarSynthesizedAttribute
Grammar::BottomUpProcessing(GrammarTreeNode* node, 
			      evaluateGAttributeFunctionType evaluateGAttributeFunction) {
  // Traverse all nodes of the grammar recursively and build the synthesized attribute
  // for each of them
  list<GrammarTreeNode *>::iterator treeNodeIterator;
  vector<GrammarSynthesizedAttribute> v;
  for( treeNodeIterator = node->nodeList.begin();
       treeNodeIterator != node->nodeList.end();
       treeNodeIterator++ ) {
    ROSE_ASSERT((*treeNodeIterator)->token!=NULL);
    ROSE_ASSERT((*treeNodeIterator)->token->grammarSubTree!=NULL);
    ROSE_ASSERT((*treeNodeIterator)->parentTreeNode!=NULL);
    v.push_back(BottomUpProcessing(*treeNodeIterator, evaluateGAttributeFunction));
  }
  return (this->*evaluateGAttributeFunction)(node, v);
}

// MS: build a string for each node of the grammar (and concatenate these strings)
// this is a (strongly) simplified version of the BottomUpProcessing class functionality
// Arguments: 1. GrammarNode, 
//            2. a function like evaluateSynthesizedAttribute,
//               with string being the synthesized attribute type
string
Grammar::naiveTraverseGrammar(GrammarTreeNode &node, 
			      evaluateStringAttributeFunctionType evaluateStringAttributeFunction) {
  // Traverse all nodes of the grammar recursively and build the synthesized string attribute
  // for each of them
  list<GrammarTreeNode *>::iterator treeNodeIterator;
  string s;
  for( treeNodeIterator = node.nodeList.begin();
       treeNodeIterator != node.nodeList.end();
       treeNodeIterator++ ) {
    ROSE_ASSERT((*treeNodeIterator)->token!=NULL);
    ROSE_ASSERT((*treeNodeIterator)->token->grammarSubTree!=NULL);
    ROSE_ASSERT((*treeNodeIterator)->parentTreeNode!=NULL);
    s+=naiveTraverseGrammar(**treeNodeIterator, evaluateStringAttributeFunction);
  }
  return (this->*evaluateStringAttributeFunction)(node, s);
}

/////////////////////////////////
// GRAMMAR AUXILIARY FUNCTIONS //
/////////////////////////////////
list<GrammarString*> 
Grammar::classMemberIncludeList(GrammarTreeNode& node) {
  // Determine the data members to be investigated (starting at the root of the grammar)
  list<GrammarString *> includeList;
  list<GrammarString *> excludeList;

  ROSE_ASSERT(includeList.size() == 0);
  ROSE_ASSERT(excludeList.size() == 0);

  // Generate include and exclude list, see function buildTreeTraversalFunctions() which
  // belongs to the 1. implementation of a tree traversal mechnism
  // BP : 10/09/2001, modified to provide address of function
  generateStringListsFromLocalLists(node,includeList,excludeList, &Terminal::getMemberDataPrototypeList); //TODO:This pointer is unsafe (used for NonTerminal objects as well! (MS)
  
  // Now edit the lists to remove elements appearing within the exclude list
  editStringList(includeList,excludeList);  
  return includeList;
}

// MK: This member function is used by the member function buildTreeTraversalFunctions()
// in order to determine if the current node of the grammar corresponds to a grammar
// class whose objects may actually occur in an AST. In a symmetric implementation
// these would exactly be the terminal objects. For the moment we have to be a little
// more careful and treat several classes as special cases ...
bool
Grammar::isAstObject(GrammarTreeNode& node)
{
  string name = node.getName();
  return (isTerminal(node.getName()) ||
	  node.getName().find("AsmInstruction") != string::npos ||
	  node.getName().find("ClassDeclaration") != string::npos ||
	  node.getName().find("FunctionDeclaration") != string::npos ||
	  node.getName().find("BasicBlock") != string::npos ||
	  node.getName().find("ClassDefinition") != string::npos ||
	  node.getName().find("ForStatement") != string::npos ||
  // DQ (11/16/2007): Added special case of FortranDo being an IR node to be traversed.
	  node.getName().find("FortranDo") != string::npos ||
	  node.getName().find("ClassSymbol") != string::npos ||
	  node.getName().find("FunctionSymbol") != string::npos ||
	  node.getName().find("FunctionType") != string::npos || // This includes MemberFunctionType!
	  node.getName().find("ClassType") != string::npos ||
	  node.getName().find("PointerType") != string::npos);
}

// MK: We need this function to determine if the object
// is a pointer to an STL container
bool
Grammar::isSTLContainerPtr(const string& typeString)
{
  if (typeString.find("InitializedNamePtrListPtr") != string::npos ||
      typeString.find("StatementPtrListPtr") != string::npos ||
   // DQ (6/21/2005): Fixup SgBaseClassList to be a list of pointers to SgBaseClass (like everything else)
   // typeString.find("BaseClassListPtr") != string::npos ||
      typeString.find("BaseClassPtrListPtr") != string::npos ||
      typeString.find("NodePtrListPtr") != string::npos ||
      typeString.find("TypePtrListPtr") != string::npos ||
      typeString.find("ExpressionPtrListPtr") != string::npos ||
      typeString.find("AttributePtrListPtr") != string::npos ||
      typeString.find("FilePtrListPtr") != string::npos ||
      typeString.find("DirectoryPtrListPtr") != string::npos ||
   // DQ (11/19/2007): Support for Fortran namelist
      typeString.find("NameGroupPtrListPtr") != string::npos ||
   // DQ (11/20/2007): Support for Fortran data statement
      typeString.find("DataStatementGroupPtrListPtr") != string::npos ||
      typeString.find("ModifierTypePtrVectorPtr") != string::npos)
    return TRUE;
  else
    return FALSE;
}

// MK: We need this function to determine if the object
// is an STL container
bool
Grammar::isSTLContainer(const string& typeString)
{
  if ((typeString.find("InitializedNamePtrList") != string::npos ||
       typeString.find("StatementPtrList") != string::npos ||
    // DQ (3/28/2007): Added to support binary handling (from Jeremiah's patch)
       typeString.find("AsmStatementPtrList") != string::npos ||
    // DQ (3/30/2007): Added to support binary handling
       typeString.find("AsmExpressionPtrList") != string::npos ||
    // DQ (6/21/2005): Fixup SgBaseClassList to be a list of pointers to SgBaseClass (like everything else)
    // typeString.find("BaseClassList") != string::npos ||
       typeString.find("BaseClassPtrList") != string::npos ||
       typeString.find("NodePtrList") != string::npos ||
       typeString.find("TypePtrList") != string::npos ||
       typeString.find("ExpressionPtrList") != string::npos ||
       typeString.find("AttributePtrList") != string::npos ||
       typeString.find("FilePtrList") != string::npos ||
       typeString.find("DirectoryPtrList") != string::npos ||
    // DQ (4/2/2007): Added to list as IR node (this is an STL list as a data member)
       typeString.find("TemplateArgumentPtrList") != string::npos ||
       typeString.find("TemplateParameterPtrList") != string::npos ||
       typeString.find("QualifiedNamePtrList") != string::npos ||
   // DQ (11/19/2007): Support for Fortran namelist
       typeString.find("NameGroupPtrList") != string::npos ||
   // DQ (11/20/2007): Support for Fortran data statement
       typeString.find("DataStatementGroupPtrList") != string::npos ||
       typeString.find("ModifierTypePtrVector") != string::npos) &&
      typeString.find("ListPtr") == string::npos)
    return TRUE;
  else
    return FALSE;
}

string
Grammar::getIteratorString(const string& typeString)
   {
     string iteratorTypeString;

     if (typeString.find("DeclarationStatementPtr") != string::npos)
        {
          return "Rose_STL_Container<" + getGrammarPrefixName() + "DeclarationStatement*>::iterator";
        }

  // DQ (6/1/2004): Changed list to contain pointers to SgInitializedName elements
  // if (strstr(typeString,"InitializedName"))
  //      (void) sprintf(iteratorTypeString,"list<%sInitializedName*>::iterator",getGrammarPrefixName());
     if (typeString.find("InitializedNamePtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "InitializedName*>::iterator";

     if (typeString.find("StatementPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "Statement*>::iterator";

     if (typeString.find("AsmStatementPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "AsmStatement*>::iterator";

     if (typeString.find("BaseClass") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "BaseClass*>::iterator";

     if (typeString.find("NodePtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "Node*>::iterator";

     if (typeString.find("TypePtrList") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "Type*>::iterator";

     if (typeString.find("ExpressionPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "Expression*>::iterator";

  // DQ (3/30/2007): Added to support binary handling (this must appear after the test for ExpressionPtr)
     if (typeString.find("AsmExpressionPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "AsmExpression*>::iterator";

     if (typeString.find("AttributePtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "Attribute*>::iterator";

     if (typeString.find("FilePtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "File*>::iterator";

     if (typeString.find("DirectoryPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "Directory*>::iterator";

     if (typeString.find("QualifiedNamePtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "QualifiedName*>::iterator";

     if (typeString.find("TypePtrVector") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "ModifierType*>::iterator";

  // DQ (4/2/2007): Added support for template argument list being an IR node
     if (typeString.find("TemplateArgumentPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "TemplateArgument*>::iterator";

  // DQ (4/2/2007): Added support for template parameter list being an IR node
     if (typeString.find("TemplateParameterPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "TemplateParameter*>::iterator";

  // DQ (11/20/2007): Support for Fortran data statement
     if (typeString.find("DataStatementGroupPtr") != string::npos)
          iteratorTypeString = "Rose_STL_Container<" + getGrammarPrefixName() + "DataStatementGroup*>::iterator";

     if (iteratorTypeString.empty() == true)
        {
          printf ("Error: unsupported typeString = %s \n",typeString.c_str());
        }
  // ROSE_ASSERT(!iteratorTypeString.empty());
     ROSE_ASSERT( iteratorTypeString.empty() == false );

     return iteratorTypeString;
   }


// MS: that's a more compact version to represent all the naming details in one single data structure
//     but it's not used yet
string
Grammar::getContainerElementTypeString(const string& typeString)
   {
     string containerTypes_elementType[14][3] = {
     // type of Container, type of pointer to container, type of container element
     //                    2. and 3. used in getIteratorString
        {"$$NOTYPE$$","DeclarationStatementPtr","DeclarationStatement*"},
     // DQ (6/1/2004): Changed list to contain pointers to SgInitializedName elements
     // {"InitializedNameList","InitializedName","InitializedName"},
        {"InitializedNamePtrList","InitializedNamePtr","InitializedName*"},
     // DQ (3/28/2007): Added to support binary handling (part of patch from Jeremiah and Thomas)
        {"AsmStatementPtrList","AsmStatementPtr","AsmStatement*"},
     // DQ (3/30/2007): Added to support binary handling
        {"AsmExpressionPtrList","AsmExpressionPtr","AsmExpression*"},
        {"StatementPtrList","StatementPtr","Statement*"},
     // DQ (6/21/2005): Fixup SgBaseClassList to be a list of pointers to SgBaseClass (like everything else)
     // {"BaseClassList","BaseClass","BaseClass"},
        {"BaseClassPtrList","BaseClassPtr","BaseClass*"},
        {"NodePtrList","NodePtr","Node*"},
        {"TypePtrList","TypePtrList","Type*"},
        {"ExpressionPtrList","ExpressionPtr","Expression*"},
        {"AttributePtrList","AttributePtr","Attribute*"},
        {"FilePtrList","FilePtr","File*"},
        {"DirectoryPtrList","DirectoryPtr","Directory*"},
        {"QualifiedNamePtrList","QualifiedNamePtr","QualifiedName*"},
        {"ModifierTypePtrVector","TypePtrVector","ModifierType*"}
     };

  // DQ (3/27/2007): Changed 10 to 13 to reflect correct length of containerTypes_elementType (above)
  // MS replace this with a map
  // for(int i = 0; i < 10; i++)
     for(unsigned int i = 0; i < sizeof(containerTypes_elementType) / sizeof(string[3]); i++)
        {
          if (typeString.find(containerTypes_elementType[i][0]) != string::npos ||
	      typeString.find(containerTypes_elementType[i][1]) != string::npos)
             {
	       return containerTypes_elementType[i][2];
             }
        }

     return "";
   }
