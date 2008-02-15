// ################################################################
// #                           Header Files                       #
// ################################################################

#include "grammar.h"
#include "nonterminal.h"
#include "grammarString.h"

using namespace std;

#define DEBUG_GRAMMAR FALSE

const std::string NonTerminal::separator = "";

// ################################################################
// #                 NonTerminal Member Functions                 #
// ################################################################
void 
NonTerminal::initialize()
   {
  // Initialize the NonTerminal specific variables ONLY

#if 0
  // setName("SetNameInInitialize");
  // setLexeme("NULL_Lexeme_String");
     setName("");
     setLexeme("");
     setTagName("");
#endif

     repeatedNonTerminal = FALSE;
     defaultNonTerminal  = FALSE;

  // Terminal::initialize();
   }

void
NonTerminal::displayName ( int indent ) const
   {
     if (defaultNonTerminal) printf ("[");
     printf ("%s",name.c_str());
     if (repeatedNonTerminal) printf ("+");
     if (defaultNonTerminal) printf ("]");
     printf (" ");
   }

void
NonTerminal::show(int indent) const
{
  list<Terminal*>::const_iterator terminalIterator;
  list<NonTerminal*>::const_iterator nonTerminalIterator;
  
     int i=0;
     for (i=0; i < indent; i++)
        {
          printf ("..");
	}

     //displayName(indent); cout << " -> ";

  // printf ("\n");
     if ( (andTokenPointerArray.size() != 0) || (andNonTerminalPointerArray.size() != 0) )
          printf ("(");

     if ( (orTokenPointerArray.size() != 0) || (orNonTerminalPointerArray.size() != 0) )  {
       //printf ("{"); //MS
     }

     for( terminalIterator = orTokenPointerArray.begin(); 
	  terminalIterator != orTokenPointerArray.end(); 
	  terminalIterator++)
       {
	 ROSE_ASSERT((*terminalIterator)!=NULL);
    displayName(indent); cout << " -> "; (*terminalIterator)->show(); cout << ";" << endl; //MS edge
	 (*terminalIterator)->show(); cout  << "[style=bold];" << endl; // Terminals are bold! MS node
       }

     for( nonTerminalIterator = orNonTerminalPointerArray.begin(); 
	  nonTerminalIterator != orNonTerminalPointerArray.end(); 
	  nonTerminalIterator++)
       {
	 // printf ("show or nonterminal i = %d ",i);
	 ROSE_ASSERT((*nonTerminalIterator)!=NULL);
    displayName(indent); cout << " -> "; (*nonTerminalIterator)->displayName(indent+1); cout << ";" << endl; //MS
       }

     if ( (orTokenPointerArray.size() != 0) || (orNonTerminalPointerArray.size() != 0) ) {
       //printf ("} "); //MS
     }

     for( terminalIterator = andTokenPointerArray.begin(); 
	  terminalIterator != andTokenPointerArray.end(); 
	  terminalIterator++)
       {
	 // printf ("show and token i = %d ",i);
	 ROSE_ASSERT((*terminalIterator)!=NULL);
	 (*terminalIterator)->show();
       }

     for( nonTerminalIterator = andNonTerminalPointerArray.begin(); 
	  nonTerminalIterator != andNonTerminalPointerArray.end(); 
	  nonTerminalIterator++)
       {
	 // printf ("show and nonterminal i = %d ",i);
	 ROSE_ASSERT((*nonTerminalIterator)!=NULL);
	 (*nonTerminalIterator)->displayName(indent+1);
       }

     if ( (andTokenPointerArray.size() != 0) || (andNonTerminalPointerArray.size() != 0) )
          printf (")");
}

NonTerminal::~NonTerminal ()
   {
  // printf ("Inside of NonTerminal Descructor! \n");
     initialize();
     setName("NULL_Name_NonTerminal");
   }

#if 1
NonTerminal::NonTerminal ()
   : Terminal ()
   {
     initialize();

  // printf ("Exiting in default NonTerminal constructor! \n");
  // ROSE_ABORT();
   }
#endif

NonTerminal::NonTerminal ( const Terminal & X )
   : Terminal (X)
   {
  // This function is used in restricted circumstances and thus has a 
  // particular semantics that might not be appropriate for general use!
  // I don't like it but that is the way it is for now!

     //printf ("Inside of NonTerminal (Terminal) Constructor! \n"); // MS

  // Initialize the NonTerminal specific variables
     initialize();

  // ROSE_ASSERT (parentTerminal == NULL);

     if (parentTerminal != NULL)
        {
          display("parentTerminal != NULL in NonTerminal ( const Terminal & X )");
          ROSE_ABORT();
        }

     ROSE_ASSERT (X.parentTerminal == NULL);

#if 0
     initialize();
     setLexeme(X.lexeme);
     setName(X.name);
     setTagName(X.tag);
  // int localLastIndex = getLastOrTokenIndex();
  // ROSE_ASSERT (localLastIndex == 0);
  // orTokenPointerArray[localLastIndex]   = &(Terminal&)X;
#else
  // Set these later (we are only building a temporary so they would make no sense!)
  // initialize();
  // X.display("Inside of NonTerminal ( const Terminal & X )");
  // ROSE_ASSERT (X.parentTerminal == NULL);

  // parentTerminal = NULL;
#endif

     const Terminal *const &tmpRef = &X;
     orTokenPointerArray.push_back ( (Terminal *const &) tmpRef );
  // ROSE_ASSERT (name != NULL);

  // We should have no parents when we are born (they are assigned later)
     ROSE_ASSERT (parentTerminal == NULL);
     ROSE_ASSERT (associatedGrammar != NULL);
   }

#if 0
NonTerminal::NonTerminal ( const NonTerminal & X , Grammar & grammar )
   : Terminal (X)
   {
    *this = X;
     grammar.addGrammarElement(X);
   }
#endif

NonTerminal::NonTerminal ( const NonTerminal & X )
// : Terminal (X) // operator= is sufficient!
   {
     //     printf ("Inside of NonTerminal Copy Constructor! \n");
    *this = X;
  // show();
   }

void NonTerminal::addElement(list<Terminal *> &tList, const Terminal &X)
{
  const Terminal * const &terminalRef = &X;
  tList.push_back( (Terminal * const &) terminalRef );
}

void NonTerminal::addElement(list<NonTerminal *> &nonTList, const NonTerminal &X)
{
  const NonTerminal * const &nonTerminalRef = &X;
  nonTList.push_back( (NonTerminal * const &)nonTerminalRef );
}

void
NonTerminal::clearLists()
   {
  // Empty the lists of reference terminals and nonterminals 

  // Remove the terminals/nonterminals from the lists (but don't deallocate the terminals/nonterminals)
     andTokenPointerArray.clear();
     andNonTerminalPointerArray.clear();
     orTokenPointerArray.clear();
     orNonTerminalPointerArray.clear();
   }


NonTerminal &
NonTerminal::copy ( char* inputLexeme, char* inputTagString )
   {
  // builds child terminals from parent terminals

  // Declare this new terminal on the heap
     NonTerminal* childNonTerminal = new NonTerminal(*this);
     ROSE_ASSERT(childNonTerminal != NULL);

  // Clear the lists of tokens that are defined for the parent 
  // terminal (since they don't make sense on the child)
     childNonTerminal->clearLists();

     childNonTerminal->setName(inputLexeme);
     childNonTerminal->setTagName(inputTagString);
     childNonTerminal->setParentTerminal(this);

     childNonTerminal->copyTerminalData (this);

  // Now add the terminal to the list of terminals in the associated grammar
     ROSE_ASSERT(childNonTerminal != NULL);
     associatedGrammar->addGrammarElement(*childNonTerminal);

     addChild(childNonTerminal);

     ROSE_ASSERT(childNonTerminal->isChild() == TRUE);

  // Error checking
     consistencyCheck();

     return *childNonTerminal;
   }


#if 0
void
NonTerminal::addToOrList ( const Terminal & X )
   {
     orTokenPointerArray.push_back (X);
   }

void
NonTerminal::addToOrList ( const NonTerminal & X )
   {
     orTokenPointerArray.push_back (X);
   }

void
NonTerminal::addToAndList ( const Terminal & X )
   {
     andTokenPointerArray.push_back (X);
   }

void
NonTerminal::addToAndList ( const NonTerminal & X )
   {
     andTokenPointerArray.push_back (X);
   }
#endif

NonTerminal &
NonTerminal::operator= ( const NonTerminal & X )
   {
  // printf ("Inside of NonTerminal::operator= ( const NonTerminal & X ) \n");

     initialize();

//   ROSE_ASSERT (X.variant  >= 0);

     Terminal::operator=(X);
     
     repeatedNonTerminal = X.repeatedNonTerminal;
     defaultNonTerminal  = X.defaultNonTerminal;

  // name = new char[strlen(X.name)+1];
  // strcpy(name,X.name);
  // setName(X.name);
  // setLexeme(X.lexeme);

     orTokenPointerArray        = X.orTokenPointerArray;
     andTokenPointerArray       = X.andTokenPointerArray;
     orNonTerminalPointerArray  = X.orNonTerminalPointerArray;
     andNonTerminalPointerArray = X.andNonTerminalPointerArray;

     associatedGrammar = X.associatedGrammar;
  // ROSE_ASSERT (associatedGrammar != NULL);

     ROSE_ASSERT(X.associatedGrammar != NULL);
  // Can't add this to the nonterminal list since it might only be a temporary used
  // as part of the evaluation of a BNF expression.
  // X.associatedGrammar->addGrammarElement(*this);
     ROSE_ASSERT(associatedGrammar != NULL);

     return *this;
   }

void
NonTerminal::setExpression ( const NonTerminal & X )
   {
  // Unlike the operator= function this function can be reserved 
  // for the specialized assignment of expressions to nonterminals.

     printf ("Inside of NonTerminal::setExpression ( const NonTerminal & X ) \n");

#if 0
     Terminal::operator=(X);
     
     repeatedNonTerminal = X.repeatedNonTerminal;
     defaultNonTerminal  = X.defaultNonTerminal;

  // name = new char[strlen(X.name)+1];
  // strcpy(name,X.name);
  // setName(X.name);
  // setLexeme(X.lexeme);

     ROSE_ASSERT (X.name   != NULL);
     ROSE_ASSERT (X.lexeme != NULL);
#else
     Terminal::setExpression(X);
#endif

     orTokenPointerArray        = X.orTokenPointerArray;
     andTokenPointerArray       = X.andTokenPointerArray;
     orNonTerminalPointerArray  = X.orNonTerminalPointerArray;
     andNonTerminalPointerArray = X.andNonTerminalPointerArray;

  // associatedGrammar = X.associatedGrammar;

     ROSE_ASSERT (associatedGrammar != NULL);
     ROSE_ASSERT(associatedGrammar == X.associatedGrammar);

  // Can't add this to the nonterminal list since it might only be a temporary used
  // as part of the evaluation of a BNF expression.
  // X.associatedGrammar->addGrammarElement(*this);
     ROSE_ASSERT(associatedGrammar != NULL);

     ROSE_ASSERT(associatedGrammar   != NULL);
     ROSE_ASSERT(getParentTerminal() != NULL);

     ROSE_ASSERT(isChild() == TRUE);

     ROSE_ASSERT(automaticGenerationOfConstructor         ==
                 getParentTerminal()->automaticGenerationOfConstructor);
     ROSE_ASSERT(automaticGenerationOfDestructor          ==
                 getParentTerminal()->automaticGenerationOfDestructor);
     ROSE_ASSERT(automaticGenerationOfDataAccessFunctions ==
                 getParentTerminal()->automaticGenerationOfDataAccessFunctions);
     ROSE_ASSERT(automaticGenerationOfCopyFunction        ==
                 getParentTerminal()->automaticGenerationOfCopyFunction);
   }

void
NonTerminal::setExpression ( const Terminal & X )
   {
  // printf ("Not implemented: NonTerminal::setExpression ( const Terminal & X ) \n");
  // ROSE_ABORT();

     Terminal::setExpression(X);

     const Terminal *const &tmpRef = &X;
     orTokenPointerArray.push_back( (Terminal *const &) tmpRef );

     ROSE_ASSERT(associatedGrammar   != NULL);
     ROSE_ASSERT(getParentTerminal() != NULL);

     ROSE_ASSERT(isChild() == TRUE);

     ROSE_ASSERT(automaticGenerationOfConstructor         ==
                 getParentTerminal()->automaticGenerationOfConstructor);
     ROSE_ASSERT(automaticGenerationOfDestructor          ==
                 getParentTerminal()->automaticGenerationOfDestructor);
     ROSE_ASSERT(automaticGenerationOfDataAccessFunctions ==
                 getParentTerminal()->automaticGenerationOfDataAccessFunctions);
     ROSE_ASSERT(automaticGenerationOfCopyFunction        ==
                 getParentTerminal()->automaticGenerationOfCopyFunction);
   }


NonTerminal &
operator| ( const Terminal & X, const Terminal & Y )
   {
  // printf ("operator| ( Terminal X   , Terminal Y ) \n");

     ROSE_ASSERT(X.associatedGrammar != NULL);
     ROSE_ASSERT(Y.associatedGrammar != NULL);

     NonTerminal* result = new NonTerminal();

     ROSE_ASSERT (result != NULL);

#if DEBUG_GRAMMAR
     printf ("Inside of operator| (Terminal,Terminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme.c_str(),X.name.c_str(),Y.lexeme.c_str(),Y.name.c_str());
#endif

  // error checking (looking for where a default named terminal was added to the list)
     ROSE_ASSERT(X.name != "DefaultInternalName");
     ROSE_ASSERT(Y.name != "DefaultInternalName");

     const Terminal *const &refX = &X;
     const Terminal *const &refY = &Y;
     result->orTokenPointerArray.push_back ((Terminal *const &)refX);
     result->orTokenPointerArray.push_back ((Terminal *const &)refY);

  // Now set the associated grammar stored within the terminal/nonterminal
#if 0
     Grammar* grammarInUse = X.getGrammar();
     ROSE_ASSERT(grammarInUse == Y.getGrammar());
     result->setGrammar (grammarInUse);
#else
     ROSE_ASSERT(X.associatedGrammar != NULL);
     ROSE_ASSERT(Y.associatedGrammar != NULL);
     ROSE_ASSERT(X.associatedGrammar == Y.associatedGrammar);
     result->setGrammar (X.associatedGrammar);
#endif

     ROSE_ASSERT (result->associatedGrammar != NULL);

  // cout << "AT THE BOTTOM OF NONTERMINAL:OPERATOR|, RETURNING REFERENCE!!!" << endl;
  // cout << "--------------------------------------------------------------" << endl;
     return *result;
   }

// NONTERMINAL_PARAMETER.show();
// printf ("Adding nonterminal (%s:%s) as a nonterminal \n",NONTERMINAL_PARAMETER.lexeme,NONTERMINAL_PARAMETER.name);
// printf ("Sucking data from nonterminal (%s:%s) into result \n",NONTERMINAL_PARAMETER.lexeme,NONTERMINAL_PARAMETER.name);

// if the input NONTERMINAL_PARAMETER is a temporary from an other expression then
// we absorbe the list in th temporary into the result.
#define MACRO_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER)        \
     if ( (NONTERMINAL_PARAMETER.isTemporary()) ||                                    \
          (NONTERMINAL_PARAMETER.name == "DefaultInternalName") )   \
        {                                                                             \
          list<Terminal *>::const_iterator termIt;                                          \
          for(termIt=NONTERMINAL_PARAMETER.TOKEN_ARRAY.begin(); termIt!=NONTERMINAL_PARAMETER.TOKEN_ARRAY.end(); termIt++) \
               result->TOKEN_ARRAY.push_back(*termIt); \
          list<NonTerminal*>::const_iterator nonTermIt;\
          for(nonTermIt=NONTERMINAL_PARAMETER.NONTERMINAL_ARRAY.begin(); nonTermIt!=NONTERMINAL_PARAMETER.NONTERMINAL_ARRAY.end(); nonTermIt++) \
               result->NONTERMINAL_ARRAY.push_back( *nonTermIt );\
        }                                                                             \
       else                                                                           \
        {                                                                             \
          ROSE_ASSERT(NONTERMINAL_PARAMETER.name != "DefaultInternalName"); \
          const NonTerminal *const &tmpNonTerminalRef = &NONTERMINAL_PARAMETER; \
          result->NONTERMINAL_ARRAY.push_back((NonTerminal *const &) tmpNonTerminalRef);                \
        }

  // NONTERMINAL_PARAMETER.display("In MACRO 'MACRO NonTerminal' result.NONTERMINAL_ARRAY.push_back");

#define MACRO_NonTerminal_Terminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER,TOKEN_PARAMETER) \
     ROSE_ASSERT (result != NULL);      \
     ROSE_ASSERT (Y.associatedGrammar != NULL);      \
     ((Terminal&)X).associatedGrammar = Y.associatedGrammar;      \
     ROSE_ASSERT ( (X.associatedGrammar != NULL) && (Y.associatedGrammar != NULL) );      \
     MACRO_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER) \
     ROSE_ASSERT(TOKEN_PARAMETER.name != "DefaultInternalName"); \
     const Terminal *const &tmpRef = &TOKEN_PARAMETER; \
     result->TOKEN_ARRAY.push_back( (Terminal *const &) tmpRef ); \
     result->setGrammar(X.associatedGrammar);         \
     result->setTemporary(TRUE);                      \
     ROSE_ASSERT (result->associatedGrammar != NULL);

#define MACRO_Terminal_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,TOKEN_PARAMETER,NONTERMINAL_PARAMETER) \
     ROSE_ASSERT (result != NULL);                   \
     ROSE_ASSERT (X.associatedGrammar != NULL);      \
     ((Terminal&)Y).associatedGrammar = X.associatedGrammar; \
     ROSE_ASSERT ( (X.associatedGrammar != NULL) && (Y.associatedGrammar != NULL) );      \
     ROSE_ASSERT(TOKEN_PARAMETER.name != "DefaultInternalName"); \
     const Terminal *const &tmpRef = &TOKEN_PARAMETER; \
     result->TOKEN_ARRAY.push_back( (Terminal *const &) tmpRef ); \
     MACRO_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER) \
     result->setGrammar(X.associatedGrammar); \
     result->setTemporary(TRUE); \
     ROSE_ASSERT (result->associatedGrammar != NULL);

#define MACRO_NonTerminal_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER_X,NONTERMINAL_PARAMETER_Y) \
     ROSE_ASSERT (result != NULL);      \
     ROSE_ASSERT ( (X.associatedGrammar != NULL) || (Y.associatedGrammar != NULL) );      \
     if (X.associatedGrammar == NULL)      \
          ((Terminal&)X).associatedGrammar = Y.associatedGrammar;      \
     if (Y.associatedGrammar == NULL)      \
          ((Terminal&)Y).associatedGrammar = X.associatedGrammar;      \
     ROSE_ASSERT ( (X.associatedGrammar != NULL) && (Y.associatedGrammar != NULL) );      \
     MACRO_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER_X) \
     MACRO_NonTerminal(TOKEN_ARRAY,NONTERMINAL_ARRAY,NONTERMINAL_PARAMETER_Y) \
     result->setGrammar(X.associatedGrammar); \
     result->setTemporary(TRUE); \
     ROSE_ASSERT (result->associatedGrammar != NULL);

NonTerminal &
operator| ( const NonTerminal & X, const Terminal & Y )
   {
     NonTerminal* result = new NonTerminal();

#if DEBUG_GRAMMAR
     printf ("Inside of operator| (NonTerminal,Terminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme.c_str(),X.name.c_str(),Y.lexeme.c_str(),Y.name.c_str());
#endif

     if (X.isTemporary() == TRUE)
          ROSE_ASSERT(X.name == "DefaultInternalName");

     MACRO_NonTerminal_Terminal(orTokenPointerArray,orNonTerminalPointerArray,X,Y)

     return *result;
   }

NonTerminal &
operator| ( const Terminal & X   , const NonTerminal & Y )
   {
     NonTerminal* result = new NonTerminal();

#if DEBUG_GRAMMAR
     printf ("Inside of operator| (Terminal,NonTerminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme.c_str(),X.name.c_str(),Y.lexeme.c_str(),Y.name.c_str());
#endif

     if (Y.isTemporary() == TRUE)
          ROSE_ASSERT(Y.name == "DefaultInternalName");

     MACRO_Terminal_NonTerminal(orTokenPointerArray,orNonTerminalPointerArray,X,Y)

     return *result;
   }

NonTerminal &
operator| ( const NonTerminal & X, const NonTerminal & Y )
   {
     NonTerminal* result = new NonTerminal();

#if DEBUG_GRAMMAR
     printf ("Inside of operator| (NonTerminal,NonTerminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme.c_str(),X.name.c_str(),Y.lexeme.c_str(),Y.name.c_str());
#endif

     if (X.isTemporary() == TRUE)
          ROSE_ASSERT(X.name == "DefaultInternalName");
     if (Y.isTemporary() == TRUE)
          ROSE_ASSERT(Y.name == "DefaultInternalName");

     ROSE_ASSERT (X.associatedGrammar != NULL);
     ROSE_ASSERT (Y.associatedGrammar != NULL);

     MACRO_NonTerminal_NonTerminal(orTokenPointerArray,orNonTerminalPointerArray,Y,X)

     return *result;
   }


NonTerminal &
NonTerminal::operator |= ( const Terminal & Y )
   {
     printf ("EXITING: Inside of NonTerminal::operator|= (Terminal) X(%s:%s)  Y(%s:%s) \n",lexeme.c_str(),name.c_str(),Y.lexeme.c_str(),Y.name.c_str());
     ROSE_ABORT();

     return *this;
   }

NonTerminal &
NonTerminal::operator |= ( const NonTerminal & Y )
   {
#if DEBUG_GRAMMAR
     printf ("Inside of NonTerminal::operator|= (NonTerminal) X(%s:%s)  Y(%s:%s) \n",lexeme.c_str(),name.c_str(),Y.lexeme.c_str(),Y.name.c_str());
#endif

     printf ("ERROR: In NonTerminal::operator=| ( const NonTerminal & Y ): Case of new nonterminal not implemented yet! \n");
     ROSE_ABORT();

     return *this;
   }

#define MACRO_SETUP_LEXEME \
     ROSE_ASSERT (X.lexeme != NULL);                    \
     ROSE_ASSERT (Y.lexeme != NULL);                    \
     ROSE_ASSERT (result.lexeme != NULL);               \
     char* newName = new char [strlen(X.lexeme)+strlen(Y.lexeme)+strlen(NonTerminal::separator)+1]; \
     strcat(newName,X.lexeme);                          \
     strcat(newName,NonTerminal::separator);   \
     strcat(newName,Y.lexeme);                          \
     result.setLexeme ( newName );

// printf ("result.lexeme = %s \n",result.lexeme);

#if 0
// Disable this part of the BNF syntax!

NonTerminal
operator& ( const Terminal & X   , const Terminal & Y )
   {
     NonTerminal result;

#if DEBUG_GRAMMAR
     printf ("Inside of operator& (Terminal,Terminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme,X.name,Y.lexeme,Y.name);
#endif

     result.andTokenPointerArray.push_back(X);
     result.andTokenPointerArray.push_back(Y);

     MACRO_SETUP_LEXEME

     return result;
   }

NonTerminal
operator& ( const NonTerminal & X, const Terminal & Y )
   {
     NonTerminal result;

#if DEBUG_GRAMMAR
     printf ("Inside of operator& (NonTerminal,Terminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme,X.name,Y.lexeme,Y.name);
#endif

     MACRO_NonTerminal_Terminal(andTokenPointerArray,andNonTerminalPointerArray,X,Y)

     MACRO_SETUP_LEXEME

  // printf ("newName = %s \n",newName);
     return result;
   }

NonTerminal
operator& ( const Terminal & X   , const NonTerminal & Y )
   {
     NonTerminal result;

#if DEBUG_GRAMMAR
     printf ("Inside of operator& (Terminal,NonTerminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme,X.name,Y.lexeme,Y.name);
#endif

     MACRO_Terminal_NonTerminal(andTokenPointerArray,andNonTerminalPointerArray,X,Y)

     MACRO_SETUP_LEXEME

     return result;
   }

NonTerminal
operator& ( const NonTerminal & X, const NonTerminal & Y )
   {
     NonTerminal result;

#if DEBUG_GRAMMAR
     printf ("Inside of operator& (NonTerminal,NonTerminal) X(%s:%s)  Y(%s:%s) \n",X.lexeme,X.name,Y.lexeme,Y.name);
#endif

     MACRO_NonTerminal_NonTerminal(andTokenPointerArray,andNonTerminalPointerArray,X,Y)

     MACRO_SETUP_LEXEME

     return result;
   }
#endif

#if 0
// Disable this part of the BNF syntax for now (while we debug other stuff)!

NonTerminal
NonTerminal::operator++ (int)
   {
     NonTerminal result(*this);
     result.repeatedNonTerminal = TRUE;
     return result;
   }

NonTerminal
Terminal::operator++ (int)
   {
     NonTerminal result(*this);
     result.repeatedNonTerminal = TRUE;
     return result;
   }

NonTerminal
NonTerminal::operator[] (NonTerminal & X)
   {
     NonTerminal result = *this & X;
     result.defaultNonTerminal = TRUE;
     ROSE_ASSERT (X.name != NULL);
     ROSE_ASSERT (result.name != NULL);
     return result;
   }

NonTerminal
NonTerminal::operator[] (Terminal & X)
   {
     NonTerminal result = *this & X;
     result.defaultNonTerminal = TRUE;
     ROSE_ASSERT (X.name != NULL);
     ROSE_ASSERT (result.name != NULL);
     return result;
   }
#endif

void
NonTerminal::consistencyCheck() const
   {
     list<Terminal *>::const_iterator terminalIterator;
     list<NonTerminal *>::const_iterator nonTerminalIterator;
  // Call the base class test
     Terminal::consistencyCheck();

  // Local index variable
  // int i = 0;

  // check the list for valid objects
     for( terminalIterator = andTokenPointerArray.begin(); 
	  terminalIterator != andTokenPointerArray.end(); 
	  terminalIterator++)
       {
	 // printf ("terminalList[%d].getName() = %s \n",i,terminalList[i].getName());
	 ROSE_ASSERT((*terminalIterator)!=NULL);
	 (*terminalIterator)->consistencyCheck();
       }

     for( nonTerminalIterator = andNonTerminalPointerArray.begin(); 
	  nonTerminalIterator != andNonTerminalPointerArray.end(); 
	  nonTerminalIterator++)
       {
	 // printf ("terminalList[%d].getName() = %s \n",i,terminalList[i].getName());
	 ROSE_ASSERT((*nonTerminalIterator)!=NULL);
	 (*nonTerminalIterator)->consistencyCheck();
       }


     for( terminalIterator = orTokenPointerArray.begin(); 
	  terminalIterator != orTokenPointerArray.end(); 
	  terminalIterator++)
       {
	 // printf ("terminalList[%d].getName() = %s \n",i,terminalList[i].getName());
	 ROSE_ASSERT((*terminalIterator)!=NULL);
	 (*terminalIterator)->consistencyCheck();
        }

     for( nonTerminalIterator = orNonTerminalPointerArray.begin(); 
	  nonTerminalIterator != orNonTerminalPointerArray.end(); 
	  nonTerminalIterator++)
       {
	 ROSE_ASSERT((*nonTerminalIterator)!=NULL);
	 (*nonTerminalIterator)->consistencyCheck();
       }
   }


void
NonTerminal::display( const char* label ) const
   {
     list<Terminal *>::const_iterator terminalIterator;
     list<NonTerminal *>::const_iterator nonTerminalIterator;

     printf ("In NonTerminal::display ( %s ) \n",label);

     Terminal::display(label);

     for( terminalIterator = orTokenPointerArray.begin(); 
	  terminalIterator != orTokenPointerArray.end(); 
	  terminalIterator++)
       {
	 ROSE_ASSERT((*terminalIterator)!=NULL);
	 string nameInProductionRule = (*terminalIterator)->name;
	 printf ("     OR Terminal:    nameInProductionRule = %s \n",nameInProductionRule.c_str());
       }

     for( nonTerminalIterator = orNonTerminalPointerArray.begin(); 
	  nonTerminalIterator != orNonTerminalPointerArray.end(); 
	  nonTerminalIterator++)
       {
	 ROSE_ASSERT((*nonTerminalIterator)!=NULL);
	 string nameInProductionRule = (*nonTerminalIterator)->name;
	 printf ("     OR NonTerminal: nameInProductionRule = %s \n",nameInProductionRule.c_str());
       }

     for( terminalIterator = andTokenPointerArray.begin(); 
	  terminalIterator != andTokenPointerArray.end(); 
	  terminalIterator++)
       {
	 ROSE_ASSERT((*terminalIterator)!=NULL);
	 string nameInProductionRule = (*terminalIterator)->name;
	 printf ("     AND Terminal:    nameInProductionRule = %s \n",nameInProductionRule.c_str());
       }

     for( nonTerminalIterator = andNonTerminalPointerArray.begin(); 
	  nonTerminalIterator != andNonTerminalPointerArray.end(); 
	  nonTerminalIterator++)
       {
	 ROSE_ASSERT((*nonTerminalIterator)!=NULL);
	 string nameInProductionRule = (*nonTerminalIterator)->name;
	 printf ("     AND NonTerminal: nameInProductionRule = %s \n",nameInProductionRule.c_str());
       }
   }

