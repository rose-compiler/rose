
#ifndef SRCBINVIEWCREATOR_H
#define SRCBINVIEWCREATOR_H

#include "WidgetCreatorInterface.h"

class QWidget;
class QEvent;

class SrcBinViewCreator
    : public WidgetCreatorInterface
{
    Q_OBJECT

    public:
        SrcBinViewCreator()
            : WidgetCreatorInterface( QIcon( ":/util/WidgetIcons/Rose.gif" ), "Source-Assembly Comparison" )
        {}

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL );
};

#endif
