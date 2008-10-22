/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRProgress     
    Description: 
 ***************************************************************************/

#ifndef QRPROGRESS_P_H
#define QRPROGRESS_P_H

#include <QRProgress.h>
#include <QProgressBar>
#include <QLabel>

#include <string>

namespace qrs {

class QRProgress_p {
   friend class QRProgress;
   
private:
   QRProgress_p(QRProgress *prg);
   void init(bool useLabel, bool autoHide);
   void setText(std::string text);   
   static QRProgress_p* init(QRProgress *prg, bool useLabel, bool autoHide);
   static void tickEvent(int value);

private:
   QRProgress *_prg;
   QProgressBar *_pbar;
   QLabel *_label;
   bool _autoHide;
};

} // namespace qrs

#endif
