#include <bROwSE/bROwSE.h>

namespace bROwSE {

P2::Attribute::Id ATTR_NBYTES(-1);
P2::Attribute::Id ATTR_NINSNS(-1);
P2::Attribute::Id ATTR_CFG_DOTFILE(-1);
P2::Attribute::Id ATTR_CFG_IMAGE(-1);
P2::Attribute::Id ATTR_CFG_COORDS(-1);
P2::Attribute::Id ATTR_CG(-1);
P2::Attribute::Id ATTR_NCALLERS(-1);
P2::Attribute::Id ATTR_NRETURNS(-1);
P2::Attribute::Id ATTR_MAYRETURN(-1);
P2::Attribute::Id ATTR_STACKDELTA(-1);

void
Context::init() {
    if (ATTR_NBYTES == P2::Attribute::INVALID_ID) {
        ATTR_NBYTES         = P2::Attribute::registerName("Size in bytes");
        ATTR_NINSNS         = P2::Attribute::registerName("Number of instructions");
        ATTR_CFG_DOTFILE    = P2::Attribute::registerName("CFG GraphViz file name");
        ATTR_CFG_IMAGE      = P2::Attribute::registerName("CFG JPEG file name");
        ATTR_CFG_COORDS     = P2::Attribute::registerName("CFG vertex coordinates");
        ATTR_CG             = P2::Attribute::registerName("Function call graph");
        ATTR_NCALLERS       = P2::Attribute::registerName("Number of call sites from whence function is called");
        ATTR_NRETURNS       = P2::Attribute::registerName("Number of returning basic blocks");
        ATTR_MAYRETURN      = P2::Attribute::registerName("May return to caller?");
        ATTR_STACKDELTA     = P2::Attribute::registerName("Stack pointer delta");
    }
}

} // namespace
