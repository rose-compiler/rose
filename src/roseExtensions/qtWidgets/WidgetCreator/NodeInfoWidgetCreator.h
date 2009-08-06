
#ifndef NODEINFOWIDGETCREATOR_H
#define NODEINFOWIDGETCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for NodeInfoWidget - see SubWindowFactory
 */
class NodeInfoWidgetCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        NodeInfoWidgetCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/PropTreeWidget.gif" ), "SgNode Properties" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
