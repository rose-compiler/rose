#include <QRTable_p.h>
#include <QRMain.h>

#include <QObject>

using namespace std;

namespace qrs {

QRTable_p::QRTable_p(QRTable *table) {
   _table = table;
}

void QRTable_p::init() {
   _table->showColHeader(false);
   _table->showRowHeader(false);
   _table->setSelectionMode(QAbstractItemView::NoSelection);
   QROSE::link(_table, SIGNAL(cellChanged(int,int)), &changeEvent);
   QROSE::link(_table, SIGNAL(cellClicked(int,int)), &clickEvent);
   QROSE::link(_table, SIGNAL(cellDoubleClicked(int,int)), &dblClickEvent);
   QROSE::link(_table, SIGNAL(currentCellChanged (int,int,int,int)), &activateEvent);
   QObject::connect(_table, SIGNAL(changed(int,int)), _table, SLOT(changeEvent(int,int)));
   QObject::connect(_table, SIGNAL(clicked(int,int)), _table,  SLOT(clickEvent(int,int)));
   QObject::connect(_table, SIGNAL(dblClicked(int,int)), _table, SLOT(dblClickEvent(int,int)));
   QObject::connect(_table, SIGNAL(activated(int,int,int,int)), _table, SLOT(activateEvent(int,int,int,int)));
}

                                    
   
QTableWidgetItem* QRTable_p::newCell() {
   QTableWidgetItem *item = new QTableWidgetItem;
   setType(QRTable::Normal, item, QRTable::All, QRTable::All);
   // center horizontally and vertically
   setHAlignment(pair<bool,bool>(false, false), item, QRTable::All, QRTable::All);
   setVAlignment(pair<bool,bool>(false, false), item, QRTable::All, QRTable::All);
   return item;
}

void QRTable_p::setText(string text, QTableWidgetItem *item, int col, int row) {
   item->setText(text.c_str());
}

void QRTable_p::setPicture(const char *icon_filename, QTableWidgetItem *item, int col, int row) {
   item->setIcon(QIcon(icon_filename));
}

void QRTable_p::setPicture(const char *xpm[], QTableWidgetItem *item, int col, int row) {
   item->setIcon(QIcon(xpm));
}

void QRTable_p::clearPicture(void *, QTableWidgetItem *item, int col, int row) {
   item->setIcon(QIcon());
}
   
void QRTable_p::setTextColor(QColor &color, QTableWidgetItem *item, int col, int row) {
   item->setForeground(QBrush(color));
}
   
void QRTable_p::setBgColor(QColor &color, QTableWidgetItem *item, int col, int row) {
   item->setBackground(QBrush(color));
}

void QRTable_p::setFont(QFont &font, QTableWidgetItem *item, int col, int row) {
   item->setFont(font);
}

void QRTable_p::setType(QRTable::Type type, QTableWidgetItem *item, int col, int row) {
   eAssert(col != QRTable::Header && row != QRTable::Header, ("type cannot be set to header!"));
   Qt::ItemFlags flags;
   flags = item->flags();
   if (type == QRTable::Normal) {
      flags = flags & (~Qt::ItemIsUserCheckable) & (~Qt::ItemIsTristate) & (~Qt::ItemIsEditable);
      item->setData(Qt::CheckStateRole, QVariant()); // remove check-box
   } else if (type == QRTable::Edit) {
      flags = (flags & (~Qt::ItemIsUserCheckable) & (~Qt::ItemIsTristate)) | Qt::ItemIsEditable;
      item->setData(Qt::CheckStateRole, QVariant()); // remove check-box
   } else if (type == QRTable::Check) {
      flags = (flags & (~Qt::ItemIsEditable) & (~Qt::ItemIsTristate)) | Qt::ItemIsUserCheckable;
      item->setCheckState(Qt::Unchecked); // to appear the check-box
   } else {
      eAssert(0, ("wrong type for table cells!"));
   }
   item->setFlags(flags);
}

void QRTable_p::setChecked(bool checked, QTableWidgetItem *item, int col, int row) {
   eAssert(col != QRTable::Header && row != QRTable::Header, ("header is not checkable!"));
   eAssert(item->flags() & Qt::ItemIsUserCheckable, ("cell (row=%d, col=%d) is not checkable!",
         item->row(), item->column()));
   if (checked) {
      item->setCheckState(Qt::Checked);
   } else {
      item->setCheckState(Qt::Unchecked);
   }
}

void QRTable_p::setEnabled(bool enabled, QTableWidgetItem *item, int col, int row) {
   Qt::ItemFlags flags = item->flags();
   if (enabled) {
      item->setFlags(flags | Qt::ItemIsEnabled);
   } else {
      item->setFlags(flags & (~Qt::ItemIsEnabled));
   }
}

void QRTable_p::setHAlignment(std::pair<bool, bool> data, QTableWidgetItem *item, int col, int row) {
   bool left = data.first;
   bool right = data.second;

   int alignmentFlags = item->textAlignment() & ~0xF;

   if (left && right) { /* justified */
      alignmentFlags |= Qt::AlignJustify;
   } else if (!left && !right) { /* center */
      alignmentFlags |= Qt::AlignHCenter;
   } else if (left) {
      alignmentFlags |= Qt::AlignLeft;
   } else if (right) {
      alignmentFlags |= Qt::AlignRight;
   }

   item->setTextAlignment(alignmentFlags);
}
      
      
void QRTable_p::setVAlignment(std::pair<bool, bool> data, QTableWidgetItem *item, int col, int row) {
   bool top = data.first;
   bool bottom = data.second;

   int alignmentFlags = item->textAlignment() & ~0xF0;

   eAssert(!top || !bottom, ("invalid vertical alignment (top=true, bottom=true)!"));
   if (!top && !bottom) { /* center */
      alignmentFlags |= Qt::AlignVCenter;
   } else if (top) {
      alignmentFlags |= Qt::AlignTop;
   } else if (bottom) {
      alignmentFlags |= Qt::AlignBottom;
   }

   item->setTextAlignment(alignmentFlags);
}

void QRTable_p::changeEvent(int row, int col) {
   QRTable *table = QROSE::cbSender<QRTable *>();
   emit table->changed(col, row);
}
         
void QRTable_p::clickEvent(int row, int col) {
   QRTable *table = QROSE::cbSender<QRTable *>();
   emit table->clicked(col, row);
}
   
void QRTable_p::dblClickEvent(int row, int col) {
   QRTable *table = QROSE::cbSender<QRTable *>();
   if (table->getType(col, row) != QRTable::Edit)
      emit table->dblClicked(col, row);
}
   
void QRTable_p::activateEvent(int row, int col, int oldRow, int oldCol) {
   QRTable *table = QROSE::cbSender<QRTable *>();
   emit table->activated(col, row, oldCol, oldRow);
}


}
