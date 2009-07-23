
#ifndef ASTBROWSERWIDGETCREATOR_H
#define ASTBROWSERWIDGETCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class AstBrowserWidgetCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        AstBrowserWidgetCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/project.gif" ), "AST Browser" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
