
#include <QRButtons.h>
#include <QRException.h>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>

using namespace std;
namespace qrs {

class QRButtons_p {
public:
	static void clickSignal() {
		QRButtons *buttons = QROSE::cbData<QRButtons *>();
		int id = buttons->getId(QROSE::cbSender<QAbstractButton *>());
		eAssert(id != -1, ("internal error - this should not have happened!"));
		emit buttons->clicked(id);
	}		
};

QRButtons::QRButtons(std::string caption): QRPanel(caption) {
	QObject::connect(this, SIGNAL(clicked(int)), SLOT(clickEvent(int)));
   _has_radio_btns = false;
}

QRButtons::QRButtons(QROSE::Orientation orientation, std::string caption): QRPanel(orientation, caption) {
	QObject::connect(this, SIGNAL(clicked(int)), SLOT(clickEvent(int)));
   _has_radio_btns = false; 
}

void QRButtons::addButtons(int numButtons, Type type) {
	eAssert(numButtons > 0, ("Number of buttons must be greater than 0!"));
	
	int base = _buttons.size();
   for (int i = base; i < (numButtons+base); i++) {
       QAbstractButton *button = NULL;
		 switch (type) {
			 case Normal: button = new QPushButton; button->setCheckable(false); break;
			 case Toggle: button = new QPushButton; button->setCheckable(true); break;
			 case Check: button = new QCheckBox; break;
          case Radio: button = new QRadioButton; if (!_has_radio_btns) {button->setChecked(true); _has_radio_btns = true; } break;
			 default: eAssert(0, ("Button type not supported!"));
		 }
		 QROSE::link(button, SIGNAL(clicked()), QRButtons_p::clickSignal, this);
	    *this << button;		 			 		 
       _buttons.push_back(button);
		 _button_types.push_back(type);
    }
}


unsigned QRButtons::numButtons() const {
	return _buttons.size();
}


QAbstractButton* QRButtons::operator [](int id) const {
  eAssert(id < (int) _buttons.size(), ("cannot get button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));
   
  return _buttons[id];
}

int QRButtons::getId(QAbstractButton *button) {
  int id = -1;
  bool found = false;
  unsigned i = 0;
  while (!found && i < _buttons.size()) {
    if (_buttons[i] == button) {
      found = true;
      id = i;
    } else {
      i++;
    }
      
  }
   
  return id;
}

QRButtons::Type QRButtons::getType(int id) const {
  eAssert(id < (int) _buttons.size(), ("cannot get type for button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));
   
  return _button_types[id];
}

void QRButtons::setCaption(const char *caption0, ...) {
	va_list ap;
	va_start(ap, caption0); 
	 
	const char *caption = caption0;
	
	for (unsigned i = 0; i < _buttons.size(); i++) {
		setCaption(i, caption);
		caption = va_arg(ap, const char *);
	}
	
	va_end(ap);
	
}

void QRButtons::setCaption(int id, const char *fmt_caption, ...) {
	eAssert(id < (int) _buttons.size(), ("cannot set caption to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));
	char buffer[1024];
	
	if (fmt_caption) {
		va_list ap;
		va_start(ap, fmt_caption);  
		vsprintf(buffer, fmt_caption, ap);
		_buttons[id]->setText(buffer); 
		va_end(ap); 
	}	
}
	
void QRButtons::setPicture(int id, const char *filename) {
  eAssert(id < (int) _buttons.size(), ("cannot set picture to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));
  
  FILE *f = fopen(filename, "r");
  eAssert(f, ("cannot open file [%s] to set button picture!", filename));
  fclose(f);
  
  QIcon icon(filename);
  _buttons[id]->setIcon(icon);
}

void QRButtons::setPicture(int id, const char *xpm[]) {
  eAssert(id < (int) _buttons.size(), ("cannot set picture to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));

  QPixmap pixmap(xpm);
  QIcon icon(pixmap);
  _buttons[id]->setIcon(icon);
}


void QRButtons::setBtnChecked(int id, bool check) {
  eAssert(id < (int) _buttons.size(), ("cannot set checked state to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));

  QAbstractButton *b = _buttons[id];

  eAssert(b->isCheckable(), ("cannot set checked state to button %s[%d], as it is not-checkable type!", QROSE::getName(this).c_str(), id, _buttons.size()));
  b->setChecked(check);
}

bool QRButtons::isBtnChecked(int id) const {
  eAssert(id < (int) _buttons.size(), ("invalid reference to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));

  QAbstractButton *b = _buttons[id];
  return b->isChecked();

}

bool QRButtons::isBtnCheckable(int id) const {
  eAssert(id < (int) _buttons.size(), ("invalid reference to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));

  QAbstractButton *b = _buttons[id];
  return b->isCheckable();
}

void QRButtons::setBtnEnabled(int id, bool enable) {
  eAssert(id < (int) _buttons.size(), ("invalid reference to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));
  
  QAbstractButton *b = _buttons[id];
  b->setEnabled(enable);
}


bool QRButtons::isBtnEnabled(int id) const {
  eAssert(id < (int) _buttons.size(), ("invalid reference to button %s[%d], as it only contains %d button(s)!", QROSE::getName(this).c_str(), id, _buttons.size()));

  QAbstractButton *b = _buttons[id];
  return b->isEnabled();
}


void QRButtons::clickEvent(int id) {

}

}
