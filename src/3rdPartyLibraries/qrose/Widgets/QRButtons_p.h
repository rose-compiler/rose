/***************************************************************************
 *   Jose Gabriel de Figueiredo Coutinho                                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   QRose GUI Library                                                     * 
 *   Class: QRButtons_p                                                    *
 ***************************************************************************/

#ifndef QRBUTTONS_P_H
#define QRBUTTONS_P_H

#include <vector>

#include <QRButtons.h>

namespace qrs {

class QRButtons_p {
   
   friend class QRButtons;

public:
   void addButtons(int numButtons, QRButtons::Type type);
   unsigned numButtons() const;

   
protected:
   QRButtons_p(QRButtons *pubObj);
   void init();
   
   static void clickSignal();
     
protected:
   std::vector<QAbstractButton *> _buttons; 
   std::vector<QRButtons::Type> _button_types;
   bool _has_radio_btns;
   
   QRButtons *_pubObj;
};


}
#endif


