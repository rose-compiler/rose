
#include "rose.h"

#include "QRSourceBoxCreator.h"
#include "QRoseComponentProxy.h"
#include "QRSourceBox.h"

QWidget *QRSourceBoxCreator::createWidget( QWidget *parent )
{
    QWidget *w( new qrs::QRoseComponentProxy( new qrs::QRSourceBox, parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
