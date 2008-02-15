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

#include "QRGroup.h"

/********************************************** QRGroup ******/
QRGroup::QRGroup (unsigned groupId) {
    m_groupId = groupId;
}


void QRGroup::attach(QRGroupWidget *widget) {
    connect(widget->getLink(), SIGNAL(triggerBroadcast(QRGroupWidget *, QRMessage *)),
	    this, SLOT(doBroadcast(QRGroupWidget*, QRMessage* )));
    connect(this, SIGNAL(broadcastEvent(QRGroupWidget*, QRMessage* )), 
	    widget->getLink(), SLOT(receiveMsg(QRGroupWidget*, QRMessage* )));
}

void QRGroup::doBroadcast(QRGroupWidget *emitter, QRMessage *msg) {    
    emit broadcastEvent(emitter, msg); 
}
 
/********************************************* QRLink **********/
QRLink::QRLink(QRGroupWidget *widget) {
    m_widget = widget;
}

void QRLink::broadcastMsg(QRMessage *msg) {
   emit triggerBroadcast(m_widget, msg);
}

void QRLink::receiveMsg(QRGroupWidget *emmiter, QRMessage *msg) {
    if (m_widget->isTargetEnabled()) {
       m_widget->receiveMsg(emmiter, msg);
   }
}
 
/********************************************* QRMessage */
QRMessage::QRMessage(int id) {
    m_id = id;
}

int QRMessage::id() {
    return m_id;
}

/********************************************* QRGroupWidget */
QRGroupWidget::QRGroupWidget() {
    m_link = new QRLink(this);
    m_emitterEnabled = true;
    m_targetEnabled = true;
}
 
QRLink* QRGroupWidget::getLink() {
    return m_link;
}

void QRGroupWidget::broadcastMsg(QRMessage *msg) {
    if (m_emitterEnabled) {
        m_link->broadcastMsg(msg);
    }
}

bool QRGroupWidget::isEmitterEnabled() {
    return m_emitterEnabled;
}

void QRGroupWidget::setEmitterEnabled(bool isOn) {
    m_emitterEnabled = isOn;
}

bool QRGroupWidget::isTargetEnabled() {
    return m_targetEnabled;
}

void QRGroupWidget::setTargetEnabled(bool isOn) {
    m_targetEnabled = isOn;
}


#include "QRGroup.moc"












