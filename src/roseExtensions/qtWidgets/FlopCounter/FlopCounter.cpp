
#include "rose.h"

#include "FlopCounter.h"


Flops FlopCounter::evaluateSynthesizedAttribute( SgNode *astNode, SynthesizedAttributesList attributeList )
{
      SgBinaryOp *op = isSgBinaryOp( astNode );

      Flops sum;
      bool derived( true );

      if( op != NULL && op->get_type()->isFloatType() )
      {
         derived = false;
         sum.add( op->variantT(), 1 );
      }

      SynthesizedAttributesList::const_iterator it( attributeList.begin() );

      for( int i = 0; it != attributeList.end(); ++it, ++i )
      {
         sum += *it;
      }

      // annotate AST
      astNode->addNewAttribute( "FLOP count: Total", new MetricAttribute(sum() ) );

      for( int i = 0; i < V_SgNumVariants; ++i )
      {
          if( i == V_SgNode )
              continue;

          size_t flops = sum( static_cast< VariantT >( i ) );

          if( flops == 0 )
              continue;

          std::string name = getSgVariant( i );
          astNode->addNewAttribute( "FLOP count: " + name, new MetricAttribute(flops) );
      }

      return sum;
}
