
#include "rose.h"
#include "QRoseComponentProxy.h"

#include "qrose.h"

#include <QHBoxLayout>
#include <QWidget>

namespace qrs {

QRoseComponentProxy::QRoseComponentProxy( QRoseComponent *comp, ::QWidget *parent )
: QWidget( parent ),
  layout( new QHBoxLayout() ),
  comp_( comp )
{
    assert( comp_ != NULL );

    QWidget *w( dynamic_cast<QWidget *>( comp ) );
    assert( w );

    setLayout( layout );
    layout->addWidget( w );

    QRLink *link( comp_->getLink(-1) );

    connect( link, SIGNAL( triggerBroadcast( QRGroupWidget *, QRMessage *) ),
             this, SLOT  ( getBroadcast( QRGroupWidget *, QRMessage *) ) );
}

void QRoseComponentProxy::setNode( SgNode *node )
{

}

void QRoseComponentProxy::gotoNode( SgNode *node )
{
    comp_->insertNodeInCache( node, true );
}

void QRoseComponentProxy::getBroadcast( QRGroupWidget *, QRMessage *msg )
{
    switch (msg->id()) {
        case QRoseComponent::msg_node:
            {
                SgNode *node = ((QRoseComponent::SgNodeMsg *) msg)->getNode();
                emit nodeActivated( node );
            }
            break;
        case QRoseComponent::msg_nodes:
            {
                //set<SgNode *> *nodes = ((SgNodesMsg *) msg)->getNodes();
            }
            break;
    }
}

}
