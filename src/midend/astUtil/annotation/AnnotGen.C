// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
#ifdef _MSC_VER
// seems to cause problems under Linux
#include "rose.h"
#endif

#include "AnnotGen.h"

bool POET::GenAnnot_AstParse::operator()
(AstInterface& fa, const AstNodePtr& n, AstNodePtr& result)
{
   AstNodePtr r1;
  if (fa.IsFunctionDefinition(n, 0,0,0,&r1) && r1 != 0) {
     fa.InsertAnnot(n, "/*@; BEGIN(_)@*/", true);
     fa.InsertAnnot(r1, "/*@=>_:Exp@*/",true);
     fa.InsertAnnot(n, "/*@END(_:Function)@*/", false); 
  }
  else if (fa.IsFortranLoop(n,0,0,0,0,&r1)) {
     fa.InsertAnnot(n, "/*@; BEGIN(_)@*/", true);
     fa.InsertAnnot(r1, "/*@=>_:Loop@*/",true);
     fa.InsertAnnot(n, "/*@END(_:Nest)@*/", false); 
  }  
  result = n;
  return false;
}

void POET::GenAnnot_AstParse::apply(AstInterface& fa, const AstNodePtr& n)
   {
     TransformAstTraverse(fa, n, *this, AstInterface::PostVisit);
     fa.InsertAnnot(n, "/*@<input _>@*/", true);
     fa.InsertAnnot(n, "/*@</input>@*/", false);
   }
