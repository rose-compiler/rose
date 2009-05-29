/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRWidget                                                      
 ***************************************************************************/


#include <QRWidgetX.h>
#include <QRTiledWidget.h>
#include <iostream>

using namespace std;

namespace qrs {

  
QRWidget::QRWidget (QWidget * parent, Qt::WindowFlags f): QWidget(parent, f) { }

string QRWidget::getName() const {
   return QROSE::getName((QWidget *) this);
}
 
void QRWidget::childIsAttached(QRTiledWidget *parent) { }

} // namespace qrs
