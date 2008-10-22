/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSelect     
    Description: 
 ***************************************************************************/

#ifndef QRSELECT_H
#define QRSELECT_H

#include <QWidget>
#include <list>
#include <string>

namespace qrs {

class QRSelect_p;

class QRSelect: public QWidget {
   friend class QRSelect_p;

   Q_OBJECT

public:

   typedef enum Type { Combo, Box, Check };
   
   QRSelect(Type type = Combo, const char *caption = 0);
   Type getType() const;
   
   void setList(const std::list<std::string> &lst, bool append = false);   
   std::list<std::string> getList() const;
   void addItem(std::string text);
   void removeItem(int index);
   void clear();
   unsigned count() const;
   
   void setText(int index, std::string text);
   std::string getText(int index) const;
   void setPicture(int index, const char *filename);
   void setPicture(int index, const char* xpm[]);

   void setSelected(int index, bool checked = true);
   std::list<int> getSelected() const;
   bool isSelected(int index) const;
   
public slots:
   virtual void selectEvent(int index);
   virtual void clickEvent(int index);
   virtual void dblClickEvent(int index);

signals:
   void selected(int index);
   void clicked(int index);
   void dblClicked(int index);

protected:
   QRSelect_p *_private;
};

} // namespace qrs

#endif
