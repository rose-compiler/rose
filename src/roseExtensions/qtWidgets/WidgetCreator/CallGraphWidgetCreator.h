
#ifndef CALLGRAPHWIDGETCREATOR_H
#define CALLGRAPHWIDGETCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for CallGraphWidget - see SubWindowFactory
 */
class CallGraphWidgetCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        CallGraphWidgetCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/CallGraph.gif" ), "Call Graph View" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
