
#ifndef ASMVIEWCREATOR_H
#define ASMVIEWCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class AsmViewCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        AsmViewCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/binaryfiles.gif" ),
                                      "Assembler View" )
        {}

        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
