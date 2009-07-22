
#include "WidgetCreatorInterface.h"

#include <QEvent>

#include <QDebug>

void WidgetCreatorInterface::registerCloseEvent( QWidget *w )
{
    widgets.push_back( w );
    w->installEventFilter( this );
}

bool WidgetCreatorInterface::eventFilter( QObject *object, QEvent *event )
{
    if( event->type() == QEvent::Close )
    {
        widgets.removeAll( dynamic_cast<QWidget *>( object ) );

        return true;
    }

    return false;
}
