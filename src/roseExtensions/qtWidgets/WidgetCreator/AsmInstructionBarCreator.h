
#ifndef ASMINSTRUCTIONBARCREATOR_H
#define ASMINSTRUCTIONBARCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class AsmInstructionBarCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        AsmInstructionBarCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/binaryfiles.gif" ), "Assembler Instruction Bar" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
