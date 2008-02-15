#ifndef __NONTERMINAL_H__
#define __NONTERMINAL_H__

// #include <list.h>
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

#include "terminal.h"

class NonTerminal : public Terminal
{
  // Inherit the Terminal interface within the Non-Terminal object

 public:
  std::list<Terminal *>    andTokenPointerArray;
  std::list<NonTerminal *> andNonTerminalPointerArray;
  std::list<Terminal *>    orTokenPointerArray;
  std::list<NonTerminal *> orNonTerminalPointerArray;
  // char* name;

  bool repeatedNonTerminal;
  bool defaultNonTerminal;

  // Used to separate identifiers
  static const std::string separator;

  ~NonTerminal ();
  NonTerminal ();
  NonTerminal ( const Terminal & token );
  NonTerminal ( const NonTerminal & X );
  NonTerminal ( const NonTerminal & X, Grammar & grammar );

  void displayName ( int indent = 0 ) const;
  void initialize();

  int getLastOrTokenIndex        () const;
  int getLastOrNonTerminalIndex  () const;
  int getLastAndTokenIndex       () const;
  int getLastAndNonTerminalIndex () const;

  void And ( const Terminal    & X );
  void And ( const NonTerminal & X );
  void Or  ( const Terminal    & X );
  void Or  ( const NonTerminal & X );


  void addElement(std::list<Terminal *> &, const Terminal &X );
  void addElement(std::list<NonTerminal *> &, const NonTerminal &X );

  // Clear the and/or lists of terminals/nonterminals (but don't deallocate the objects in the lists)
  void clearLists();

  // builds child terminals from parent terminals
  NonTerminal & copy ( char* lexeme, char* tagString );

  void show ( int indent = 0 ) const;

                 // The inplace modification of a nonterminal always returns a nonterminal by definition
  NonTerminal & operator |= ( const Terminal & Y );
  NonTerminal & operator |= ( const NonTerminal & Y );

                 // Friend functions representing OR and AND of Terminal and NonTerminal elements of the grammar
  friend NonTerminal & operator| ( const Terminal    & X, const Terminal    & Y );
  friend NonTerminal & operator| ( const NonTerminal & X, const Terminal    & Y );
  friend NonTerminal & operator| ( const Terminal    & X, const NonTerminal & Y );
  friend NonTerminal & operator| ( const NonTerminal & X, const NonTerminal & Y );

  friend NonTerminal & operator& ( const Terminal    & X, const Terminal    & Y );
  friend NonTerminal & operator& ( const NonTerminal & X, const Terminal    & Y );
  friend NonTerminal & operator& ( const Terminal    & X, const NonTerminal & Y );
  friend NonTerminal & operator& ( const NonTerminal & X, const NonTerminal & Y );

                 // Member function representing repeated specification of the nonterminal grammar element 
  NonTerminal operator++ ( int );
  NonTerminal operator[] ( NonTerminal & X );
  NonTerminal operator[] ( Terminal    & X );

  // specialized operator='s for Terminal/NonTerminal types
  // NonTerminal & operator= ( const Terminal & X );
  NonTerminal & operator= ( const NonTerminal & X );

  // Used for assignment of expressions to terminals (operator= is 
  // usually used but we want to have specialized functions so that
  // the operator= can be reserved for more traditional use)
  void setExpression ( const Terminal & X );
  void setExpression ( const NonTerminal & X );

  void consistencyCheck() const;
  void display ( const char* label = "" ) const;

  // We use a single isChild() function in the base 
  // class instead of a special isChildNonTerminal()
  // bool isChildNonTerminal() const;
};

// DQ (4/23/2006): This is required for the g++ 4.1.0 compiler where the
// friend declaration in the class NonTerminal is not enough.
NonTerminal & operator| ( const Terminal    & X, const Terminal    & Y );
NonTerminal & operator| ( const NonTerminal & X, const Terminal    & Y );
NonTerminal & operator| ( const Terminal    & X, const NonTerminal & Y );
NonTerminal & operator| ( const NonTerminal & X, const NonTerminal & Y );

#endif
