
#include "SrcBinViewCreator.h"
#include "SrcBinView.h"

QWidget *SrcBinViewCreator::createWidget( QWidget *parent )
{
    QWidget *w( new SrcBinView( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
