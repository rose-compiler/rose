
#include "rose.h"

#include <cmath>

#include "MetricsInfo.h"

using namespace std;

double MetricsInfo::eval( SgNode *node, const QString& metricName ) const
{
    MetricAttribute *metricAttr = NULL;
    if( node != NULL )
        metricAttr = dynamic_cast<MetricAttribute *>( node->getAttribute( metricName.toStdString() ) );

    if( !metricAttr )
    {
        return 0.0;
    }

    double value( metricAttr->getValue() );
    double normalizeValue;
    double newValue( 0.0f );

    if( normalizeByRange )
    {
        double min( minNormalize );
        double max( maxNormalize );
        normalizeValue = max - min;
        if( min >= 0.0f && max <= 1.0f )
            newValue = value;
        else if( fabs( max - min ) > 1e-9f )
        {
            newValue = value / normalizeValue;
        }
    }
    else
    {
        MetricAttribute *normalizeAttr( dynamic_cast<MetricAttribute *>( node->getAttribute( normalizeMetricName.toStdString() ) ) );

        normalizeValue = normalizeAttr ? normalizeAttr->getValue() : 1.0;
        newValue = normalizeValue == 0 ? 0.0 : value/normalizeValue;
    }

    if( newValue < 0.0 ) newValue = 0.0;
    if( newValue > 1.0 ) newValue = 1.0;

    return newValue;
}
