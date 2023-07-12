#include <sage3basic.h>

SgType* SgAdaRenamingRefExp::get_type() const
{
  SgAdaRenamingDecl* decl = get_decl();
  ROSE_ASSERT(decl);

  SgExpression* ren = decl->get_renamed();
  ROSE_ASSERT(ren);

  return ren->get_type();
}
