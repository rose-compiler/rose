/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSpaceWidget     
    Description:                                                     
 ***************************************************************************/

#ifndef QRSPACEWIDGETCLASS_H
#define QRSPACEWIDGETCLASS_H

#include <QRMacros.h>
#include <QWidget>

namespace qrs {

class QRTiledWidget;

// this is a greedy invisible widget that occupies the most space
// as possible. Great for aligning widgets on the left, right and center.
class QRSpaceWidget: public QWidget { 
   Q_OBJECT
   public:
       QRSpaceWidget();

   protected slots:
       virtual void childIsAttached(QRTiledWidget *parent);
};

}// namespace qrs;

#endif
