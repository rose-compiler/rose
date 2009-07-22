
#include "AstBrowserWidgetCreator.h"
#include "AstBrowserWidget.h"

QWidget *AstBrowserWidgetCreator::createWidget( QWidget *parent )
{
    QWidget *w( new AstBrowserWidget( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(widgets.size() ) );
    w->setWindowIcon( getIcon() );


    return w;
}
