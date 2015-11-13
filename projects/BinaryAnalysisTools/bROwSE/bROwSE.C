#include <rose.h>
#include <bROwSE/bROwSE.h>

using namespace rose::BinaryAnalysis;

namespace bROwSE {

Sawyer::Message::Facility mlog;

Sawyer::Attribute::Id ATTR_NBytes(-1);
Sawyer::Attribute::Id ATTR_NInsns(-1);
Sawyer::Attribute::Id ATTR_NIntervals(-1);
Sawyer::Attribute::Id ATTR_NDiscontiguousBlocks(-1);
Sawyer::Attribute::Id ATTR_CfgGraphVizFile(-1);
Sawyer::Attribute::Id ATTR_CfgImage(-1);
Sawyer::Attribute::Id ATTR_CfgVertexCoords(-1);
Sawyer::Attribute::Id ATTR_CallGraph(-1);
Sawyer::Attribute::Id ATTR_NCallers(-1);
Sawyer::Attribute::Id ATTR_NCallees(-1);
Sawyer::Attribute::Id ATTR_NReturns(-1);
Sawyer::Attribute::Id ATTR_MayReturn(-1);
Sawyer::Attribute::Id ATTR_StackDelta(-1);
Sawyer::Attribute::Id ATTR_Ast(-1);
Sawyer::Attribute::Id ATTR_Heat(-1);
Sawyer::Attribute::Id ATTR_DataFlow(-1);

void
Context::init() {
    if (ATTR_NBytes == Sawyer::Attribute::INVALID_ID) {
        ATTR_NBytes               = Sawyer::Attribute::declare("Size in bytes");
        ATTR_NInsns               = Sawyer::Attribute::declare("Number of instructions");
        ATTR_NIntervals           = Sawyer::Attribute::declare("Number of contiguous intervals");
        ATTR_NDiscontiguousBlocks = Sawyer::Attribute::declare("Number of discontigous basic blocks");
        ATTR_CfgGraphVizFile      = Sawyer::Attribute::declare("CFG GraphViz file name");
        ATTR_CfgImage             = Sawyer::Attribute::declare("CFG JPEG file name");
        ATTR_CfgVertexCoords      = Sawyer::Attribute::declare("CFG vertex coordinates");
        ATTR_CallGraph            = Sawyer::Attribute::declare("Function call graph");
        ATTR_NCallers             = Sawyer::Attribute::declare("Number of call sites from whence function is called");
        ATTR_NCallees             = Sawyer::Attribute::declare("Number of calls made by this function");
        ATTR_NReturns             = Sawyer::Attribute::declare("Number of returning basic blocks");
        ATTR_MayReturn            = Sawyer::Attribute::declare("May return to caller?");
        ATTR_StackDelta           = Sawyer::Attribute::declare("Stack pointer delta");
        ATTR_Ast                  = Sawyer::Attribute::declare("Abstract syntax tree");
        ATTR_Heat                 = Sawyer::Attribute::declare("Heat value for color gradients [0..1]");
        ATTR_DataFlow             = Sawyer::Attribute::declare("Data flow information");
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
