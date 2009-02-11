#include <QREdit.h>
#include <QREdit_p.h>
#include <QRException.h>

#include <QLineEdit>
#include <QTextEdit>

using namespace std;

namespace qrs {

QREdit::QREdit(QREdit::Type type, const char *caption) {
   _private = new QREdit_p(this);
   _private->init(type, caption);
}

void QREdit::setText(string text) {
   _private->setText(text);
}

string QREdit::getText() const {
   return _private->getText();
}

void QREdit::clear() {
   _private->clear();
}

void QREdit::setReadOnly(bool readOnly) {
   if  (_private->_type == Line) {
      ((QLineEdit *) _private->_widget)->setReadOnly(readOnly);
   } else if (_private->_type == Box){
      ((QTextEdit *) _private->_widget)->setReadOnly(readOnly);
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }   
}

bool QREdit::isReadOnly() const {
   if  (_private->_type == Line) {
      return ((QLineEdit *) _private->_widget)->isReadOnly();
   } else if (_private->_type == Box){
      return ((QTextEdit *) _private->_widget)->isReadOnly();
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}

void QREdit::setFont(QFont font) {
   if  (_private->_type == Line) {
      ((QLineEdit *) _private->_widget)->setFont(font);
   } else if (_private->_type == Box){
      ((QTextEdit *) _private->_widget)->setFont(font);
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}

QFont QREdit::getFont() const {
   if  (_private->_type == Line) {
      return ((QLineEdit *) _private->_widget)->font();
   } else if (_private->_type == Box){
      return ((QTextEdit *) _private->_widget)->font();
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}


void QREdit::setTextColor(QColor color) {
   if  ((_private->_type == Line) || (_private->_type == Box)) {
      QPalette pal = _private->_widget->palette();
      pal.setColor(QPalette::Text, color);
      _private->_widget->setPalette(pal);
      return _private->_widget->setPalette(pal);
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}

QColor QREdit::getTextColor() const {
   QPalette pal = _private->_widget->palette();
   return pal.color(QPalette::Text);
}


void QREdit::setBgColor(QColor color) {
   if  ((_private->_type == Line) || (_private->_type == Box)) {
      QPalette pal = _private->_widget->palette();
      pal.setColor(QPalette::Base, color);
      _private->_widget->setPalette(pal);
      return _private->_widget->setPalette(pal);
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}

QColor QREdit::getBgColor() const {
   QPalette pal = _private->_widget->palette();
   return pal.color(QPalette::Base);
}

void QREdit::changeEvent(string text) {

}


} // namespace qrs
