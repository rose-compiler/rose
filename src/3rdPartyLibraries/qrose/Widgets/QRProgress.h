/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRProgress     
    Description: 
 ***************************************************************************/

#ifndef QRPROGRESS_H
#define QRPROGRESS_H

#include <QWidget>
#include <string>

namespace qrs {

class QRProgress_p;
   
class QRProgress: public QWidget {

   Q_OBJECT

   friend class QRProgress_p;
   
public:
   QRProgress(bool useLabel = true, bool autoHide = false);
   void set(int totalSteps);
   void set(int currentStep, int totalSteps);
   
   void tick(int steps = -1);
   void tick(std::string txt, int steps = -1);
   int value() const;
   int maximum() const;
   void reset();

public slots:
   virtual void startEvent();
   virtual void tickEvent(int value);
   virtual void endEvent();
   
signals:
   void started();
   void ticked(int value);
   void ended();
   
protected:
   QRProgress_p *_private;

};

} //namespace qrs

#endif
