#include <QRSeparator.h>
#include <QRSeparator_p.h>
#include <QRTiledWidget.h>
#include <QRException.h>

namespace qrs {

QRSeparator::QRSeparator() {
   _private = QRSeparator_p::getInstance(this);
}

QRSeparator::QRSeparator(QROSE::Orientation orientation) {
   _private = QRSeparator_p::getInstance(this, orientation);
}


void QRSeparator::childIsAttached(QRTiledWidget *parent) {
   eAssert(_private, ("internal error!"));
   _private->init(parent->getOrientation());
}
   

} // namespace qrs
