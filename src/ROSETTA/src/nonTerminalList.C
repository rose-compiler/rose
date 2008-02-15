#ifndef __NONTERMINAL_LIST_C__
#define __NONTERMINAL_LIST_C__
// ################################################################
// #                           Header Files                       #
// ################################################################

#include "grammar.h"
#include "terminalList.h"
#include "terminal.h"
#include "nonterminal.h"
#include "grammarString.h"

using namespace std;

// ################################################################
// #                 NonTerminal Member Functions                 #
// ################################################################

NonTerminalList::~NonTerminalList ()
   {
   }

NonTerminalList::NonTerminalList ()
   : TerminalList()
   {
   }

NonTerminalList::NonTerminalList ( const Terminal & X )
   : TerminalList()
   {
     const Terminal *const &Y = &X;
     push_back((Terminal *const &)Y);
   }

NonTerminalList::NonTerminalList ( const NonTerminal & X )
   : TerminalList()
   {
     const Terminal *const &Y = (Terminal *) &X;
     push_back( (Terminal *const &) Y );
   }

NonTerminalList::NonTerminalList ( const TerminalList & X )
   : TerminalList(X)
   {
   }

NonTerminalList & NonTerminalList::operator= ( const NonTerminalList & X )
   {
     list<Terminal *>::operator= ( (const TerminalList &) X );
     return *this;
   }

NonTerminal &
NonTerminalList::operator[] ( unsigned int i ) const
{
  NonTerminal* returnNonTerminal = NULL;
  unsigned int elemIndex = 0;
  
  ROSE_ASSERT(this != NULL);

  if ( i >= size() )
    {
      cout << "ERROR:" <<  i << " is outside list bounds" << endl;
      ROSE_ASSERT( i < size() );  
    }

  list<Terminal *>::const_iterator it;  
  for(it = begin(); it!=end(); it++)
    {
      if(elemIndex == i)
	break;
      else
	elemIndex++;
    }

  returnNonTerminal = (NonTerminal *)*it;
  
  ROSE_ASSERT(returnNonTerminal!=NULL);

  return *returnNonTerminal;
}

NonTerminal &
NonTerminalList::operator[] ( const string& name ) const
{
  NonTerminal* returnNonTerminal = NULL;
  ROSE_ASSERT(this != NULL);
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      string terminalName = (*it)->getName();
      if( name == terminalName )
	{
	  returnNonTerminal = (NonTerminal *)*it;
	  break;
	}
    }

  return *returnNonTerminal;
}

void
NonTerminalList::display( const string& label ) const
{
  ROSE_ASSERT(this != NULL);
#if 1
     printf ("In NonTerminalList::display(label=%s): list<Terminal *> size = %zu \n",label.c_str(),size());
#endif
  
  
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      ROSE_ASSERT( *it != NULL );
      string terminalName = (*it)->getName();
      printf ("(*it)->getName() = %s \n", terminalName.c_str() );
    }  
}

void
NonTerminalList::consistencyCheck() const
   {
  // Put error checking here!
     TerminalList::consistencyCheck();
   }

#endif //__NONTERMINAL_LIST_C__

