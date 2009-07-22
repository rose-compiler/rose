
#include "NodeInfoWidgetCreator.h"
#include "NodeInfoWidget.h"

QWidget *NodeInfoWidgetCreator::createWidget( QWidget *parent )
{
    QWidget *w( new NodeInfoWidget( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(widgets.size() ) );
    w->setWindowIcon( getIcon() );

    return w;
}
