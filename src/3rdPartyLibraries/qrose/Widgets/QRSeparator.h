/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSeparator     
    Description:                                                     
 ***************************************************************************/

#ifndef QRSEPARATOR_H
#define QRSEPARATOR_H

#include <QRMain.h>
#include <QFrame>

namespace qrs {

class QRSeparator_p;
class QRTiledWidget;

class QRSeparator: public QFrame {
   Q_OBJECT
      
   friend class QRSeparator_p;
   
public:
   QRSeparator();
   QRSeparator(QROSE::Orientation orientation);

protected slots:
   virtual void childIsAttached(QRTiledWidget *parent);
   
private:
   QRSeparator_p *_private;   
};   

   
} // namespace qrs
#endif
