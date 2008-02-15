/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                          *
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

#ifndef QROSEGROUP_H
#define QROSEGROUP_H

#include <QRGroup.h>
#include <set>

#include <config.h>
#include <rose.h>
    
#include <QRCoords.h>

class QRoseGroupWidget: public QRGroupWidget {
            
public:
    typedef enum { msg_node, msg_cbox } typeMsgId;   
            
    QRoseGroupWidget();    
    
    bool isCacheAccumEnabled();
    void setCacheAccumEnabled(bool is_on);        
    void insertNodeInCache(SgNode *node, bool emit_event = true);
    void clearCache(bool emit_event = true);   
    bool isNodeInCache(SgNode *node);
    void setCacheAutoInsert(bool isOn);
    bool isCacheAutoInsert();
    unsigned cacheSize();
    std::set<SgNode *>* getCache() { return &m_cache; };
    
    // messages
    class SgNodeMsg: public QRMessage {
	public:
	   SgNodeMsg(SgNode *node);
	   SgNode *getNode();
	
	protected:
	   SgNode *m_node;
    };
        
    class CBoxMsg: public QRMessage {
	public:
	   CBoxMsg(QRCoordBox *box);
           QRCoordBox* getBox();
	   
	protected:
	   QRCoordBox *m_box;
    };
    
        
protected:    
    void broadcastNode(SgNode *node);
    virtual void receiveMsg(QRGroupWidget *emitter, QRMessage *msg);
    // event handler
    virtual void eventCacheModified();        
    
protected:
    std::set<SgNode *> m_cache;
    SgNode *m_rootNode;
    bool m_accumEnabled;
    bool m_autoInsert;
};



#endif

