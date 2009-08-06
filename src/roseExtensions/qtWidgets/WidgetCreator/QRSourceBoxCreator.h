

#ifndef QRSOURCEBOXCREATOR_H
#define QRSOURCEBOXCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for QRSourceBox - see SubWindowFactory
 */
class QRSourceBoxCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        QRSourceBoxCreator()
            : WidgetCreatorInterface( QIcon( ":/util/NodeIcons/sourcefile.gif" ), "QRSourceBox" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
