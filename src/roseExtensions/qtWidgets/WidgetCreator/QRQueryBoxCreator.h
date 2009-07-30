
#ifndef QRQUERYBOXCREATOR_H
#define QRQUERYBOXCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class QRQueryBoxCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        QRQueryBoxCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/Rose.gif" ), "QRQueryBox" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
