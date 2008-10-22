/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSeparator_p    
    Description:                                                     
 ***************************************************************************/

#ifndef QRSEPARATOR_P_H
#define QRSEPARATOR_P_H

#include <QRSeparator.h>
#include <QRMain.h>

namespace qrs {

class QRSeparator_p {
   friend class QRSeparator;
   
private:
   QRSeparator_p(QRSeparator *sep);

   void init(QROSE::Orientation orientation);

   static QRSeparator_p* getInstance(QRSeparator *sep);
   static QRSeparator_p* getInstance(QRSeparator *sep, QROSE::Orientation orientation);

private:
   QRSeparator *_sep;
   bool _initialised;
};

} // namespace qrs

#endif
