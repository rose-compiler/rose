
#ifndef BASTVIEWCREATOR_H
#define BASTVIEWCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for BAstView - see SubWindowFactory
 */
class BeautifiedAstCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        BeautifiedAstCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/project.gif" ), "Beautified AST View" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
