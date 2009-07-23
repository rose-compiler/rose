
#include "RoseFileSelectorCreator.h"
#include "RoseFileComboBox.h"

QWidget *RoseFileSelectorCreator::createWidget( QWidget *parent )
{
    QWidget *w( new RoseFileComboBox( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
