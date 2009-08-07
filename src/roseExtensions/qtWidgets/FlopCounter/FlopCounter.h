
#ifndef FLOP_COUNTER_H
#define FLOP_COUNTER_H



#include "AstProcessing.h"
#include "NewAstProcessing.h"
#include "Flops.h"


/**
 * \brief Annotates the AST with information of FLOP operations
 *
 * Counts the occurances of FLOP operations like, assignments, additions, multiplies etc.
 * and annotates them as MetricAttributes
 *
 * Usage example for annotation:
 * \code
   FlopCounterProcessor flops;
   flops.traverse( file );
   \endcode
 */
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
