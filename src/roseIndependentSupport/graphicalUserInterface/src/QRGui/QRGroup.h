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

#ifndef QRGROUP_H
#define QRGROUP_H

#include <qobject.h>
   
class QRGroup;
class QRLink;
class QRGroupWidget;

class QRMessage {
public:    
    QRMessage(int id);
    int id();
        
protected:    
  int m_id;  
};

class QRGroup: public QObject {
    Q_OBJECT    
        
public:    
    QRGroup(unsigned groupId);   
    void attach(QRGroupWidget *widget);
    
public slots:
    void doBroadcast(QRGroupWidget *emitter, QRMessage *msg);

signals:
    void broadcastEvent(QRGroupWidget *emmiter, QRMessage *msg);

protected:
    unsigned m_groupId;
};

class QRLink: public QObject {
    Q_OBJECT
        
public: 
    QRLink(QRGroupWidget *widget);
    void broadcastMsg(QRMessage *msg);
    
public slots:
    void receiveMsg(QRGroupWidget *emmiter, QRMessage *msg);
    
signals:
    void triggerBroadcast(QRGroupWidget *emmiter, QRMessage *msg);
    
protected:
    QRGroupWidget *m_widget;
};


class QRGroupWidget {
    friend class QRLink;
public:
    QRGroupWidget();
    QRLink* getLink();
            
    bool isEmitterEnabled();
    void setEmitterEnabled(bool isOn);
    
    bool isTargetEnabled();
    void setTargetEnabled(bool isOn);
    
protected:    
    virtual void broadcastMsg(QRMessage *msg); 
    virtual void receiveMsg(QRGroupWidget *emitter, QRMessage *msg) = 0;
    
protected:
    QRLink *m_link;    
    bool m_emitterEnabled;    
    bool m_targetEnabled;
};


#endif
