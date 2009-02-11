/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSpaceWidget     
    Description:                                                     
 ***************************************************************************/

#include <QRSpaceWidget.h>
#include <QRException.h>
#include <QRTiledWidget.h>

using namespace std;

namespace qrs {
   
QRSpaceWidget::QRSpaceWidget() {

}

void QRSpaceWidget::childIsAttached(QRTiledWidget *parent) {
   if (parent->getOrientation() == QROSE::TopDown) {
      setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
   } else if (parent->getOrientation() == QROSE::LeftRight) {
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);      
   } else {
      throw new QRException("QRSpaceWidget::childIsAttached - parent has invalid orientation!");
   }
}

} // namespace qrs
