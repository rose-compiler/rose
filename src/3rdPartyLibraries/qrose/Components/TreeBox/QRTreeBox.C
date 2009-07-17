/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>

#include <QRTreeBox.h>
#include <QRListViewItem.h>
#include <QRTreeVisitor.h>
#include <QRAstInfo.h>
#include <QRException.h>
#include <QRIcons.h>

using namespace std;

namespace qrs {

int QRTreeBox::statusTimer = 2000;

QRTreeBox::QRTreeBox()
{
    init(0);
}

QRTreeBox::QRTreeBox(SgNode *root)
{
    init(root);
}

QRTreeBox::~QRTreeBox() {
    delete m_wgPopupMenu;
}

void QRTreeBox::init(SgNode *root) {

    // toolbar
    m_wgToolbar = new QRToolBar(QROSE::LeftRight);

    m_wgToolbar->addToggleButton("target", RoseIcons::target);
    m_wgToolbar->insertSeparator();
    m_wgToolbar->addToggleButton("cache", RoseIcons::cacheview);
    m_wgToolbar->addToggleButton("collect", RoseIcons::cachecollect);
    m_wgToolbar->addButton("clear", RoseIcons::cacheclear);
    m_wgToolbar->addButton("broadcast", RoseIcons::cachebroadcast);
    m_wgToolbar->insertSeparator();
    m_wgToolbar->addToggleButton("highlight", RoseIcons::highlight);
    m_wgToolbar->addToggleButton("lookup", RoseIcons::lookup);

    QROSE::link(m_wgToolbar, SIGNAL(clicked(int)), QRTreeBox::toolbarClicked, this);

   // listview
   m_wgListView = new Q3ListView(this);
   m_wgListView->setShowSortIndicator(true);
   connect(m_wgListView,
           SIGNAL(rightButtonClicked(Q3ListViewItem *, const QPoint &, int)),
           this, SLOT(showPopupMenu(Q3ListViewItem *, const QPoint &, int)));
   m_wgListView->setDefaultRenameAction (Q3ListView::Accept);
   connect(m_wgListView, SIGNAL(doubleClicked(Q3ListViewItem *, const QPoint &, int)),
	   this, SLOT(doubleClickCell(Q3ListViewItem*, const QPoint&, int )));
   connect(m_wgListView, SIGNAL(itemRenamed(Q3ListViewItem *, int)),
           this, SLOT(modifiedCell(Q3ListViewItem *, int)));
   connect(m_wgListView->header(), SIGNAL(clicked(int)), this, SLOT(clickedHeader(int)));

   // cacheview
   m_wgCacheView = new Q3ListView(this);
   m_wgCacheView->setShowSortIndicator(true);
   connect(m_wgCacheView,
           SIGNAL(rightButtonClicked(Q3ListViewItem *, const QPoint &, int)),
           this, SLOT(showPopupMenu(Q3ListViewItem *, const QPoint &, int)));
   connect(m_wgCacheView, SIGNAL(doubleClicked(Q3ListViewItem *, const QPoint &, int)),
	   this, SLOT(doubleClickCell(Q3ListViewItem*, const QPoint&, int )));
    connect(m_wgCacheView, SIGNAL(itemRenamed(Q3ListViewItem *, int)),
           this, SLOT(modifiedCell(Q3ListViewItem *, int)));
   m_wgCacheView->setDefaultRenameAction (Q3ListView::Accept);
   connect(m_wgCacheView->header(), SIGNAL(clicked(int)), this, SLOT(clickedHeader(int)));

   // status bar
   m_wgStatusbar = new QStatusBar(this);
   m_wgStatusbar->setSizeGripEnabled(false);
   QLabel* label = new QLabel( m_wgStatusbar );
   m_wgStatusbar->addWidget(label, 1, true);
   connect(this,SIGNAL(setInfo(const QString&,int)),m_wgStatusbar,
            SLOT(message(const QString&,int)));
   connect(m_wgStatusbar, SIGNAL(messageChanged(const QString&)),
            label, SLOT(setText(const QString&)));

   // setting the layout
   m_wgLayout = new Q3VBoxLayout(this);
   m_wgLayout->add(m_wgToolbar);
   m_wgLayout->add(m_wgListView);
   m_wgLayout->add(m_wgCacheView);
   m_wgLayout->add(m_wgStatusbar);

   // popup menu for Project Tree Column
   m_wgPopupMenu = new Q3PopupMenu(this);
   m_wgPopupMenu->insertItem("use node as root", pm_set_root);
   m_wgPopupMenu->insertItem("use default root", pm_set_default_root);
   m_wgPopupMenu->insertSeparator();
   m_wgPopupMenu->insertItem("expand children", pm_expand);
   m_wgPopupMenu->insertItem("collapse children", pm_collapse);
   m_wgPopupMenu->insertItem("expand code style", pm_expand_code);
   connect(m_wgPopupMenu, SIGNAL(activated(int)),
           this, SLOT(popupMenuHandler(int)));

   m_wgPopupCacheMenu = new Q3PopupMenu(this);
   m_wgPopupCacheMenu->insertItem("remove node", pm_remove);
   m_wgPopupCacheMenu->insertSeparator();
   m_wgPopupCacheMenu->insertItem("expand children", pm_expand);
   m_wgPopupCacheMenu->insertItem("collapse children", pm_collapse);
   m_wgPopupCacheMenu->insertItem("expand code style", pm_expand_code);
   connect(m_wgPopupCacheMenu, SIGNAL(activated(int)),
           this, SLOT(popupMenuHandler(int)));

   m_pressedKey = 0;


   // defaults
   m_defaultRootNode = m_rootNode = root;
   setTargetMode(true);
   setCollectMode(false);
   setLookup(false);
   setHighlight(false);
   setDetailLevel(1);
   setCacheShowChildren(true);

   if (m_rootNode == 0) {
      setCacheView(true);
      m_wgToolbar->setEnabled(tbCacheView, false);
      m_project = NULL;
   } else {
      m_wgToolbar->setEnabled(tbCacheView, true);
      setCacheView(false);
      m_project = root->get_parent()? TransformationSupport::getProject(root): NULL;
   }

   // it will be true the first time is shown.
   m_hasShown = false;
}

void QRTreeBox::toolbarClicked(int index) {
	QRTreeBox *treebox = QROSE::cbData<QRTreeBox *>();
	QRToolBar *tb = QROSE::cbSender<QRToolBar *>();
	switch (index) {
	case tbTargetMode: treebox->setTargetMode(tb->isChecked(index)); break;
	case tbCacheView: treebox->setCacheView(tb->isChecked(index)); break;
	case tbCollectMode: treebox->setCollectMode(tb->isChecked(index)); break;
	case tbClearCache: treebox->clearCache(); break;
	case tbBroadcastCache: treebox->broadcastCache(); break;
	case tbHighlight: treebox->setHighlight(tb->isChecked(index)); break;
	case tbLookup: treebox->setLookup(tb->isChecked(index)); break;
	}
}

SgNode *QRTreeBox::getRoot() {
   return m_defaultRootNode;
}

void QRTreeBox::setDetailLevel(int level) {
   m_detailLevel = level;
}

void QRTreeBox::setCacheShowChildren(bool isOn) {
   m_cacheShowChildren = isOn;
}

void QRTreeBox::showToolbar(bool setOn) {
   m_wgToolbar->setHidden(!setOn);
}

void QRTreeBox::initView() {
   loadTree(true);
   loadTree(false);
}

bool QRTreeBox::isCacheView() {
   return m_isCacheView;
}

bool QRTreeBox::isHighlight() {
   return m_isHighlight;
}

bool QRTreeBox::isLookup() {
   return m_isLookup;
}

/**************************************************************************[SLOTS]**/
void QRTreeBox::setTargetMode(bool isOn) {

   QRoseComponent::setTargetMode(isOn);
   emit setInfo(QString("target mode: %1 [%2]")
                         .arg(isOn? "ON": "OFF")
                         .arg(isOn? "accepting incoming nodes":
                                    "rejecting incoming nodes"), statusTimer);

   m_wgToolbar->setChecked(tbTargetMode, isOn);
}

void QRTreeBox::setCollectMode(bool isOn) {
   QRoseComponent::setCollectMode(isOn);
   emit setInfo(QString("collect mode: %1 [%2]")
               .arg(isOn? "ON": "OFF")
               .arg(isOn? "new nodes are appended to the cache":
                          "new nodes overwrite existing nodes in the cache"), statusTimer);
   m_wgToolbar->setChecked(tbCollectMode, isOn);

}

void QRTreeBox::clearCache() {
   QRoseComponent::clearCache(true);
}

void QRTreeBox::broadcastCache() {
if (cacheSize() == 0) return;
   QRoseComponent::broadcastCache();
   emit setInfo(QString("cache broadcasted [%1 %2]!").arg(cacheSize()).arg(cacheSize() == 1? "node": "nodes"), statusTimer);
}

void QRTreeBox::setCacheView(bool isOn) {
   m_isCacheView = isOn;

   m_wgListView->setHidden(isOn);
   m_wgCacheView->setHidden(!isOn);

   emit setInfo(QString("cache view mode: %1")
                        .arg(isOn? "ON": "OFF"), statusTimer);


   m_wgToolbar->setChecked(tbCacheView, isOn);
}

void QRTreeBox::setHighlight(bool isOn) {
    m_isHighlight = isOn;
    refresh();
    emit setInfo(QString("highlight mode: %1 [%2]")
                        .arg(isOn? "ON": "OFF")
                        .arg(isOn? "cache items will be highlighted" : "cache items will not be highlighted")
                        , statusTimer);
    m_wgToolbar->setChecked(tbHighlight, isOn);
}

void QRTreeBox::setLookup(bool isOn) {
   m_isLookup = isOn;
   refresh();
   emit setInfo(QString("lookup mode: %1 [%2]")
                        .arg(isOn? "ON": "OFF")
                        .arg(isOn? "last received item will be searched automatically" :
                                   "last received item will not be searched")
                        , statusTimer);
   m_wgToolbar->setChecked(tbLookup, isOn);
}

void QRTreeBox::updateCacheViewButton() {
   unsigned n = cacheSize();
   if (n == 0) {
      m_wgToolbar->setCaption(tbCacheView, "cache");
   } else {
      m_wgToolbar->setCaption(tbCacheView, QString("cache [%1]").arg(n).toStdString());
   }
}

void QRTreeBox::eventCacheClear() {

   loadTree(false);
   if (!isCacheView() && isHighlight()) {
      refresh();
   }
   updateCacheViewButton();
   emit setInfo("cache cleared", statusTimer);
}

void QRTreeBox::eventCacheNewNodes(set<SgNode *> &nodes) {
   updateCacheViewButton();
   loadTree(false);
   if (!isCacheView() && isHighlight())
      refresh();
   emit setInfo(QString("%1 %2 been received [total=%3]")
                .arg(nodes.size()).arg((nodes.size() == 1)? "node has":
                                 "nodes have").arg(cacheSize()),statusTimer);
}

void QRTreeBox::eventCacheRemovedNodes(std::set<SgNode *> &nodes) {
   updateCacheViewButton();
   loadTree(false);
   if (!isCacheView() && isHighlight()) {
      refresh();
   }
   emit setInfo(QString("%1 %2 been removed [total=%3]")
                .arg(nodes.size()).arg((nodes.size() == 1)? "node has":
                                 "nodes have").arg(cacheSize()),statusTimer);
}

Q3ListView* QRTreeBox::getCurrentView() {
   if (isCacheView())
      return m_wgCacheView;
   else
      return m_wgListView;
}

void QRTreeBox::receiveMsg(QRGroupWidget *emitter, QRMessage *msg) {
   switch (msg->id()) {
      case msg_node:
          {
	     SgNode *node = ((QRoseComponent::SgNodeMsg *) msg)->getNode();
             if (isTargetMode()) {
                insertNodeInCache(node);
             }
             lookupNode(node);
	  }
	  break;
      case msg_nodes:
          {
             set<SgNode *> *nodes = ((SgNodesMsg *) msg)->getNodes();
             if (isTargetMode()) {
                insertNodesInCache(nodes);
             }
          }
	  break;
   }
}

void QRTreeBox::lookupNode(SgNode *node) {
   if (isLookup()) {
      if (m_nodeItems.find(node) != m_nodeItems.end()) {
         QRListViewItem *selectedItem = m_nodeItems[node];
	 if (selectedItem) {
   	    m_wgListView->setSelected(selectedItem, true);
            m_wgListView->ensureItemVisible(selectedItem);
	 }
      }
   }
}

void QRTreeBox::loadTree(bool doListView) {
   Q3ListView *lv = doListView? m_wgListView : m_wgCacheView;

   map<QRListViewItem *, SgNode *> *vi = doListView? (&m_viewItems) : (&m_viewCacheItems) ;
   map<SgNode *, QRListViewItem *> *ni = doListView? (&m_nodeItems) : (&m_nodeCacheItems);
   map<string, unsigned> *cn = doListView? (&m_capturedNames) : (&m_capturedCacheNames);
   map<unsigned, string> *ci = doListView? (&m_capturedIndices) : (&m_capturedCacheIndices);

   SgNode *rootNode = doListView? m_rootNode : m_project;

   if (!doListView && !rootNode) {
      // we will assume that the universe is the project of the first cache node
      set<SgNode *> *cache = getCache();
      if (!cache->empty()) {
         SgNode *node = *(cache->begin());
         // if a node is not associated to a project (e.g. types), then
         // getProject() aborts the program. This is a small fix to avoid this.
         if (node->get_parent()) {
            rootNode = TransformationSupport::getProject(node);
         }
      }
   }

   vi->clear();
   ni->clear();
   cn->clear();
   ci->clear();
   lv->clear();
   while (lv->columns())
      lv->removeColumn(0);

   lv->addColumn("Nodes", 600);
   lv->setAllColumnsShowFocus (true);
   lv->setSorting(0);
   lv->setShowSortIndicator(false);
   lv->setRootIsDecorated(true);
   m_sortColumn = 0;
   m_sortCacheColumn = 0;

   QRTreeVisitor visitor(this);


   visitor.setArguments(!doListView, lv, vi, ni, cn, ci, m_detailLevel, m_cacheShowChildren);
   emit setInfo("rendering tree... please wait...", 0);
   QROSE::processEvents();
   visitor.performTraversal(rootNode);

   emit setInfo("done!", statusTimer);

}


void QRTreeBox::clickedHeader(int column) {

   int* sortColumn = isCacheView()?  &m_sortCacheColumn : &m_sortColumn;
   bool* sortAscending = isCacheView()? &m_sortCacheAscending : &m_sortAscending;
   Q3ListView *lv = isCacheView()? m_wgCacheView : m_wgListView;

    if (column == 0) {
	if (*sortColumn != 0) {
	    lv->setSorting(0, true);
	    lv->sort();
            lv->setShowSortIndicator(false);
	} else {
	    lv->setSorting(-1);
	}
    } else {
        if (column != *sortColumn) {
	    *sortAscending = true;
        } else {
	    *sortAscending = !*sortAscending;
        }
        lv->header()->setSortIndicator(column, *sortAscending);
        lv->setSorting(column, *sortAscending);
        lv->sort();
    }
    *sortColumn = column;
}

int QRTreeBox::compareItems(QRListViewItem *item1, QRListViewItem *item2, int col, bool ascending) {
    if (col == 0) {
        // sorting for column 0 (Project Tree) is just the order in which they were created
	if (item1->getId() < item2->getId()) {
	    return -1;
	} else if (item1->getId() == item2->getId()) {
	    return 0;
	} else {
	    return 1;
	}
    } else {
	// find attribute name corresponding to 'col' index
	std::map<unsigned, std::string>::iterator iter;
        if (isCacheView()) iter = m_capturedCacheIndices.find(col); else
                           iter = m_capturedIndices.find(col);
	eAssert(iter != m_capturedIndices.end(), ("internal error: cannot find attribute!"));

        string attrib_name = iter->second;
	QRAttribute *qrattrib = getAttribute(attrib_name);

	AstAttribute *att_ast1 = item1->getAstAttribute(attrib_name);
	AstAttribute *att_ast2 = item2->getAstAttribute(attrib_name);

	return qrattrib->compare(att_ast1, att_ast2, ascending);
    }
}

void QRTreeBox::doubleClickCell(Q3ListViewItem *item, const QPoint &point, int column) {
    if (column == 0) {
	if (m_pressedKey == Qt::Key_Control) {
           bool targetMode = isTargetMode();
           if (isCacheView()) QRoseComponent::setTargetMode(false);
   	       broadcastNode(((QRListViewItem *) item)->getNode());
           item->setOpen(!item->isOpen());
           if (isCacheView()) QRoseComponent::setTargetMode(targetMode);
        }
    } else {
	QRAttribute *qrattrib = ((QRListViewItem *) item)->getQRattrib(column);
	if (!qrattrib) return;

	AstAttribute *ast_attrib = ((QRListViewItem *) item)->getAstAttribute(qrattrib->name());

	if (ast_attrib && qrattrib->canModify()) {
	    if (qrattrib->canEdit()) {
		item->setRenameEnabled(column, true);
		item->startRename(column);
		item->setRenameEnabled(column, false);
	    } else {
		qrattrib->fromString(ast_attrib, qrattrib->getNextValue(ast_attrib->toString()));
		((QRListViewItem *) item)->setCellValue(column, qrattrib, ast_attrib->toString());
                 syncCell((QRListViewItem *)item, column, qrattrib);
	    }
	    item->setOpen(!item->isOpen());
	}
    }
}

void QRTreeBox::modifiedCell(Q3ListViewItem *item, int column) {
    QString text = item->text(column);
    QRAttribute *qrattrib = ((QRListViewItem *) item)->getQRattrib(column);
    AstAttribute *ast_attrib = ((QRListViewItem *) item)->getAstAttribute(qrattrib->name());

    if (ast_attrib) {
       qrattrib->fromString(ast_attrib, text.toStdString());
    }

   ((QRListViewItem *) item)->setCellValue(column, qrattrib, ast_attrib->toString());
   syncCell((QRListViewItem *) item, column, qrattrib);
}

void QRTreeBox::syncCell(QRListViewItem *item, int column, QRAttribute *qrattrib) {
   // Q3ListView *lv = isCacheView()? m_wgListView : m_wgCacheView;
   map<SgNode *, QRListViewItem *> *ni = isCacheView()? &m_nodeItems : &m_nodeCacheItems;
   map<string, unsigned>  *cn = isCacheView()? &m_capturedNames : &m_capturedCacheNames;
   SgNode *node = item->getNode();
   if (!node) return;
   map<SgNode *, QRListViewItem *>::iterator iter;
   if ((iter = ni->find(node)) == ni->end()) return;
   QRListViewItem *syncItem = iter->second;

   // get column
   map<string, unsigned>::iterator iter1;
   if ((iter1 = cn->find(qrattrib->name())) == cn->end()) return;

   int syncColumn = iter1->second;

   syncItem->setCellValue(syncColumn, qrattrib, item->text(column).toStdString());

}

void QRTreeBox::showPopupMenu(Q3ListViewItem *item, const QPoint &point, int column) {
    if (item && column == 0) {
        if (isCacheView()) {
            m_wgPopupCacheMenu->setItemEnabled(pm_remove, item->parent() == NULL);
 	    m_wgPopupCacheMenu->exec(point);
        } else {
	   m_wgPopupMenu->exec(point);
        }
    }
}

void QRTreeBox::popupMenuHandler(int id) {

    QRListViewItem *selected_item = (QRListViewItem *) getCurrentView()->selectedItem();
    SgNode *selected_node = selected_item->getNode();
    switch (id) {
	case pm_set_root:
	    m_rootNode = selected_item->getNode();
	    loadTree(true);
	    selected_item = m_nodeItems[selected_node];
	    m_wgListView->setSelected(selected_item, true);
	    m_wgListView->setOpen(selected_item, true);
   	    break;
	case pm_set_default_root:
	    m_rootNode = m_defaultRootNode;
	    loadTree(true);
	    selected_item = m_nodeItems[selected_node];
	    m_wgListView->setSelected(selected_item, true);
	    m_wgListView->ensureItemVisible(selected_item);
	    break;
        case pm_remove:
            removeNodeFromCache(selected_node);
            break;
	case pm_expand:
	case pm_collapse:
	case pm_expand_code:
	    {
		list<QRListViewItem *> items;
		items.push_back(selected_item);

		while (items.size() != 0) {
   		    QRListViewItem *item;
		    item = items.front(); items.erase(items.begin());
		    if ((item == selected_item) && (id != pm_collapse)) {
			item->setOpen(true);
		    } else {
   		       item->setOpen((id == pm_expand_code)?
                          QRAstInfo::isRelevant(item->getNode()) : id == pm_expand);
		    }
		    QRListViewItem *child_item = (QRListViewItem *) item->firstChild();
		    while (child_item) {
                        items.push_back(child_item);
                        child_item = (QRListViewItem *) child_item->nextSibling();
                    }
		}
	    }
	    break;
    }
}


void QRTreeBox::keyPressEvent(QKeyEvent *event) {
    m_pressedKey = event->key();
}

void QRTreeBox::keyReleaseEvent(QKeyEvent *event) {
    m_pressedKey = 0;
}

map<QRListViewItem *, SgNode *>* QRTreeBox::viewItems() {
   return &m_viewItems;
}

map<SgNode *, QRListViewItem *>* QRTreeBox::nodeItems() {
   return &m_nodeItems;
}

void QRTreeBox::refresh() {
    m_wgCacheView->viewport()->repaint();
    m_wgListView->viewport()->repaint();
}

void QRTreeBox::showEvent (QShowEvent *event) {
	if (!m_hasShown) {
	   m_hasShown = true;
	   initView();
	}
}

}
