#include <bROwSE/bROwSE.h>

namespace bROwSE {

Sawyer::Message::Facility mlog;

P2::Attribute::Id ATTR_NBytes(-1);
P2::Attribute::Id ATTR_NInsns(-1);
P2::Attribute::Id ATTR_NIntervals(-1);
P2::Attribute::Id ATTR_NDiscontiguousBlocks(-1);
P2::Attribute::Id ATTR_CfgGraphVizFile(-1);
P2::Attribute::Id ATTR_CfgImage(-1);
P2::Attribute::Id ATTR_CfgVertexCoords(-1);
P2::Attribute::Id ATTR_CallGraph(-1);
P2::Attribute::Id ATTR_NCallers(-1);
P2::Attribute::Id ATTR_NReturns(-1);
P2::Attribute::Id ATTR_MayReturn(-1);
P2::Attribute::Id ATTR_StackDelta(-1);
P2::Attribute::Id ATTR_Ast(-1);
P2::Attribute::Id ATTR_Heat(-1);
P2::Attribute::Id ATTR_DataFlow(-1);

void
Context::init() {
    if (ATTR_NBytes == P2::Attribute::INVALID_ID) {
        ATTR_NBytes               = P2::Attribute::registerName("Size in bytes");
        ATTR_NInsns               = P2::Attribute::registerName("Number of instructions");
        ATTR_NIntervals           = P2::Attribute::registerName("Number of contiguous intervals");
        ATTR_NDiscontiguousBlocks = P2::Attribute::registerName("Number of discontigous basic blocks");
        ATTR_CfgGraphVizFile      = P2::Attribute::registerName("CFG GraphViz file name");
        ATTR_CfgImage             = P2::Attribute::registerName("CFG JPEG file name");
        ATTR_CfgVertexCoords      = P2::Attribute::registerName("CFG vertex coordinates");
        ATTR_CallGraph            = P2::Attribute::registerName("Function call graph");
        ATTR_NCallers             = P2::Attribute::registerName("Number of call sites from whence function is called");
        ATTR_NReturns             = P2::Attribute::registerName("Number of returning basic blocks");
        ATTR_MayReturn            = P2::Attribute::registerName("May return to caller?");
        ATTR_StackDelta           = P2::Attribute::registerName("Stack pointer delta");
        ATTR_Ast                  = P2::Attribute::registerName("Abstract syntax tree");
        ATTR_Heat                 = P2::Attribute::registerName("Heat value for color gradients [0..1]");
        ATTR_DataFlow             = P2::Attribute::registerName("Data flow information");
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
