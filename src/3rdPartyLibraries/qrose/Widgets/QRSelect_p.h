/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSelect_p    
    Description: 
 ***************************************************************************/

#ifndef QRSELECT_P_H
#define QRSELECT_P_H

#include <QRSelect.h>
#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidgetItem>

namespace qrs {

class QRSelect_p: QObject {
   friend class QRSelect;
   Q_OBJECT

private:   
   QRSelect_p(QRSelect *sel);
   void init(QRSelect::Type type, const char *caption);

   static void selectEvent();
   static void selectEvent(int);

public slots:
   virtual void clickEvent(QListWidgetItem *item);
   virtual void dblClickEvent(QListWidgetItem *item);

private:
   QRSelect::Type _type;
   QRSelect *_sel;
   QBoxLayout *_layout;
   QWidget *_widget;
   QLabel *_label;
};

} // namespace qrs

#endif
