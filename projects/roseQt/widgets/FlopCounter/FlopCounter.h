
#ifndef FLOP_COUNTER_H
#define FLOP_COUNTER_H



#include "AstProcessing/AstProcessing.h"
#include "Flops.h"

#include "AstProcessing.h"

class FlopCounter
{
    protected:
        typedef StackFrameVector<Flops> SynthesizedAttributesList;

        Flops evaluateSynthesizedAttribute( SgNode *astNode, SynthesizedAttributesList attributeList );

        Flops defaultSynthesizedAttribute()
        {
            return Flops();
        }
};

typedef AstProcessing::BottomUp<FlopCounter, Flops> FlopCounterProcessor;

#endif
