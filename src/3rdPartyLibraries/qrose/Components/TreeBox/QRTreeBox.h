/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   Class: QRTreeBox                                                      *
 ***************************************************************************/

#ifndef QRTREEBOX_H
#define QRTREEBOX_H

#include <map>
#include <string>

#include <Qt3Support>
#include <QtGui>

#include <QRToolBar.h>
#include <QRoseComponent.h>
#include <QRAttribute.h>
#include <QRMacros.h>

class SgProject;

namespace qrs {

class QRListViewItem;

/**
* \ingroup qrose_comps
*/

/// Component that implements ROSE IR Traversal
class QRTreeBox: public QWidget, public QRoseComponent, public QRAttributeWidget {

    Q_OBJECT

    friend class QRTreeVisitor;
    friend class QRListViewItem;

public:
	enum ToolbarIdx { tbTargetMode = 0, tbCacheView, tbCollectMode, tbClearCache, tbBroadcastCache, tbHighlight, tbLookup };

    //@{
            /// @name Initialization

            /// Creates a tree-box associated to group 0 in cache-view mode only
   QRTreeBox();
            /// Creates a tree-box associated to group 0 with IR root @a root
   QRTreeBox(SgNode *root);
            /// Creates a tree-box associated to group @a group with IR root @a root
   ~QRTreeBox();
   //@}

    //@{
            /// @name Component properties

            /// Returns true if its in cache-view mode
   bool isCacheView();
            /// Returns true if cache items are highlight
   bool isHighlight();
            /// Returns true if component finds tree location when a node is received
   bool isLookup();
            /// Returns root node associated with this component
   SgNode *getRoot();
    //@}

    //@{
             /// @name Component settings

   void showToolbar(bool setOn);
            /// Sets description level for each tree item (0=class name, 1=class name or code description, 2=class name and description
   void setDetailLevel(int level);
            /// Sets cache-view to display cache items with its children
   void setCacheShowChildren(bool isOn);
            /// Displays tree item associated with @a node
   void lookupNode(SgNode *node);
    //@}

public slots:
    void setTargetMode(bool isOn);
    void setCollectMode(bool isOn);
    void clearCache();
    void broadcastCache();
             /// Set cache view mode [shows nodes in cache]
    void setCacheView(bool isOn);
            /// Sets component to highlight cache-nodes
    void setHighlight(bool isOn);
            /// Sets component to lookup source position of node received
    void setLookup(bool isOn);
    virtual void initView();

signals:
            /// signal emitted  when component status is changed
    void setInfo(const QString &str, int display_time);

protected slots:
    virtual void showPopupMenu(Q3ListViewItem *item, const QPoint &point, int column);
    virtual void popupMenuHandler(int id);
    virtual void doubleClickCell(Q3ListViewItem *item, const QPoint &point, int column);
    virtual void modifiedCell(Q3ListViewItem *item, int column);
    virtual void clickedHeader(int column);

protected:
   // callback to handle toolbar clicks
   static void toolbarClicked(int);

   // popup menu: project tree nodes
   typedef enum popmenu_nodes_id { pm_set_root, pm_set_default_root,
                 pm_remove, pm_expand, pm_collapse, pm_expand_code};


    //@{
          /// @name Event Handlers (overridden methods)
    virtual void eventCacheClear();
    virtual void eventCacheNewNodes(std::set<SgNode *> &nodes);
    virtual void eventCacheRemovedNodes(std::set<SgNode *> &nodes);
    virtual void receiveMsg(QRGroupWidget *emitter, QRMessage *msg);

    //@}

    virtual void init(SgNode *root);
    void refresh();
    void loadTree(bool doListView);

    void updateCacheViewButton();
    std::map<QRListViewItem *, SgNode *>* viewItems();
    std::map<SgNode *, QRListViewItem *>* nodeItems();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    Q3ListView* getCurrentView();

    int compareItems(QRListViewItem *item1, QRListViewItem *item2, int col,
                      bool ascending);

    void syncCell(QRListViewItem *item, int column, QRAttribute *qrattrib);

    virtual void showEvent (QShowEvent *event);
    static int statusTimer;

protected:
	bool m_hasShown;
    bool m_isCacheView;
    bool m_isHighlight;
    bool m_isLookup;

    int m_pressedKey;
    int m_detailLevel;
    bool m_cacheShowChildren;

    SgProject *m_project;
    SgNode *m_rootNode;
    SgNode *m_defaultRootNode;

    // maps between SgNode and ListViewItem
    std::map<QRListViewItem *, SgNode *> m_viewItems;
    std::map<SgNode *, QRListViewItem *> m_nodeItems;
    std::map<QRListViewItem *, SgNode *> m_viewCacheItems;
    std::map<SgNode *, QRListViewItem *> m_nodeCacheItems;
    std::map<std::string, unsigned> m_capturedNames;
    std::map<unsigned, std::string> m_capturedIndices;
    std::map<std::string, unsigned> m_capturedCacheNames;
    std::map<unsigned, std::string> m_capturedCacheIndices;

    /* sort state */
    int m_sortColumn;
    bool m_sortAscending;
    int m_sortCacheColumn;
    bool m_sortCacheAscending;

    /* widgets */
    QR_EXPORT(QRToolBar, Toolbar);
    QR_EXPORT(QStatusBar, Statusbar);
    QR_EXPORT(Q3ListView, ListView);
    QR_EXPORT(Q3ListView, CacheView);
    QR_EXPORT(Q3VBoxLayout, Layout);
    QR_EXPORT(Q3PopupMenu, PopupMenu);
    QR_EXPORT(Q3PopupMenu, PopupCacheMenu);

};
}

#endif

