
#include "WidgetCreatorInterface.h"

#include <QEvent>

#include <QDebug>

QWidget *WidgetCreatorInterface::addWidget( QWidget *parent )
{
    counter++;
    return createWidget( parent );
}
