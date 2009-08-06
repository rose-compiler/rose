
#ifndef ROSECODEEDITCREATOR_H
#define ROSECODEEDITCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for RoseCodeEdit - see SubWindowFactory
 */
class RoseCodeEditCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        RoseCodeEditCreator()
            : WidgetCreatorInterface( QIcon(":/util/NodeIcons/sourcefile.gif" ), "Source Code" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
