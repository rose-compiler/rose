
#include "RoseCodeEditCreator.h"
#include "RoseCodeEdit.h"

QWidget *RoseCodeEditCreator::createWidget( QWidget *parent )
{
    QWidget *w( new RoseCodeEdit( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(widgets.size() ) );
    w->setWindowIcon( getIcon() );

    return w;
}
