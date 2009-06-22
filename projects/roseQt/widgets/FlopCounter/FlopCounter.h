
#ifndef FLOP_COUNTER_H
#define FLOP_COUNTER_H



#include "AstProcessing.h"
#include "NewAstProcessing.h"
#include "Flops.h"



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
