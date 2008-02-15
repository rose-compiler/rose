#ifndef __TERMINALLIST_H__
#define __TERMINALLIST_H__

#include <list>

// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

class Terminal;
class NonTerminal;
class GrammarString;

class TerminalList : public std::list<Terminal *>
{
  // Later we can make this an associative list (STL map) 
  // so that the operator[] (char*) is more efficient!
 public:
  ~TerminalList ();
  TerminalList ();
  TerminalList ( const Terminal & X );
  TerminalList ( const std::list<Terminal *> & X );
  TerminalList ( const TerminalList & X );
  TerminalList & operator= ( const TerminalList & X );

  bool containedInList ( const std::string& name ) const;
  Terminal & operator[] (unsigned int ) const;
  Terminal & operator[] (const std::string&) const;

  // Setup functions and data within the implementation of the grammar represented by this nonTerminal
  void setFunctionPrototype        ( const GrammarString & memberFunction );
  void setFunctionPrototype        ( const std::string& markerString, const std::string& filename, 
				     bool pureVirtual = FALSE );

  void setSubTreeFunctionPrototype ( const GrammarString & memberFunction );
  void setSubTreeFunctionPrototype ( const std::string& markerString, const std::string& filename, 
				     bool pureVirtual = FALSE );

  void setDataPrototype            ( const GrammarString & memberData); 
  void setDataPrototype            ( const std::string& markerString, const std::string& filename );

  void setFunctionSource           ( const std::string& markerString, const std::string& filename,
				     bool pureVirtual = FALSE );
  void setSubTreeFunctionSource    ( const std::string& markerString, const std::string& filename,
				     bool pureVirtual = FALSE );

  void editSubstitute ( const std::string& oldString, const std::string& newString );

  void display( const std::string& label = "" ) const;

  void consistencyCheck() const;
};

class NonTerminalList : public TerminalList
{
  // Later we can make this an associative list (STL map) 
  // so that the operator[] (char*) is more efficient!
 public:
  ~NonTerminalList ();
  NonTerminalList ();
  NonTerminalList ( const Terminal & X );
  NonTerminalList ( const NonTerminal & X );
  NonTerminalList ( const TerminalList & X );
  NonTerminalList & operator= ( const NonTerminalList & X );

  NonTerminal & operator[] (unsigned int ) const;
  NonTerminal & operator[] (const std::string&) const;

  void display( const std::string& label = "" ) const;

  void consistencyCheck() const;
};

#ifdef __GNUG__
//#include "terminalList.C"
//#include "nonTerminalList.C"
#endif

#endif
