
#ifndef ROSEFILESELECTORCREATOR_H
#define ROSEFILESELECTORCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class RoseFileSelectorCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        RoseFileSelectorCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/Rose.gif" ), "Select File" )
        {}

        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
