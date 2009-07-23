
#ifndef ASTGRAPHWIDGETCREATOR_H
#define ASTGRAPHWIDGETCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class AstGraphWidgetCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        AstGraphWidgetCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/Graph.gif" ), "AST Tree View" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
