
#include "BeautifiedAstCreator.h"
#include "BAstView.h"

QWidget *BeautifiedAstCreator::createWidget( QWidget *parent )
{
    QWidget *w( new BAstView( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(widgets.size() ) );
    w->setWindowIcon( getIcon() );

    return w;
}
