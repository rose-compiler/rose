/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRGROUP_H
#define QRGROUP_H

#include <QObject>
#include <map>

namespace qrs {

class QRLink;
class QRGroupWidget;

/**
* \ingroup qrose_framework
*/

/// Group message
class QRMessage {
public:
     /// Message initialization
    QRMessage(int id);
    /// Returns the message id (type)
    int id();

protected:
  int m_id;
};

/// Set of components that communicate with each other through broadcasting mechanism
class QRGroup: public QObject {
    Q_OBJECT

    friend class QROSE_P;
    friend class QRGroupWidget;

protected:
    /// Group intialization
    QRGroup(unsigned groupId);
    /// Associates a component to this group.
    void attach(QRGroupWidget *widget);

    /// Detaches a component from this group.
    void detach(QRGroupWidget *widget);


public slots:
    /// Broadcasts a message to all nodes associated
    void doBroadcast(QRGroupWidget *emitter, QRMessage *msg);

signals:
    /// Signal is emitted when a message is broadcasted
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

/// Base class for components that can associate with groups.
class QRGroupWidget {
    friend class QRLink;
public:
    QRGroupWidget();
    ~QRGroupWidget();

    QRLink* getLink(int groupId);

   //@ {
         /// @name Group management

    /// return true if widget belongs to groupId
    bool belongsToGroup(int groupId);
    /// attaches to groupId
    void attachToGroup(int groupId);
    /// attaches only to groupId (removes other groups)
    void attachOnlyToGroup(int groupId);
    /// removes from groupId
    void detachFromGroup(int groupId);
    /// removes from every group
    void detachFromAllGroups();

  //@}

    //@ {
         /// @name Broadcast

    /// returns true if a message can be emitted
    bool isEmitterEnabled();
    /// enables or disables message emission
    void setEmitterEnabled(bool isOn);

    /// returns true if a message is to be received
    bool isRcvEnabled();
    /// enables or disables message reception
    void setRcvEnabled(bool isOn);

    //@}

protected:
    virtual void broadcastMsg(QRMessage *msg, int groupId = -1);
    virtual void receiveMsg(QRGroupWidget *emitter, QRMessage *msg) = 0;

protected:
    std::map<int, QRLink *> m_links;
    bool m_emitterEnabled;
    bool m_rcvEnabled;
};
}

#endif
