
#include "AsmInstructionBarCreator.h"
#include "AsmInstructionsBar.h"

QWidget *AsmInstructionBarCreator::createWidget( QWidget *parent )
{
    QWidget *w( new AsmInstructionsBar( parent ) );

    w->setWindowTitle( getName() + QString( " [%1]" ).arg(counter ) );
    w->setWindowIcon( getIcon() );

    return w;
}
