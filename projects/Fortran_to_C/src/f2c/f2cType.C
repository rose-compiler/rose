#include "f2c.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;


SgType* Fortran_to_C::translateType(SgType* oldType)
{
  switch(oldType->variantT())
  {
    case V_SgTypeString:
      {
        SgExpression* stringLength = deepCopy(isSgTypeString(oldType)->get_lengthExpression());
        SgArrayType* newType =  buildArrayType(buildCharType(),NULL);
        newType->set_rank(1);
        newType->set_dim_info(buildExprListExp(stringLength));
        return newType;
      }
    case V_SgTypeFloat:
      {
        SgIntVal* typeKind = isSgIntVal(oldType->get_type_kind());
        if(typeKind == NULL)
        {
          // TODO: we just return original type. Might need to fix this in the future
          return oldType;
        }
        switch(typeKind->get_value())
        {
          case 4:
            return buildFloatType();
          case 8:
            return buildDoubleType();
          default:
            ROSE_ASSERT(false);
        }
      }
    case V_SgTypeInt:
      {
        SgIntVal* typeKind = isSgIntVal(oldType->get_type_kind());
        if(typeKind == NULL)
        {
          // TODO: we just return original type. Might need to fix this in the future
          return oldType;
        }
        switch(typeKind->get_value())
        {
          case 2:
            return buildShortType();
          case 4:
            return buildIntType();
          case 8:
            return buildLongType();
          default:
            ROSE_ASSERT(false);
        }
      }
    default:
      return oldType;
  }
}
