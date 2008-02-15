#ifndef __TERMINAL_LIST_C__
#define __TERMINAL_LIST_C__
// ################################################################
// #                           Header Files                       #
// ################################################################

#include "grammar.h"
#include "terminalList.h"
#include "terminal.h"
#include "grammarString.h"

using namespace std;

// ################################################################
// #                 NonTerminal Member Functions                 #
// ################################################################

TerminalList::~TerminalList ()
   {
   }

TerminalList::TerminalList ()
   : list<Terminal *>()
   {
   }

TerminalList::TerminalList ( const Terminal & X )
   : list<Terminal *>()
   {
     const Terminal *const &Y = &X;
     push_back( (Terminal *const &) Y );
   }

TerminalList::TerminalList ( const list<Terminal *> & X )
   {
     abort();
   }

TerminalList::TerminalList ( const TerminalList & X )
   {
     abort();
   }

TerminalList & TerminalList::operator= ( const TerminalList & X )
   {
     list<Terminal *>::operator= ( X );
     return *this;
   }

bool
TerminalList::containedInList ( const string& name ) const
{
  
  bool returnValue = FALSE;
  
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      
      string nameFromList = (*it)->getName();
      
      if( name == nameFromList )
	{
	  returnValue = TRUE;
	  break;
	}

    }
  
  return returnValue;
}

Terminal &TerminalList::operator[] ( unsigned int i ) const
{
  Terminal* returnTerminal = NULL;
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

  returnTerminal = (Terminal *) *it;
  
  ROSE_ASSERT(returnTerminal!=NULL);
  
  return *returnTerminal;
}

Terminal & TerminalList::operator[] ( const string& name ) const
{
  Terminal *returnVal = NULL;
  
  ROSE_ASSERT(this != NULL);
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      if( name == (*it)->getName() )
	{
	  returnVal = *it;
	  break;
	}	
    }

  if( returnVal == NULL )
    {
      cout << "failed to find matching terminal with name" << name << " in list " << endl;
      ROSE_ASSERT(returnVal!=NULL);
    }

  return *returnVal;
}


void
TerminalList::display( const string& label ) const
{
  
#if 1
  printf ("In TerminalList::display(label=%s): list<Terminal> size = %zu \n",label.c_str(),size());
#endif
  ROSE_ASSERT(this != NULL);
  
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      ROSE_ASSERT( *it != NULL );
      string terminalName = (*it)->getName();
      printf ("(*it)->getName() = %s \n", terminalName.c_str() );
      //	 delete [] terminalName;   // prevent memory leaks
    }  
}

void TerminalList::setFunctionPrototype ( const GrammarString & memberFunction )
{
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setFunctionPrototype (memberFunction);
    }
}

void TerminalList::setFunctionPrototype  
   ( const string& markerString, const string& filename, bool pureVirtual )
{
  // Error checking (there must be at least one leaf) length zero 
  // implies that the buildTree function has not been called
  ROSE_ASSERT( size() > 0 );
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setFunctionPrototype (markerString,filename,pureVirtual);
    }
}

void TerminalList::setSubTreeFunctionPrototype ( const GrammarString & memberFunction )
{
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setSubTreeFunctionPrototype (memberFunction);
    }
}

void TerminalList::setSubTreeFunctionPrototype 
   ( const string& markerString, const string& filename, bool pureVirtual )
{
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setSubTreeFunctionPrototype (markerString,filename,pureVirtual);
    }
}

void TerminalList::setDataPrototype ( const GrammarString & memberData)
{
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setDataPrototype (memberData);
    }
}

void TerminalList::setFunctionSource           
   ( const string& markerString, const string& filename, bool pureVirtual )
{
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setFunctionSource (markerString,filename,pureVirtual);
    }
}

void TerminalList::setSubTreeFunctionSource 
( const string& markerString, const string& filename, bool pureVirtual )
{
  list<Terminal *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->setSubTreeFunctionSource (markerString,filename,pureVirtual);
    }
}

void TerminalList::editSubstitute ( const string& oldString, const string& newString )
{
  list<Terminal *>::const_iterator it;  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->editSubstitute (oldString,newString);
    }
}

void
TerminalList::consistencyCheck() const
{
  // Put error checking here!
  // check the terminal list for valid objects
  list<Terminal *>::const_iterator it;  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->consistencyCheck();
    }
}

#endif // __TERMINAL_LIST_C__




