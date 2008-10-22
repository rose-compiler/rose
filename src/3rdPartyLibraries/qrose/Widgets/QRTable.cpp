#include <QRTable.h>
#include <QRTable_p.h>
#include <QRException.h>
#include <QHeaderView>
#include <QTableWidgetItem>

#include <stdarg.h>
#include <list>

using namespace std;

namespace qrs {

QRTable::QRTable() {
   _private = new QRTable_p(this);
   _private->init();
}


QRTable::QRTable(int numCols, ...) {
   _private = new QRTable_p(this);
   _private->init();
   eAssert(numCols > 0, ("invalid number of columns (%d) in QRTable constructor (> 0)",
                         numCols));
   addCols(numCols);
   va_list lst;   
   va_start(lst, numCols);
   for (int i = 0; i < numCols; i++) {
      const char* header = va_arg(lst, const char *);
      setText(header, i, QRTable::Header);
   }
}

QRTable::~QRTable() {
   delete _private;
}

void QRTable::addCols(int numCols, int colId) {
   int colCount = (int) columnCount();
   int rCount = (int) rowCount();
   eAssert(((colId >= -1) && (colId <= columnCount())), ("invalid column id (%d), must be less or equal than %d!",
                                    colId, colCount));
   if (colId < 0)
      colId = colCount;
   for (int i = 0; i < numCols; i++) {
      insertColumn(colId);
      for (int row = 0; row < rCount; row++) {
         QTableWidgetItem *item = _private->newCell();
         setItem(row, colId, item);
      }
      setHorizontalHeaderItem (colId, new QTableWidgetItem(""));
   }
}

void QRTable::removeCol(int colId) {
   eAssert(((colId >= 0) && (colId < columnCount())), ("column index (%d) out of bounds (0..%d)!", colId, columnCount()-1));

   removeColumn (colId); 
}


void QRTable::setColHeaders(const char *caption0, ...) {
   int colCount = (int) columnCount();
   eAssert(colCount > 0, ("no columns found - you need to add at least one column!"));
   va_list lst;   
   va_start(lst, caption0);
   setText(caption0, 0, QRTable::Header);
   for (int i = 1; i < colCount; i++) {
      const char* caption = va_arg(lst, const char *);
      setText(caption, i, QRTable::Header);
   }
}


void QRTable::showColHeader(bool show) {
   if (show)
      horizontalHeader()->show();
   else
      horizontalHeader()->hide();
}

void QRTable::addRows(int numRows, int rowId) {
   int colCount = (int) columnCount();
   int rCount = (int) rowCount();
   eAssert(rowId <= rCount, ("invalid row id (%d), must be less or equal than %d!",
                                    rowId, rCount));
   if (rowId < 0)
      rowId = rCount;
   for (int i = 0; i < numRows; i++) {
      insertRow(rowId);
      for (int column = 0; column < colCount; column++) {
         QTableWidgetItem *item = _private->newCell();
         setItem(rowId, column, item);
      }
      setVerticalHeaderItem (rowId, new QTableWidgetItem(""));
   }
}

void QRTable::removeRow(int rowId) {
   eAssert(((rowId >= 0) && (rowId < rowCount())), ("row index (%d) out of bounds (0..%d)!", rowId, rowCount()-1));

   QTableWidget::removeRow (rowId);
}

void QRTable::setRowHeaders(const char *caption0, ...) {
   int rCount = (int) rowCount();
   eAssert(rCount > 0, ("no rows found - you need to add at least one row!"));
   va_list lst;
   va_start(lst, caption0);
   setText(caption0, QRTable::Header, 0);
   for (int i = 1; i < rCount; i++) {
      const char* caption = va_arg(lst, const char *);
      setText(caption, QRTable::Header, i);
   }
}

void QRTable::showRowHeader(bool show) {
   if (show)
      verticalHeader()->show();
   else
      verticalHeader()->hide();
}

void QRTable::setText(string text, int col, int row) {
   _private->setCellsAttribute<string>(&QRTable_p::setText, text, col, row);
}

void QRTable::clearText(int col, int row) {
   _private->setCellsAttribute<string>(&QRTable_p::setText, "", col, row);
}
   
void QRTable::setPicture(const char *icon_filename, int col, int row)  {
   _private->setCellsAttribute<const char *>(&QRTable_p::setPicture, icon_filename, col, row);
}

void QRTable::setPicture(const char *xpm[], int col, int row) {
   _private->setCellsAttribute<const char *[]>(&QRTable_p::setPicture, xpm, col, row);
}

void QRTable::clearPicture(int col, int row) {
   _private->setCellsAttribute<void *>(&QRTable_p::clearPicture, 0, col, row);
}

void QRTable::setTextColor(QColor color, int col, int row) {
   _private->setCellsAttribute<QColor &>(&QRTable_p::setTextColor, color, col, row);
}

void QRTable::setBgColor(QColor color, int col, int row) {
   _private->setCellsAttribute<QColor &>(&QRTable_p::setBgColor, color, col, row);
}


void QRTable::setFont(QFont font, int col, int row) {
   _private->setCellsAttribute<QFont &>(&QRTable_p::setFont, font, col, row);
}

QFont QRTable::getFont(int col, int row) const {
  int colCount = (int) columnCount();
  int rCount = (int) rowCount();
   
  eAssert(((col >= 0) && (col < colCount) && (row >= 0) && (row < rCount)),
           ("(col=%d, row=%d) out of bounds: num cols=%d, num rows=%d!",
            col, row, colCount, rCount));
   
   return item(row, col)->font();
}

void QRTable::setType(Type type, int col, int row) {
   _private->setCellsAttribute<Type>(&QRTable_p::setType, type, col, row);
}

QRTable::Type QRTable::getType(int col, int row) const {
   int colCount = (int) columnCount();
   int rCount = (int) rowCount();
   
   eAssert(((col >= 0) && (col < colCount) && (row >= 0) && (row < rCount)),
           ("(col=%d, row=%d) out of bounds: num cols=%d, num rows=%d!",
            col, row, colCount, rCount));

   Qt::ItemFlags flags;
   flags = item(row, col)->flags();
   
   if (flags & Qt::ItemIsUserCheckable)
      return Check;
   else if (flags & Qt::ItemIsEditable)
      return Edit;
   else
      return Normal;
}

void QRTable::setChecked(bool checked, int col, int row) {
   _private->setCellsAttribute<bool>(&QRTable_p::setChecked, checked, col, row);
}

bool QRTable::isChecked(int col, int row) const {
   int colCount = (int) columnCount();
   int rCount = (int) rowCount();
   
   eAssert(((col >= 0) && (col < colCount) && (row >= 0) && (row < rCount)),
           ("(col=%d, row=%d) out of bounds: num cols=%d, num rows=%d!",
            col, row, colCount, rCount));

   QTableWidgetItem *witem = item(row, col);
   eAssert(isCheckable(col, row), ("cell (col=%d, row=%d) is not checkable!",
         col, row));
   
   return witem->checkState() == Qt::Checked;
}

bool QRTable::isCheckable(int col, int row) const {
   int colCount = (int) columnCount();
   int rCount = (int) rowCount();
   
   eAssert(((col >= 0) && (col < colCount) && (row >= 0) && (row < rCount)),
           ("(col=%d, row=%d) out of bounds: num cols=%d, num rows=%d!",
            col, row, colCount, rCount));

   QTableWidgetItem *witem = item(row, col);
   return witem->flags() & Qt::ItemIsUserCheckable;
}

void QRTable::setEnabled(bool enabled, int col, int row) {
   _private->setCellsAttribute<bool>(&QRTable_p::setEnabled, enabled, col, row);
}

bool QRTable::isEnabled(int col, int row) const {
   int colCount = (int) columnCount();
   int rCount = (int) rowCount();
   
   eAssert(((col >= 0) && (col < colCount) && (row >= 0) && (row < rCount)),
           ("(col=%d, row=%d) out of bounds: num cols=%d, num rows=%d!",
            col, row, colCount, rCount));

   return item(row, col)->flags() & Qt::ItemIsEnabled;
}

void QRTable::setHAlignment(bool left, bool right, int col, int row) {
   pair<bool, bool> data(left, right);
   _private->setCellsAttribute<pair<bool, bool> >(&QRTable_p::setHAlignment, data, col, row);

}

void QRTable::setVAlignment(bool top, bool bottom, int col, int row) {
   pair<bool, bool> data(top, bottom);
   _private->setCellsAttribute<pair<bool, bool> >(&QRTable_p::setVAlignment, data, col, row);
}


void QRTable::setHDim(int col, int width) {
   int colCount = (int) columnCount();

   eAssert(col != QRTable::Header, ("invalid horizontal width: QRTable::Header!"));
   eAssert((((col >= 0) && (col < colCount)) || col == QRTable::All),
           ("col=%d is out of bounds: num cols=%d!",
            col, colCount));

   for (int i = 0; i < colCount; i++) {
      bool hit = (col == QRTable::All) || (col == i);
      if (hit) {
         if (width == -1) {
            resizeColumnToContents(i);
         } else {
            setColumnWidth(i, width);
         }
      }
   }
}

void QRTable::setVDim(int row, int height) {
   int rCount = (int) rowCount();

   eAssert(row != QRTable::Header, ("invalid height: QRTable::Header!"));
   eAssert((((row >= 0) && (row < rCount)) || row == QRTable::All),
           ("row=%d is out of bounds: num rows=%d!",
            row, rCount));

   for (int i = 0; i < rCount; i++) {
      bool hit = (row == QRTable::All) || (row == i);
      if (hit) {
         if (height == -1) {
            resizeRowToContents(i);
         } else {
            setRowHeight(i, height);
         }
      }
   }
}

void QRTable::activateCell(int col, int row) {
   setCurrentCell(row, col);
}

void QRTable::activateEvent(int col, int row, int oldCol, int oldRow) {

}

void QRTable::changeEvent(int col, int row) {

}

void QRTable::clickEvent(int col, int row) {

}
   
void QRTable::dblClickEvent(int col, int row) {

}


   
} // namespace qrs




