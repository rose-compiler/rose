/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRTable     
    Description: 
 ***************************************************************************/

#ifndef QRTABLE_H
#define QRTABLE_H

#include <QRMacros.h>
#include <QTableWidget>
#include <QColor>

namespace qrs {

class QRTable_p;
   
class QRTable: public QTableWidget {

   friend class QRTable_p;

   Q_OBJECT
   
public:
   typedef enum { Normal, Edit, Check } Type;
   typedef enum { All = -1, Header = -2 } Index;
   QRTable();
   
   QRTable(int numCols, ...);
   ~QRTable();

   // colId is the column after which to insert n columns
   // if colId = -1, then it adds columns at the end
   void addCols(int numCols, int colId = -1);
   void removeCol(int colId);   
   void setColHeaders(const char *caption0, ...);
   void showColHeader(bool show = true);
   
   void addRows(int numRows, int rowId = -1);
   void removeRow(int rowId);
   void setRowHeaders(const char *caption0, ...);
   void showRowHeader(bool show = true);

   // the following methods allow you to set the attributes of one or more cells
   // col=c, row=r : sets attribute of cell (c,r)
   // col=All, row=r: sets attribute of row r
   // col=c, row=All, sets attributes of column c
   // col=All, row=All: sets attributes of all cells
   // col=c, row=Header: sets attributes of column header c
   // col=Header, row=r: sets attribute of row header r
   // col=All, row=Header: sets attributes of all column headers
   // col=Header, row=All: sets attribute of all row headers
   // col=Header, row=Header: sets atttributes of all headers
   void setText(std::string text, int col, int row = All);
   void clearText(int col, int row = All);
   void setPicture(const char *icon_filename, int col, int row = All);
   void setPicture(const char *xpm[], int col, int row = All);
   void clearPicture(int col, int row = All);
   void setTextColor(QColor color, int col, int row = All);
   void setBgColor(QColor color, int col, int row = All);
   void setFont(QFont font, int col, int row = All);
   QFont getFont(int col, int row) const;
   void setType(Type type, int col, int row = All);
   Type getType(int col, int row) const;
   void setChecked(bool checked, int col, int row = All);
   bool isChecked(int col, int row) const;
   bool isCheckable(int col, int row) const;
   void setEnabled(bool enabled, int col, int row = All);
   bool isEnabled(int col, int row) const;
   void setHAlignment(bool left, bool right, int col, int row = All);
   void setVAlignment(bool top, bool bottom, int col, int row = All);
   //======================================================================
   void activateCell(int col, int row);

   // sets width and height of columns and rows
   void setHDim(int col, int width = -1);
   void setVDim(int row, int height = -1);

protected slots:
   virtual void activateEvent(int col, int row, int oldCol, int oldRow);
   virtual void changeEvent(int col, int row);
   virtual void clickEvent(int col, int row);
   virtual void dblClickEvent(int col, int row);
   
signals:
   void activated(int col, int row, int oldCol, int oldRow);
   void changed(int col, int row);
   void clicked(int col, int row);
   void dblClicked(int col, int row);
   
protected:
   QRTable_p *_private;
};

} // namespace qrs
#endif
