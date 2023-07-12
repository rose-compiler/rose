#include <sage3basic.h>

SgType* SgAdaTaskRefExp::get_type() const
{
  return SgAdaTaskType::createType(get_decl(), NULL);
}
