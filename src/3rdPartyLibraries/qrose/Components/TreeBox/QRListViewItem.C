#include <qpainter.h>

#include <QRListViewItem.h>
#include <QRTreeBox.h>

#include <rose.h>

using namespace std;

namespace qrs {

QRListViewItem::QRListViewItem(Q3ListView *parent, const char *name): Q3ListViewItem(parent, name), m_node(NULL)
{

}
QRListViewItem::QRListViewItem( Q3ListView *parent, Q3ListViewItem *sibling, const char *name):
    Q3ListViewItem(parent, sibling, name), m_node(NULL)

{

}


QRListViewItem::QRListViewItem( Q3ListViewItem *parent, const char *name): Q3ListViewItem(parent, name), m_node(NULL)
{

}

QRListViewItem::QRListViewItem( Q3ListViewItem *parent, Q3ListViewItem *sibling, const char *name):
	Q3ListViewItem(parent, sibling, name), m_node(NULL)
{

}

void QRListViewItem::paintCell( QPainter *p, const QColorGroup &cg,
                            int column, int width, int alignment)

{
   p->save();
   QColorGroup cgn(cg);

   if (isWritable(column)) {
      cgn.setColor(QColorGroup::Base, QColor(231,255,216));
   }

   if (((QRTreeBox *) listView()->parent())->isHighlight()) {
      if (column == 0 && m_node && ((QRTreeBox *) listView()->parent())->isNodeInCache(m_node)) {
         //cgn.setColor(QColorGroup::Base, QColor(231,255,216));
         cgn.setColor(QColorGroup::Base, QColor(216,239,202));
      }
   }

   Q3ListViewItem::paintCell(p,cgn,column,width,alignment);

   // Draw a box around the Cell
   int nheight = height() - 1;
   int nwidth = width - 1;
   p->setPen(cg.color(QColorGroup::Background));
   p->drawLine(0,nheight,nwidth,nheight);

   //p->lineTo(nwidth, 0);
   QPainterPath path;
   path.lineTo(nwidth, 0);
   p->drawPath(path);

   p->restore();

}

int QRListViewItem::compare ( Q3ListViewItem * i, int col, bool ascending ) const {
  return ((QRTreeBox *) listView()->parent())->compareItems((QRListViewItem *) this,
          (QRListViewItem *) i, col, ascending);
}

unsigned QRListViewItem::getId() {
   return m_id;
}

void QRListViewItem::setId(unsigned id) {
   m_id = id;
}


void QRListViewItem::setNode(SgNode *node) {
   m_node = node;
}

SgNode* QRListViewItem::getNode() {
   return m_node;
}

AstAttribute* QRListViewItem::getAstAttribute(string attrib_name) {
   if (!m_node) return NULL;
   if (m_node->attributeExists(attrib_name)) {
      return m_node->getAttribute(attrib_name);
   } else {
      return NULL;
   }
}

QRAttribute* QRListViewItem::getQRattrib(int column) {
   if (column == 0) return NULL;
   Q3ListView *lv = listView();
   QRTreeBox *tree = (QRTreeBox *) lv->parent();
   string attrib_name;
   if (tree->getWidgetListView() == lv)
       attrib_name = tree->m_capturedIndices[column];
   else
       attrib_name = tree->m_capturedCacheIndices[column];

   QRAttribute *qrattrib = tree->getAttribute(attrib_name);
   return qrattrib;
}


bool QRListViewItem::isWritable(int column, QRAttribute *qrattrib) {
   if (column == 0) return false;
   if (qrattrib == NULL) {
      qrattrib = getQRattrib(column);
   }

   return getAstAttribute(qrattrib->name()) && qrattrib->canModify();
}

void QRListViewItem::setCellValue(int column, QRAttribute *qrattrib, string str) {
   QPixmap *pixmap = qrattrib->getPixmap(str);
   setText(column, str.c_str());
   if (pixmap) {
     setPixmap(column, *pixmap);
   }
}

}
