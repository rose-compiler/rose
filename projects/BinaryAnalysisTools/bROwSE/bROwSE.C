#include <rose.h>
#include <bROwSE/bROwSE.h>

using namespace rose::BinaryAnalysis;

namespace bROwSE {

Sawyer::Message::Facility mlog;

Attribute::Id ATTR_NBytes(-1);
Attribute::Id ATTR_NInsns(-1);
Attribute::Id ATTR_NIntervals(-1);
Attribute::Id ATTR_NDiscontiguousBlocks(-1);
Attribute::Id ATTR_CfgGraphVizFile(-1);
Attribute::Id ATTR_CfgImage(-1);
Attribute::Id ATTR_CfgVertexCoords(-1);
Attribute::Id ATTR_CallGraph(-1);
Attribute::Id ATTR_NCallers(-1);
Attribute::Id ATTR_NCallees(-1);
Attribute::Id ATTR_NReturns(-1);
Attribute::Id ATTR_MayReturn(-1);
Attribute::Id ATTR_StackDelta(-1);
Attribute::Id ATTR_Ast(-1);
Attribute::Id ATTR_Heat(-1);
Attribute::Id ATTR_DataFlow(-1);
Attribute::Id ATTR_CallConvDef(-1);

void
Context::init() {
    if (ATTR_NBytes == Attribute::INVALID_ID) {
        ATTR_NBytes               = Attribute::declare("Size in bytes");
        ATTR_NInsns               = Attribute::declare("Number of instructions");
        ATTR_NIntervals           = Attribute::declare("Number of contiguous intervals");
        ATTR_NDiscontiguousBlocks = Attribute::declare("Number of discontigous basic blocks");
        ATTR_CfgGraphVizFile      = Attribute::declare("CFG GraphViz file name");
        ATTR_CfgImage             = Attribute::declare("CFG JPEG file name");
        ATTR_CfgVertexCoords      = Attribute::declare("CFG vertex coordinates");
        ATTR_CallGraph            = Attribute::declare("Function call graph");
        ATTR_NCallers             = Attribute::declare("Number of call sites from whence function is called");
        ATTR_NCallees             = Attribute::declare("Number of calls made by this function");
        ATTR_NReturns             = Attribute::declare("Number of returning basic blocks");
        ATTR_MayReturn            = Attribute::declare("May return to caller?");
        ATTR_StackDelta           = Attribute::declare("Stack pointer delta");
        ATTR_Ast                  = Attribute::declare("Abstract syntax tree");
        ATTR_Heat                 = Attribute::declare("Heat value for color gradients [0..1]");
        ATTR_DataFlow             = Attribute::declare("Data flow information");
        ATTR_CallConvDef          = Attribute::declare("Calling convention definition");
    }
}

// Convert colors from one representation to another
Wt::WColor toWt(const rose::Color::RGB &rgb) {
    using namespace rose::Color;
    int r = (int)round(clip(rgb.r())*255);
    int g = (int)round(clip(rgb.g())*255);
    int b = (int)round(clip(rgb.b())*255);
    int a = (int)round(clip(rgb.a())*255);
    return Wt::WColor(r, g, b, a);
}


} // namespace
