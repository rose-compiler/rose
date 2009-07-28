#include "rose.h"

#include <QApplication>
#include <QMainWindow>

#include <QDebug>

#include "qrose.h"

#include "QRoseComponentProxy.h"
#include "AstBrowserWidget.h"
#include "NodeInfoWidget.h"

using namespace qrs;

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    SgProject *project( frontend( argc, argv ) );

    QWidget    *win   ( new QWidget() );
    QBoxLayout *layout( new QHBoxLayout() );


    QRSourceBox         *sourceBox( new QRSourceBox( project ) );
    QRoseComponentProxy *proxy    ( new QRoseComponentProxy( sourceBox, win ) );
    NodeInfoWidget      *nodeInfo ( new NodeInfoWidget );

    QObject::connect( proxy   , SIGNAL( clicked( SgNode * ) ),
                      nodeInfo, SLOT  ( setNode( SgNode * ) ) );

    win->setLayout( layout );
    layout->addWidget( proxy );
    layout->addWidget( nodeInfo );

    win->show();

    return app.exec();
}
