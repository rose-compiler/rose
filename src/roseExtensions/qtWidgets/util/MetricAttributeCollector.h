
#ifndef METRIC_ATTRIBUTE_COLLECTOR_H
#define METRIC_ATTRIBTUE_COLLECTOR_H

#include <QObject>
#include <QString>

#include "sage3basic.h"
#include "NewAstProcessing.h"

class SgNode;

//#include "MetricsInfo.h"

class MetricAttributeCollector
    : public QObject
{
 Q_OBJECT

 public:
    typedef std::pair<double, double> range;
    typedef std::map<QString, range> range_container;
    typedef range_container::iterator iterator;
    typedef range_container::const_iterator const_iterator;

    int evaluateInheritedAttribute( SgNode *astNode, int );

    iterator begin()
    {
       return attr.begin();
    }
    iterator end()
    {
       return attr.end();
    }

    const int size() const
    {
        return attr.size();
    }
 private:
    range_container attr;
};

#endif
