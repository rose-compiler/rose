// ROSETTA has no way to exclude certain IR node definitions based on the user's choice of languages.  Therefore, we need to
// provide (stub) definitions for those nodes' virtual methods in order to get things to compile when those nodes aren't
// actually being used anywhere.
#include "sage3basic.h"

SgAsmType *SgAsmIntegerValueExpression::get_type() { abort(); }
SgAsmType *SgAsmByteValueExpression::get_type() { abort(); }
SgAsmType *SgAsmWordValueExpression::get_type() { abort(); }
SgAsmType *SgAsmDoubleWordValueExpression::get_type() { abort(); }
SgAsmType *SgAsmQuadWordValueExpression::get_type() { abort(); }
