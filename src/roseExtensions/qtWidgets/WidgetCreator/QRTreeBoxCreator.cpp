
#include "rose.h"

#include "QRTreeBoxCreator.h"
#include "QRoseComponentProxy.h"
#include "QRTreeBox.h"

#include <QWidget>

QWidget *QRTreeBoxCreator::createWidget( QWidget *parent )
{
    QWidget *w( new qrs::QRoseComponentProxy( new qrs::QRTreeBox, parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
