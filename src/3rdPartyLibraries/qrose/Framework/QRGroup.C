/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <QRGroup.h>
#include <QRMain.h>

using namespace std;
using namespace qrs;

namespace qrs {
/********************************************** QRGroup ******/
QRGroup::QRGroup (unsigned groupId) {
    m_groupId = groupId;
}


void QRGroup::attach(QRGroupWidget *widget) {
    connect(widget->getLink(m_groupId), SIGNAL(triggerBroadcast(QRGroupWidget *, QRMessage *)),
	    this, SLOT(doBroadcast(QRGroupWidget*, QRMessage* )));
    connect(this, SIGNAL(broadcastEvent(QRGroupWidget*, QRMessage* )),
	    widget->getLink(m_groupId), SLOT(receiveMsg(QRGroupWidget*, QRMessage* )));
}

void QRGroup::detach(QRGroupWidget *widget) {
    disconnect(widget->getLink(m_groupId),0, 0, 0);
    disconnect(widget->getLink(m_groupId));
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
    if (m_widget->isRcvEnabled()) {
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
    m_emitterEnabled = true;
    m_rcvEnabled = true;
}

QRGroupWidget::~QRGroupWidget() {
   detachFromAllGroups();
}

bool QRGroupWidget::belongsToGroup(int groupId) {
   map<int, QRLink *>::iterator iter = m_links.find(groupId);
   return iter != m_links.end();
}

void QRGroupWidget::attachToGroup(int groupId) {
   map<int, QRLink *>::iterator iter = m_links.find(groupId);
   if (iter == m_links.end()) {
      m_links[groupId] = new QRLink(this);
      QROSE::getGroup(groupId)->attach(this);
   }
}

void QRGroupWidget::attachOnlyToGroup(int groupId) {
   detachFromAllGroups();
   attachToGroup(groupId);
}

void QRGroupWidget::detachFromGroup(int groupId) {
   map<int, QRLink *>::iterator iter = m_links.find(groupId);
   if (iter != m_links.end()) {
      QROSE::getGroup(groupId)->detach(this);
      delete iter->second;
      m_links.erase(iter);
   }
}

void QRGroupWidget::detachFromAllGroups() {
   map<int, QRLink *>::iterator iter;

   while ((iter = m_links.begin()) != m_links.end()) {
      detachFromGroup(iter->first);
   }

}

QRLink* QRGroupWidget::getLink(int groupId) {
    map<int, QRLink *>::iterator iter;
    if (groupId == -1) {
       iter = m_links.begin();
    } else {
       iter = m_links.find(groupId);
    }
    if (iter != m_links.end()) return iter->second; else return NULL;
}

void QRGroupWidget::broadcastMsg(QRMessage *msg, int groupId) {
    if (m_emitterEnabled) {
        QRLink *link = getLink(groupId);
        if (link) link->broadcastMsg(msg);
    }
}

bool QRGroupWidget::isEmitterEnabled() {
    return m_emitterEnabled;
}

void QRGroupWidget::setEmitterEnabled(bool isOn) {
    m_emitterEnabled = isOn;
}

bool QRGroupWidget::isRcvEnabled() {
    return m_rcvEnabled;
}

void QRGroupWidget::setRcvEnabled(bool isOn) {
    m_rcvEnabled = isOn;
}

}




