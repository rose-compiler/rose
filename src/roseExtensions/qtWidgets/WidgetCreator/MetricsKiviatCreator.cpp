
#include "MetricsKiviatCreator.h"
#include "MetricsKiviat.h"

QWidget *MetricsKiviatCreator::createWidget( QWidget *parent )
{
    QWidget *w( new MetricsKiviat( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(widgets.size() ) );
    w->setWindowIcon( getIcon() );

    return w;
}
