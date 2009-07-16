/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include "QRoseComponent.h"
#include <QRCoords.h>
#include <rose.h>

using namespace std;

namespace qrs {

QRoseComponent::SgNodeMsg::SgNodeMsg(SgNode *node): QRMessage(msg_node) {
    m_node = node;
}

SgNode *QRoseComponent::SgNodeMsg::getNode() {
    return m_node;
}

QRoseComponent::SgNodesMsg::SgNodesMsg(set<SgNode *> *nodes): QRMessage(msg_nodes) {
   for (set<SgNode *>::iterator iter = nodes->begin();
        iter != nodes->end(); iter++)
   {
       m_nodes.insert(*iter);
   }
}

set<SgNode *> *QRoseComponent::SgNodesMsg::getNodes() {
   return &m_nodes;
}

QRoseComponent::QRoseComponent() {
   initComponent();

   attachToGroup(0);
}


void QRoseComponent::initComponent() {
   m_isCollectMode = false;
   m_isTargetMode = false;
   m_cache.clear();
}

bool QRoseComponent::isCollectMode() {
    return m_isCollectMode;
}

void QRoseComponent::setCollectMode(bool isOn) {
	m_isCollectMode = isOn;
}

void QRoseComponent::removeNodeFromCache(SgNode *node, bool emit_event) {
   set<SgNode *>::iterator iter = m_cache.find(node);
   if (iter != m_cache.end()) {
      m_cache.erase(iter);
      if (emit_event) {
         set<SgNode *> removed_nodes; removed_nodes.insert(*iter);
         eventCacheRemovedNodes(removed_nodes);
      }
   }
}

void QRoseComponent::insertNodeInCache(SgNode *node, bool emitEvent) {
    if (!isCollectMode()) {
       clearCache(emitEvent);
    }
    pair<set<SgNode *>::iterator,bool> ret;
    ret = m_cache.insert(node);
    if (emitEvent && ret.second) {
       set<SgNode *> new_nodes; new_nodes.insert(node);
       eventCacheNewNodes(new_nodes);
    }
}

void QRoseComponent::insertNodesInCache(set<SgNode *> *nodes, bool emitEvent) {
    if (!isCollectMode()) {
       clearCache(emitEvent);
    }

    set<SgNode *> new_nodes;

    for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
       pair<set<SgNode *>::iterator,bool> ret;
       ret = m_cache.insert(*iter);
       if (ret.second) new_nodes.insert(*iter);
    }


    if (emitEvent && !new_nodes.empty()) {
       eventCacheNewNodes(new_nodes);
    }
}

void QRoseComponent::clearCache(bool emitEvent) {
    m_cache.clear();
    if (emitEvent) {
       eventCacheClear();
    }
}

bool QRoseComponent::isNodeInCache(SgNode *node) {
   return (m_cache.find(node) != m_cache.end());
}


void QRoseComponent::broadcastNode(SgNode *node, int groupId) {
    SgNodeMsg msg(node);
    broadcastMsg(&msg, groupId);
}

void QRoseComponent::broadcastCache(int groupId) {
	if (m_cache.empty()) return;
    SgNodesMsg msg(&m_cache);
    broadcastMsg(&msg, groupId);
}


void QRoseComponent::eventCacheClear() {

}

void QRoseComponent::eventCacheNewNodes(set<SgNode *> &nodes) {

}

void QRoseComponent::eventCacheRemovedNodes(set<SgNode *> &nodes) {


}
bool QRoseComponent::isTargetMode() {
    return m_isTargetMode;
}

void QRoseComponent::setTargetMode(bool isOn) {
    m_isTargetMode = isOn;
}

unsigned QRoseComponent::cacheSize() {
    return m_cache.size();
}

void QRoseComponent::receiveMsg(QRGroupWidget *emitter, QRMessage *msg) {
    if (isTargetMode()) {
       switch (msg->id()) {
          case msg_node:
             {
   	        SgNode *node = ((SgNodeMsg *) msg)->getNode();
	        insertNodeInCache(node);
             }
             break;
          case msg_nodes:
             {
                set<SgNode *> *nodes = ((SgNodesMsg *) msg)->getNodes();
                insertNodesInCache(nodes);
             }
             break;
       }
   }
}
}

