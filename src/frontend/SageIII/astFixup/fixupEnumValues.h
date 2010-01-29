#ifndef FIXUP_ENUM_VALUES_H
#define FIXUP_ENUM_VALUES_H

#include "Cxx_Grammar.h"

/*! \brief This traversal uses the Memory Pool traversal to fixup the enum declarations referenced in the enum values (SgEnumVal).

    This traversal fixes any NULL declarations to SgEnumDeclaration held in the SgEnumVal.  These can be NULL
    when the enum value is referenced in a class before it is seen in that class (allowed in C++, but a difficult
    detail to handle sometimes).  In this case I am unable to find the required EDG information so that I can build 
    the enum declaration when I see the enum value so I build the enum value with a NULL pointer to the declaration 
    and we fix it up here. See test2007_83.C for an example of this.

    \implementation It is not clear that we can always resolve what enum declaration might be referenced.  We need
    to check this out.  Using directives could make this very difficult.  At the moment we search in the class, clearly
    that is not good enough generally.
 */
class FixupEnumValues : public ROSE_VisitTraversal
   {
     public:
     virtual ~FixupEnumValues() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void fixupEnumValues();

// endif for FIXUP_ENUM_VALUES_H
#endif
