#include <QRButtons_p.h>
#include <QRException.h>
#include <QObject>
#include <QAbstractButton>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>

namespace qrs {

QRButtons_p::QRButtons_p(QRButtons *pubObj) {
   _pubObj = pubObj;
}

void QRButtons_p::init() {
   QObject::connect(_pubObj, SIGNAL(clicked(int)), _pubObj, SLOT(clickEvent(int)));
   _has_radio_btns = false;
}

void QRButtons_p::addButtons(int numButtons, QRButtons::Type type) {
   eAssert(numButtons > 0, ("Number of buttons must be greater than 0!"));

   int base = _buttons.size();
   for (int i = base; i < (numButtons+base); i++) {
       QAbstractButton *button = NULL;
		 switch (type) {
			 case QRButtons::Normal: button = new QPushButton; button->setCheckable(false); break;
			 case QRButtons::Toggle: button = new QPushButton; button->setCheckable(true); break;
			 case QRButtons::Check: button = new QCheckBox; break;
          case QRButtons::Radio: button = new QRadioButton;
                      if (!_has_radio_btns) {
                         button->setChecked(true);
                         _has_radio_btns = true; }
                         break;
			 default: eAssert(0, ("Button type not supported!"));
		 }
		 QROSE::link(button, SIGNAL(clicked()), QRButtons_p::clickSignal, this);
	    *_pubObj << button;
       _buttons.push_back(button);
       _button_types.push_back(type);
    }
}

unsigned QRButtons_p::numButtons() const {
   return _buttons.size();
}

void QRButtons_p::clickSignal() {
   QRButtons *buttons = QROSE::cbData<QRButtons *>();
   int id = buttons->getId(QROSE::cbSender<QAbstractButton *>());
   eAssert(id != -1, ("internal error - this should not have happened!"));
   emit buttons->clicked(id);
}

}
