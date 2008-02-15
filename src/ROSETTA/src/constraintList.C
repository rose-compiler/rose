#ifndef __CONSTRAINTLIST_C__
#define __CONSTRAINTLIST_C__
// ################################################################
// #                           Header Files                       #
// ################################################################

#include "constraintList.h"
#include "constraint.h"
#include "grammarString.h"
#include "ROSETTA_macros.h"

using namespace std;

// ################################################################
// #                 NonTerminal Member Functions                 #
// ################################################################

ConstraintListType::~ConstraintListType ()
   {
   }

ConstraintListType::ConstraintListType ()
   {
   }

ConstraintListType::ConstraintListType ( const Constraint &X )
   {
     //     addElement( X );
     const Constraint *const &Y = &X;
     push_back( (Constraint *const &) Y );
   }

#if 0
ConstraintListType::ConstraintListType ( const ConstraintListType & X )
   {
     printf ("Copy constructor not implemented! \n");
     ROSE_ABORT();
   }
#endif

ConstraintListType & 
ConstraintListType::operator= ( const ConstraintListType & X )
   {
     printf ("ConstraintListType::operator= not tested! \n");
     ROSE_ABORT();

  // list<Constraint*>::operator= ( ((ConstraintListType&)X)::list<Constraint*> );
     return *this;
   }

bool
ConstraintListType::containedInList ( char* name ) const
   {
     bool returnValue = FALSE;

     list<Constraint *>::const_iterator it;
     
     for( it = begin(); it != end(); it++ )
       {
	 assert( *it != NULL );
	 if( name ==  (*it)->getName() )
	   {
	     returnValue = TRUE;
	     break;
	   }
       }

     return returnValue;
   }

Constraint &
ConstraintListType::operator[] ( char* name ) const
{
  Constraint *returnVal = NULL;

  ROSE_ASSERT(this != NULL);
  list<Constraint *>::const_iterator it;
  
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
      cout << "failed to find matching constraint with name" << name << " in list " << endl;
      ROSE_ASSERT(returnVal!=NULL);
    }

  return *returnVal;
}

void
ConstraintListType::addConstraint( const Constraint* X )
   {
     // addElement(*X);
     const Constraint *const &Y = X;
     push_back( (Constraint *const &) Y );
   }


void
ConstraintListType::consistencyCheck() const
{
  // Error checking
  
  list<Constraint *>::const_iterator it;
  
  for( it = begin(); it != end(); it++ )
    {
      assert( *it != NULL );
      (*it)->consistencyCheck();
    }
}

#endif //__CONSTRAINTLIST_C__
