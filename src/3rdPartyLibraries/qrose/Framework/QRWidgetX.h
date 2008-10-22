/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QROSE     
    Description:                                                     
 ***************************************************************************/

#ifndef QRWIDGETCLASS_H
#define QRWIDGETCLASS_H

#include <QRMacros.h>
#include <QWidget>

namespace qrs {

class QRTiledWidget;

class QRWidget: public QWidget {
   Q_OBJECT

   public:
      QRWidget ( QWidget * parent = 0, Qt::WindowFlags f = 0 );
      std::string getName() const;

   public:
      virtual void childIsAttached(QRTiledWidget *parent);
};

// setEnabled(true/false)
// hide(); show();


// 

} // namespace qrs

#endif


