
#include "rose.h"

#include <QMdiArea>
#include <QMenu>
#include <QVBoxLayout>

#include <QMdiSubWindow>

#include <QDebug>

#include "SubWindowFactory.h"
#include "WidgetCreatorInterface.h"

SubWindowFactory::~SubWindowFactory()
{
    qDeleteAll( interfaces );
}

void SubWindowFactory::registerSubWindow( WidgetCreatorInterface *winInterface )
{
    assert( winInterface );

    interfaces.push_back( winInterface );

    connect( winInterface->createAction(), SIGNAL( triggered() ),
             this                        , SLOT  ( addSubWindow() ) );
}

void SubWindowFactory::fillMenu( QMenu *menu )
{
    foreach( WidgetCreatorInterface *widgets, interfaces )
    {
        menu->addAction( widgets->createAction() );
    }
}

void SubWindowFactory::addSubWindow()
{
    assert( sender() );

    foreach( WidgetCreatorInterface *widgets, interfaces )
    {
        if( widgets->createAction() == sender() )
        {
            QWidget *subWidget = new QWidget();
            QWidget *widget( widgets->createWidget( subWidget ) );

            QVBoxLayout * layout = new QVBoxLayout( subWidget );
            layout->addWidget( widget );

            QMdiSubWindow *win = QMdiArea::addSubWindow( subWidget );
            win->setWindowTitle( widget->windowTitle() );
            win->setWindowIcon( widget->windowIcon() );

            subWidget->show();
            widget->show();

            widgets->registerCloseEvent( win );

            /*QMenu *sysMenu( win->systemMenu() );
            sysMenu->addSeparator();
            QMenu *linkMenu( sysMenu->addMenu( "Link To ..." ) );*/

            break;
        }
    }
}
