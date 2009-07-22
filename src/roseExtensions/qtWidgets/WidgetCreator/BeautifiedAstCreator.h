
#ifndef BASTVIEWCREATOR_H
#define BASTVIEWCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class BeautifiedAstCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        BeautifiedAstCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/project.gif" ), "Beautified AST View" )
        {}

        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
