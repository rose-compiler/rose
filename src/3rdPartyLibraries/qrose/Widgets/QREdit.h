/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QREdit     
    Description: 
 ***************************************************************************/

#ifndef QREDIT_H
#define QREDIT_H

#include <QRMacros.h>
#include <QWidget>

#include <list>
#include <string>

namespace qrs {

class QREdit_p;

class QREdit: public QWidget {
   friend class QREdit_p;

   Q_OBJECT

public:
   
   typedef enum Type { Line, Box };

   QREdit(Type type = Line, const char *caption = 0);

   void setText(std::string text);
   std::string getText() const;   
   
   void clear();

   void setReadOnly(bool readOnly);
   bool isReadOnly() const;
   void setFont(QFont font);
   QFont getFont() const;
   void setTextColor(QColor color);
   QColor getTextColor() const;   
   void setBgColor(QColor color);
   QColor getBgColor() const;

   
protected slots:
   virtual void changeEvent(std::string text);

signals:
   void changed(std::string text);

protected:
   QREdit_p *_private;
};

} // namespace qrs
#endif
