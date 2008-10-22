/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRTable_p (private)     
    Description: 
 ***************************************************************************/

#include <QRTable.h>
#include <QRException.h>
#include <list>

namespace qrs {

class QRTable_p {
   friend class QRTable;
   
private:
   QRTable_p(QRTable *table);
   void init();   
   QTableWidgetItem* newCell();

   // change attribute methods
   static void setText(std::string text, QTableWidgetItem *item, int col, int row);
   static void setPicture(const char *icon_filename, QTableWidgetItem *item, int col, int row);
   static void setPicture(const char *xpm[], QTableWidgetItem *item, int col, int row);
   static void clearPicture(void *, QTableWidgetItem *item, int col, int row);
   static void setTextColor(QColor &color, QTableWidgetItem *item, int col, int row);
   static void setBgColor(QColor &color, QTableWidgetItem *item, int col, int row);
   static void setFont(QFont &font, QTableWidgetItem *item, int col, int row);
   static void setType(QRTable::Type type, QTableWidgetItem *item, int col, int row);
   static void setChecked(bool checked, QTableWidgetItem *item, int col, int row);
   static void setEnabled(bool enabled, QTableWidgetItem *item, int col, int row);
   static void setHAlignment(std::pair<bool, bool> data, QTableWidgetItem *item, int col, int row);
   static void setVAlignment(std::pair<bool, bool> data, QTableWidgetItem *item, int col, int row);
   template <class T>
      void setCellsAttribute(void (*fn) (T, QTableWidgetItem *, int, int), T data, int col, int row) {
         int colCount = (int) _table->columnCount();
         int rCount = (int) _table->rowCount();
         
         if (col >= 0) {
            eAssert(col < (int) colCount, ("invalid column index (%d), must be less or equal than %d!",
                                                col, colCount));
         }
         if (row >= 0) {
            eAssert(row < (int) rCount, ("invalid row index (%d), must be less or equal than %d!",
                                                row, rCount));
         }

         std::list<QTableWidgetItem *> items;

         if (col >= 0 && row >= 0) { /* cell */
            items.push_back(_table->item(row, col));
         } else if (col >= 0 && row == QRTable::All) { /* set all cells in column "col" */
            for (int i = 0; i < rCount; i++) {
               items.push_back(_table->item(i, col));
            }
         } else if (row >= 0 && col == QRTable::All) { /* set all cells in row "row" */
            for (int i = 0; i < colCount; i++) {
               items.push_back(_table->item(row, i));
            }
         } else if (col >= 0 && row == QRTable::Header) { /* set horizontal header cell */
            _table->showColHeader();
            items.push_back(_table->horizontalHeaderItem(col));
         } else if (col == QRTable::All && row == QRTable::Header) { /* set horizontal header */
            _table->showColHeader();
            for (int i = 0; i < colCount; i++) {
                items.push_back(_table->horizontalHeaderItem(i));
            }
         } else if (row >= 0 && col == QRTable::Header) { /* set vertical header cell */
            _table->showRowHeader();
            items.push_back(_table->verticalHeaderItem(row));
         } else if (row == QRTable::All && col == QRTable::Header) { /* set vertical header */
            _table->showRowHeader();
            for (int i = 0; i < rCount; i++) {
                items.push_back(_table->verticalHeaderItem(i));
            }            
         }  else if (row == QRTable::Header && col == QRTable::Header) { /* set all header cells */
            _table->showColHeader();
            _table->showRowHeader();
            for (int i = 0; i < rCount; i++) {
                items.push_back(_table->verticalHeaderItem(i));
            }            
            for (int i = 0; i < colCount; i++) {
                items.push_back(_table->horizontalHeaderItem(i));
            }
         } else if (row == QRTable::All && col == QRTable::All) { /* set all cells */
            for (int i = 0; i < rCount; i++) {
               for (int j = 0; j < colCount; j++) {
                  items.push_back(_table->item(i, j));
               }
            }
         }
         for (std::list<QTableWidgetItem *>::iterator iter = items.begin(); iter != items.end(); iter++) {
            QTableWidgetItem *item = *iter;
            if (item) (*fn) (data, item, col, row);
         }
      }

   static void changeEvent(int row, int col);
   static void clickEvent(int row, int col);
   static void dblClickEvent(int row, int col);
   static void activateEvent(int row, int col, int oldRow, int oldCol);
   
protected:
   QRTable *_table;
};

} // namespace qrs
