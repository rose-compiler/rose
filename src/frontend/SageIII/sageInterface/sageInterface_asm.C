// tps (01/14/2010) : Switching from rose.h to sage3. added integerops.h
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include <stdint.h>
#include <iostream>
#include "integerOps.h"

uint64_t
SageInterface::getAsmConstant(SgAsmValueExpression *expr)
{
    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr))
        return ival->get_absoluteValue();
    std::cerr << "Bad variant " << expr->class_name() << " in " <<__func__ << std::endl;
    abort();
    // DQ (11/30/2009): avoid MSVC warning of path without return stmt.
    return 0;
}

int64_t
SageInterface::getAsmSignedConstant(SgAsmValueExpression *expr) 
{
    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr))
        return ival->get_signedValue();
    std::cerr <<"Bad variant " <<expr->class_name() <<" in " <<__func__ <<std::endl;
    abort();
    // DQ (11/30/2009): avoid MSVC warning of path without return stmt.
    return 0;
}

#endif
