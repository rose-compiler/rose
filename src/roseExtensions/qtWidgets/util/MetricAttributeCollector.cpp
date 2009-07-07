#include "rose.h"


#include <QDebug>

#include "MetricAttributeCollector.h"


using namespace std;

int MetricAttributeCollector::evaluateInheritedAttribute( SgNode *astNode, int )
{
   if(astNode->get_attributeMechanism() == NULL)
       return 0;

   AstAttributeMechanism::AttributeIdentifiers aidents = astNode->get_attributeMechanism()->getAttributeIdentifiers();
   for( AstAttributeMechanism::AttributeIdentifiers::iterator it = aidents.begin(); it != aidents.end(); ++it )
   {
      MetricAttribute *metrAttr = dynamic_cast<MetricAttribute *>( astNode->getAttribute( *it ) );
      if( metrAttr )
      {
         const QString name( (*it).c_str() );
         const double value( metrAttr->getValue() );

         iterator it = attr.find( name );
         if( it == end() )
         {
            attr.insert( range_container::value_type( name, range( value, value ) ) );
         }
         else
         {
            if( value < it->second.first  ) it->second.first  = value; // update min
            if( value > it->second.second ) it->second.second = value; // update max
         }
      }
   }
   return 0;
}
