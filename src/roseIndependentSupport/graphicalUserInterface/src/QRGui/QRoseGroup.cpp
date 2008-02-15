/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "QRoseGroup.h"
#include <QRGui.h>
#include <QRCoords.h>


QRoseGroupWidget::SgNodeMsg::SgNodeMsg(SgNode *node): QRMessage(msg_node) {
    m_node = node;
}
SgNode *QRoseGroupWidget::SgNodeMsg::getNode() {
    return m_node;
}

QRoseGroupWidget::QRoseGroupWidget() {
    m_rootNode = QRGUI::get_root();
    m_accumEnabled = false;
    m_autoInsert = false;    
}


bool QRoseGroupWidget::isCacheAccumEnabled() {
    return m_accumEnabled;
}

void QRoseGroupWidget::setCacheAccumEnabled(bool isOn) {
    m_accumEnabled = isOn;
}

void QRoseGroupWidget::insertNodeInCache(SgNode *node, bool emitEvent) {
    if (!m_accumEnabled) {
       clearCache(emitEvent);    
    }
    m_cache.insert(node);    
    if (emitEvent) {
       eventCacheModified();
    }
}

void QRoseGroupWidget::clearCache(bool emitEvent) {
    m_cache.clear();  
    if (emitEvent) {
       eventCacheModified();
    }
}

bool QRoseGroupWidget::isNodeInCache(SgNode *node) {
   return (m_cache.find(node) != m_cache.end());
}


void QRoseGroupWidget::broadcastNode(SgNode *node) {
    SgNodeMsg msg(node);
    broadcastMsg(&msg);
}

QRoseGroupWidget::CBoxMsg::CBoxMsg(QRCoordBox *box): QRMessage(msg_cbox) {
    m_box = box;    
} 
  

QRCoordBox* QRoseGroupWidget::CBoxMsg::getBox() {
    return m_box;
}

void QRoseGroupWidget::eventCacheModified() {

}

bool QRoseGroupWidget::isCacheAutoInsert() {
    return m_autoInsert;    
}

void QRoseGroupWidget::setCacheAutoInsert(bool isOn) {
    m_autoInsert = isOn;
}

unsigned QRoseGroupWidget::cacheSize() {
    return m_cache.size();
}

void QRoseGroupWidget::receiveMsg(QRGroupWidget *emitter, QRMessage *msg) {
    if (m_autoInsert) {
       if (msg->id() == msg_node) {
   	   SgNode *node = ((SgNodeMsg *) msg)->getNode();
	   insertNodeInCache(node);
       }
   }
}
	
