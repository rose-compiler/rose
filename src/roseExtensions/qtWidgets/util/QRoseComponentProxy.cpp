
#include "rose.h"
#include "QRoseComponentProxy.h"

namespace qrs {

QRoseComponentProxy::QRoseComponentProxy( QRoseComponent *comp )
: comp_( comp )
{
    assert( comp_ != NULL );

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
                emit clicked( node );
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
