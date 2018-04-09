#include "sage3basic.h"
#include "UntypedJovialConverter.h"
#include "Jovial_to_ROSE_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0

using namespace Untyped;
using std::cout;
using std::endl;

void
UntypedJovialConverter::convertLabel (SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                      SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
   std::string label_name = ut_stmt->get_label_string();
   if (!label_name.empty())
      {
         char* next;
      // SageInterface::setFortranNumericLabel(sg_stmt, std::stoul(label_name));
      // The modifications in setFortranNumericLabel should be moved to SageInterface

      // TODO - This will need to handle multiple labels per Jovial statement
      // TODO - Change Fortran specific name

         setFortranNumericLabel(sg_stmt, strtoul(label_name.c_str(),&next,10), label_type, label_scope);
         ROSE_ASSERT(next != label_name.c_str());
      }
}
