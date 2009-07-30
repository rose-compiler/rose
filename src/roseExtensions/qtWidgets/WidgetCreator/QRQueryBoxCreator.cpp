
#include "rose.h"

#include "QRQueryBoxCreator.h"
#include "QRoseComponentProxy.h"
#include "QRQueryBox.h"

#include <QWidget>

QWidget *QRQueryBoxCreator::createWidget( QWidget *parent )
{
    QWidget *w( new qrs::QRoseComponentProxy( new qrs::QRQueryBox, parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
