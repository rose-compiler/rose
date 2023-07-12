#include <sage3basic.h>

SgName
SgLabelRefExp::get_name() const
   {
     ROSE_ASSERT(p_symbol != NULL);
     return p_symbol->get_name();
   }

int
SgLabelRefExp::get_numeric_label_value() const
   {
     ROSE_ASSERT(p_symbol != NULL);
     return p_symbol->get_numeric_label_value();
   }

bool
SgLabelRefExp::isFunctionCallArgumentForAlternativeReturnType() const
   {
  // We might want to make sure that this is for a Fortran AST.
  // This is relevant more for type checking than for the unparing since a SgLabel expression will always be output using "*" syntax.
  // However initial results from this test is showing that the function type arguments list is at least sometime empty when it should not be.

     bool returnValue = false;
     return returnValue;
   }
