#include <sage3basic.h>

SgType* SgAdaProtectedRefExp::get_type() const
{
  return SgAdaProtectedType::createType(get_decl(), NULL);
}
