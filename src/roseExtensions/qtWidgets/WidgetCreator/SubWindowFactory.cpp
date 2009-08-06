
#include "rose.h"

#include <QMdiArea>
#include <QMenu>
#include <QVBoxLayout>

#include <QMdiSubWindow>

#include <QEvent>

#include <QDebug>

#include "SubWindowFactory.h"
#include "WidgetCreatorInterface.h"

typedef QPair<QWidget *, QWidget *> WidgetPair;
Q_DECLARE_METATYPE( WidgetPair );

SubWindowFactory::~SubWindowFactory()
{
    qDeleteAll( interfaces );
}

void SubWindowFactory::registerSubWindow( WidgetCreatorInterface *winInterface )
{
    Q_ASSERT( winInterface );

    interfaces.push_back( winInterface );

    connect( winInterface->createAction(), SIGNAL( triggered() ),
             this                        , SLOT  ( addSubWindowAction() ) );
}

QList<QAction *> SubWindowFactory::getActions() const
{
    QList<QAction *> res;

    foreach( WidgetCreatorInterface *widgets, interfaces )
    {
        res.push_back( widgets->createAction() );
    }

    return res;
}

void SubWindowFactory::addSubWindowAction()
{
    assert( sender() );

    foreach( WidgetCreatorInterface *widgets, interfaces )
    {
        if( widgets->createAction() == sender() )
        {
            QWidget *parent = new QWidget();
            QWidget *widget( widgets->addWidget( parent ) );

            QVBoxLayout * layout = new QVBoxLayout( parent );
            layout->addWidget( widget );

            QMdiSubWindow *win = QMdiArea::addSubWindow( parent );
            win->setWindowTitle( widget->windowTitle() );
            win->setWindowIcon( widget->windowIcon() );

            parent->show();
            widget->show();

            win->installEventFilter( this );
            openWidgets.insert( win, widget );

            rebuildSystemMenus();

            break;
        }
    }
}

void SubWindowFactory::linkAction()
{
    QAction *action( dynamic_cast<QAction *>( sender() ) );
    if( action == NULL ) return;

    QPair<QWidget *, QWidget *> pair(
            qVariantValue<QPair<QWidget *, QWidget *> >( action->data() ) );

    if( action->isChecked() )
    {
        connect( pair.first, SIGNAL( nodeActivated( SgNode * ) ),
                 pair.second, SLOT( gotoNode( SgNode * ) ) );
        connect( pair.first, SIGNAL( nodeActivatedAlt( SgNode * ) ),
                 pair.second, SLOT( setNode( SgNode * ) ) );

        linked[pair] = true;
    }
    else
    {
        disconnect( pair.first, SIGNAL( nodeActivated( SgNode * ) ),
                    pair.second, SLOT( gotoNode( SgNode * ) ) );
        disconnect( pair.first, SIGNAL( nodeActivatedAlt( SgNode * ) ),
                    pair.second, SLOT( setNode( SgNode * ) ) );

        linked[pair] = false;
    }
}

bool SubWindowFactory::eventFilter( QObject *object, QEvent *event )
{
    if( event->type() == QEvent::Close )
    {
        QMap<QMdiSubWindow *, QWidget *>::iterator it( 
                openWidgets.find( dynamic_cast<QMdiSubWindow *>( object ) ) );

        if( it != openWidgets.end() )
        {
            openWidgets.erase( it );

            foreach( QWidget *w, openWidgets )
            {
                QPair<QWidget *, QWidget *> linkedPair( it.value(), w );

                QMap<QPair<QWidget *, QWidget *>, bool>::iterator
                    jt( linked.find( linkedPair ) );

                if( jt != linked.end() )
                {
                    linked.erase( jt );
                }
            }
            rebuildSystemMenus();
        }
    }

    return false;
}

void SubWindowFactory::rebuildSystemMenus()
{

    QMap<QMdiSubWindow *, QWidget *>::iterator pair(
            openWidgets.begin() );

    while( pair != openWidgets.end() )
    {
        QMenu *sysMenu( pair.key()->systemMenu() );
        QMenu *linkMenu( NULL );
        
        linkMenu = sysMenu->findChild<QMenu *>( "linkMenu" );

        if( linkMenu == NULL )
        {
            if( openWidgets.size() == 1 ) return;

            sysMenu->addSeparator();
            linkMenu = sysMenu->addMenu( "Link To ..." );
            linkMenu->setObjectName( "linkMenu" );
        }
        else
        {
            if( openWidgets.size() == 1 )
            {
                delete linkMenu;
                return;
            }
            linkMenu->clear();
        }

        QMap<QMdiSubWindow *, QWidget *>::iterator otherPair(
                openWidgets.begin() );
        while( otherPair != openWidgets.end() )
        {
            if( otherPair.key() != pair.key() )
            {
                QAction *action( linkMenu->addAction( otherPair.key()->windowTitle() ) );

                QVariant variant;
                QPair<QWidget *, QWidget *> linkPair( pair.value(), otherPair.value() );
                qVariantSetValue( variant, linkPair );
                action->setData( variant );
                action->setCheckable( true );

                if( linked[linkPair] )
                {
                    action->setChecked( true );
                }

                connect( action, SIGNAL( triggered() ),
                         this  , SLOT  ( linkAction() ) );
            }
            ++otherPair;
        }

        ++pair;
    }
}
