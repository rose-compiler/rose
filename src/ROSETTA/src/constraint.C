// ################################################################
// #                           Header Files                       #
// ################################################################

#include "constraint.h"
#include "grammarString.h"

// ################################################################
// #                 Constraint Member Functions                  #
// ################################################################

Constraint::~Constraint ()
   {
   }

Constraint::Constraint ()
   {
     printf ("ERROR: wrong constructor called (default constructor inappropriate)! \n");
     ROSE_ABORT();
   }

Constraint::Constraint ( const std::string& inputName, const std::string& inputConstraintString )
   {
     name             = inputName;
     constraintString = inputConstraintString;
   }

Constraint::Constraint ( const Constraint & X )
   {
     printf ("ERROR: wrong constructor called (copy constructor not implemented)! \n");
     ROSE_ABORT();
   }

Constraint &
Constraint::operator= ( const Constraint & X )
   {
     // BP : 10/18/2001, bugfix
     printf ("ERROR: Constraint::operator= not implemented)! \n");
     ROSE_ABORT();
     return *this;
   }

void
Constraint::addConstraint ( const std::string& inputConstraintString )
  // JJW 10-15-2007 don't have the check for the constraint string already being set
   {
#if 0
     if (constraintString == NULL)
       {
#endif
	 constraintString = inputConstraintString;
#if 0
       }
       else
        {
          printf ("ERROR in Constraint::addConstraint: constraintString already set to: '%s' \n", constraintString);
          ROSE_ABORT();
        }

     ROSE_ASSERT(constraintString != NULL);
#endif
   }

const std::string&
Constraint::getName () const
   {
     return name;
   }

const std::string&
Constraint::getConstraintString () const
   {
     return constraintString;
   }

void
Constraint::setName ( const std::string& inputName )
   {
     name = inputName;
   }

void
Constraint::consistencyCheck() const
   {
   }

