
#ifndef METRICSKIVIATCREATOR_H
#define METRICSKIVIATCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for MetricsKiviat - see SubWindowFactory
 */
class MetricsKiviatCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        MetricsKiviatCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/Kiviat.gif" ), "Metrics Kiviat" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
