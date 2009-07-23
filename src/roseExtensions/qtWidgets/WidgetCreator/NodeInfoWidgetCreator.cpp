
#include "NodeInfoWidgetCreator.h"
#include "NodeInfoWidget.h"

QWidget *NodeInfoWidgetCreator::createWidget( QWidget *parent )
{
    QWidget *w( new NodeInfoWidget( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
