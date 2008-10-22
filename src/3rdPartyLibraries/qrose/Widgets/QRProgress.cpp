#include <QRProgress.h>
#include <QRProgress_p.h>
#include <QRMain.h>
#include <QRException.h>
#
using namespace std;

namespace qrs {

QRProgress::QRProgress(bool useLabel, bool autoHide) {
   _private = QRProgress_p::init(this, useLabel, autoHide);
}   


void QRProgress::set(int steps) {
   eAssert(steps > 0, ("number of steps must be greater than 0!"));
   _private->_pbar->setMaximum(steps);
   QROSE::processEvents();
}

void QRProgress::set(int currentStep, int totalSteps) {
   eAssert(currentStep <= totalSteps, ("currentStep (%d) must be lower than totalSteps (%d)",
                                       currentStep, totalSteps));
   _private->_pbar->setValue(currentStep);
   set(totalSteps);
}   


void QRProgress::tick(int steps) {
   int v = _private->_pbar->value();
   if (_private->_autoHide) {
      if (v == -1) {
         show();
       } else {
         if (v == _private->_pbar->maximum()) {
            reset();
            return;
         }
      }
   }
   int s;
   if (steps == -1) s = 1; else s = steps;
   int n = _private->_pbar->value() + s;
   _private->_pbar->setValue(n);
   QROSE::processEvents();
}

void QRProgress::tick(string txt, int steps) {
   tick(steps);
   _private->setText(txt);
}

int QRProgress::value() const {
   return _private->_pbar->value();
}

int QRProgress::maximum() const {
   return _private->_pbar->maximum();
}

void QRProgress::reset() {
   _private->_pbar->reset();
   if (_private->_autoHide) {
      hide();
   }
}

void QRProgress::tickEvent(int value) {
}

void QRProgress::startEvent() {
}

void QRProgress::endEvent() {

}

} // namespace qrs
