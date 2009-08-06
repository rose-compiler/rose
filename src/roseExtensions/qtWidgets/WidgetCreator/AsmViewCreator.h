
#ifndef ASMVIEWCREATOR_H
#define ASMVIEWCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for AsmView - see SubWindowFactory
 */
class AsmViewCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        AsmViewCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/binaryfiles.gif" ),
                                      "Assembler View" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
