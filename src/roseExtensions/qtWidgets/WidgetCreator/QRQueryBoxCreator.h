
#ifndef QRQUERYBOXCREATOR_H
#define QRQUERYBOXCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

/**
 * \brief Creator for QRQueryBox - see SubWindowFactory
 */
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
