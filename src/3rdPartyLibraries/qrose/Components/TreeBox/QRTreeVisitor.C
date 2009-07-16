
#include <QRTreeVisitor.h>
#include <QRMain.h>
#include <QRTreeBox.h>
#include <QRAstInfo.h>

using namespace std;

namespace qrs {

QRTreeVisitor::QRTreeVisitor(QRTreeBox *tree) {
   m_tree = tree;
   m_id_counter = 0;
}


void QRTreeVisitor::setArguments(bool cacheView, Q3ListView *lv,
                           map<QRListViewItem *, SgNode *> *viewItems,
                           map<SgNode *, QRListViewItem *> *nodeItems,
                           map<std::string, unsigned> *capturedNames,
                           map<unsigned, std::string> *capturedIndices,
                           int detailLevel, bool cacheShowChildren)
{
   m_cacheView = cacheView;
   m_lv = lv;
   m_viewItems = viewItems;
   m_nodeItems = nodeItems;
   m_capturedNames = capturedNames;
   m_capturedIndices = capturedIndices;
   m_detailLevel = detailLevel;
   m_cacheShowChildren = cacheShowChildren;
}

void QRTreeVisitor::performTraversal(SgNode *node) {
   prepareTraversal();
   if (!m_cacheView || m_cacheShowChildren) {
	  QRTreeVisitorInfo rootNode;
      traverse(node, rootNode);
   }
}

void QRTreeVisitor::prepareTraversal() {
   if (!m_cacheView) return;
   // lay down all cache items
   set<SgNode *> *cache = m_tree->getCache();
   Q3ListViewItem *lastChildItem = NULL;
   for (set<SgNode *>::iterator iter = cache->begin(); iter != cache->end(); iter++) {
       SgNode *node = *iter; QRListViewItem *li;
       li = new QRListViewItem(m_lv, lastChildItem,
                       getNodeDesc(node).c_str());
       registerNode(li, node);
       lastChildItem = li;
   }
}

QRTreeVisitorInfo QRTreeVisitor::evaluateInheritedAttribute
          (SgNode *node, QRTreeVisitorInfo info)
{
    // possible traversal bug if this conditional yields false
    if ((info.level == 0) || (m_ids.find(node->get_parent()) != m_ids.end())) {
       m_ids[node] = m_id_counter;
       if (!m_cacheView)
          processNormalView(node, info);
       else {
	      processCacheView(node, info);
       }
       m_id_counter++;
    }

    info.level++;
    return info;
}

void QRTreeVisitor::registerNode (QRListViewItem *li, SgNode *node) {
   if (!li) return;
   map<SgNode *, QRListViewItem *>::iterator iter =
               m_nodeItems->find(node);
   if (iter != m_nodeItems->end())
     delete iter->second;
   (*m_nodeItems)[node] = li;
   m_lastChild[node] = NULL;

   li->setId(m_ids[node]);
   li->setNode(node);

   QPixmap *pixmap = QRAstInfo::getPixmap(node);
   if (pixmap) {
      li->setPixmap(0, *pixmap);
   }

   (*m_viewItems)[li] = node;

   if (!m_cacheView) {
      li->setOpen(QRAstInfo::isRelevant(node));
   }

   // handling attributes
   if (m_tree->isCaptureEnabled()) {

      map<string, AstAttribute *> attributes;
      m_tree->matchedAttributes(node, attributes);

      for (map<string, AstAttribute *>::iterator iter = attributes.begin();
   	      iter != attributes.end(); iter++)
      {
         // check if we have a column for this attribute
	 string attrib_name = iter->first;
	 map<string, unsigned>::iterator column_iter =
                     m_capturedNames->find(attrib_name);
	 unsigned column_index;
	 if (column_iter != m_capturedNames->end()) {
	    column_index = column_iter->second;
	 } else {
	    column_index = m_lv->addColumn(attrib_name.c_str());
	    (*m_capturedNames)[attrib_name] = column_index;
            (*m_capturedIndices)[column_index] = attrib_name;
   	 }
	 QRAttribute *qrattrib = m_tree->getAttribute(attrib_name);
	 li->setCellValue(column_index, qrattrib, iter->second->toString());
      }
   }
}


void QRTreeVisitor::processNormalView(SgNode *node, QRTreeVisitorInfo &info) {

   QRListViewItem *li = NULL;
   if (info.level == 0) {
      li = new QRListViewItem(m_lv, getNodeDesc(node).c_str());
   } else {
      SgNode *parent = node->get_parent();
      map<SgNode *, QRListViewItem *>::iterator iter = m_lastChild.find(parent);
      if (iter != m_lastChild.end()) {
         QRListViewItem *&last_child_item = iter->second;
	 if (!last_child_item) { // first child
            li = new QRListViewItem((*m_nodeItems)[parent],
                       getNodeDesc(node).c_str());
	 } else {
	    li = new QRListViewItem((*m_nodeItems)[parent], last_child_item,
                          getNodeDesc(node).c_str());
	 }
	 last_child_item = li;
      }
  }
  if (li)
     registerNode(li, node);
}

void QRTreeVisitor::processCacheView(SgNode *node, QRTreeVisitorInfo &info) {
   QRListViewItem *li = NULL;
   SgNode *parent = node->get_parent();
   QRListViewItem *parent_item = m_nodeItems->find(parent)->second;
   if (parent_item) {
      map<SgNode *, QRListViewItem *>::iterator iter = m_lastChild.find(parent);
      if (iter != m_lastChild.end()) {
         QRListViewItem *&last_child_item = iter->second;
         li = new QRListViewItem((*m_nodeItems)[parent],
                       last_child_item, getNodeDesc(node).c_str());
         last_child_item = li;
      }
      if (li) {
         registerNode(li, node);
      }
   }
}

string QRTreeVisitor::getNodeDesc(SgNode *node) {
   if (m_detailLevel == 0) {
       return node->class_name();
   } else if (m_detailLevel == 1) {
	   string str;
       str = QRAstInfo::getInfo(node);
       if (str.empty())
    	   str = QROSE::format("<%s>", node->class_name().c_str());
       return str;
   } else {
	   string info = QRAstInfo::getInfo(node);
	   string node_class = node->class_name();
       if (info.empty()) {
    	   return QROSE::format("<%s>", node_class.c_str());
       } else {
	      return QROSE::format("%s <%s>", info.c_str(), node_class.c_str());
       }
   }
}

}

