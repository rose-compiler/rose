
#include "CallGraphWidgetCreator.h"
#include "CallGraphWidget.h"

QWidget *CallGraphWidgetCreator::createWidget( QWidget *parent )
{
    QWidget *w( new CallGraphWidget( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
