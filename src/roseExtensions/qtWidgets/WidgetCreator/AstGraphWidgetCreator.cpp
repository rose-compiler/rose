
#include "AstGraphWidgetCreator.h"
#include "AstGraphWidget.h"

QWidget *AstGraphWidgetCreator::createWidget( QWidget *parent )
{
    QWidget *w( new AstGraphWidget( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
