#include <QRSeparator_p.h>
#include <QRException.h>

namespace qrs {

QRSeparator_p::QRSeparator_p(QRSeparator *sep) {
   _sep = sep;
   _initialised = false;
}

void QRSeparator_p::init(QROSE::Orientation orientation) {
   if (_initialised) return;
   if (orientation == QROSE::TopDown) {
      _sep->setFrameShape(QFrame::HLine);
   } else if (orientation == QROSE::LeftRight) {
      _sep->setFrameShape(QFrame::VLine);
   } else {
      eAssert(0, ("wrong orientation!"));
   }
   _initialised = true;
}

QRSeparator_p* QRSeparator_p::getInstance(QRSeparator *sep) {
   QRSeparator_p *s = new QRSeparator_p(sep);
   return s;
}

QRSeparator_p* QRSeparator_p::getInstance(QRSeparator *sep, QROSE::Orientation orientation) {
   QRSeparator_p *s = new QRSeparator_p(sep);
   s->init(orientation);
   return s;
}


} // namespace qrs
