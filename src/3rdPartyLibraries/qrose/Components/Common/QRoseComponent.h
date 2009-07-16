/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   Class: QRoseComponent                                                 *
 ***************************************************************************/

#ifndef QROSECOMPONENT_H
#define QROSECOMPONENT_H


#include <QRGroup.h>
#include <set>

class SgNode;

namespace qrs {

/**
* \ingroup qrose_comps
*/

/// Base class for components that contain an IR node cache and broadcast capabilities
class QRoseComponent: public QRGroupWidget {

public:
    typedef enum { msg_node, msg_nodes } typeMsgId;

    QRoseComponent();

    std::set<SgNode *>* getCache() { return &m_cache; };


    //@{
          /// @name Cache properties

          /// Returns number of IR nodes in cache
    unsigned cacheSize();
          /// Returns true if @a node is in cache
    bool isNodeInCache(SgNode *node);
          /// Sets component to receive nodes and place them in cache
    void setTargetMode(bool isOn);
          /// Returns true if component is in target mode
    bool isTargetMode();
          /// Sets whether component clears the cache before receiving a node (or nodes)
    void setCollectMode(bool isOn);
          /// Returns true if component is in accumulation mode
    bool isCollectMode();

    //@}

    //@{
          /// @name Cache

          /// Inserts node in cache and optionally triggers event
    void insertNodeInCache(SgNode *node, bool emit_event = true);
          /// Inserts a list of nodes in cache and optionally triggers event
    void insertNodesInCache(std::set<SgNode *>*nodes, bool emit_event = true);
          /// Removes node from cache and optionally triggers event
    void removeNodeFromCache(SgNode *node, bool emit_event = true);
          /// Clears all elements from cache, and optionally triggers event
    void clearCache(bool emit_event = true);

    //@}

    //@{

          /// @name Broadcast

          /// Broadcasts node to all group components (-1 = first group)
    void broadcastNode(SgNode *node, int groupId = -1);
          /// Broadcast all elements of the cache to group components
    void broadcastCache(int groupId = -1);

    //@}

    //@{
          /// @name Event Handlers

          /// Implements default behaviour for receiving a message
    virtual void receiveMsg(QRGroupWidget *emitter, QRMessage *msg);

          /// Event handler called when cache is cleared
    virtual void eventCacheClear();
          /// Event handler called when cache has one or more new nodes
    virtual void eventCacheNewNodes(std::set<SgNode *> &nodes);
          /// Event handler called when nodes are removed from cache
    virtual void eventCacheRemovedNodes(std::set<SgNode *> &nodes);

    //@}

    /// Message that carries an IR node
    class SgNodeMsg: public QRMessage {
	public:
           /// Initializes a message containing one node
	   SgNodeMsg(SgNode *node);
           /// Returns node associated with message
	   SgNode *getNode();

	protected:
	   SgNode *m_node;
    };

     /// Message that carries a list of nodes
    class SgNodesMsg: public QRMessage {
	public:
           /// Initializes a message containing a list of nodes
	   SgNodesMsg(std::set<SgNode *> *nodes);
           /// Returns a list of nodes associated with message
	   std::set<SgNode *> *getNodes();

	protected:
	   std::set<SgNode *> m_nodes;
    };

protected:
    void initComponent();


protected:
    std::set<SgNode *> m_cache;
    bool m_isCollectMode;
    bool m_isTargetMode;
};

}

#endif

