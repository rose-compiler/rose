
#include "AsmViewCreator.h"
#include "AsmView.h"

QWidget *AsmViewCreator::createWidget( QWidget *parent )
{
    QWidget *w( new AsmView( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg( counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
