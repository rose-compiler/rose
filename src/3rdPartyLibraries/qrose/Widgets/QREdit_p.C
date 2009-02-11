#include <QREdit_p.h>
#include <QRMain.h>
#include <QRException.h>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

using namespace std;

namespace qrs {

QREdit_p::QREdit_p(QREdit *editor) {
   _editor = editor;
}

void QREdit_p::init(QREdit::Type type, const char *caption) {
   _type = type;   
   switch (_type) {
   case QREdit::Line:
      _layout = new QHBoxLayout;
      _widget = new QLineEdit;
      if (caption) {
         _label = new QLabel(caption); 
         _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
         _layout->addWidget(_label);
         _label->setBuddy(_widget);
      } else {
         _label = NULL;
      }   
      _layout->addWidget(_widget);
      _layout->setSpacing(10);
      _editor->setLayout(_layout);
      QROSE::link(_widget, SIGNAL(textChanged (const QString &)), (void (*) (const char *)) &changeEvent, this);
      break;
   case QREdit::Box:
      _layout = new QVBoxLayout;
      _widget = new QTextEdit;
      if (caption) {
         _label = new QLabel(caption);
         _layout->addWidget(_label);
         _label->setBuddy(_widget);
      } else {
         _label = NULL;
      }
      _layout->addWidget(_widget);
      _layout->setSpacing(3);
      _editor->setLayout(_layout);
       QROSE::link(_widget, SIGNAL(textChanged ()), (void (*) ()) &changeEvent, this);
       break;
   default:
      eAssert(0, ("invalid QREdit type!"));
   }
   
}  
 
   
void QREdit_p::setText(string text) {
   if  (_type == QREdit::Line) {
      ((QLineEdit *) _widget)->setText(text.c_str());
   } else if (_type == QREdit::Box){
      ((QTextEdit *) _widget)->setText(text.c_str());
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}

string QREdit_p::getText() const {
   if  (_type == QREdit::Line) {
      return ((QLineEdit *) _widget)->text().toAscii().constData();
   } else if (_type == QREdit::Box){
      return (const char *)  ((QTextEdit *) _widget)->toPlainText().toAscii().constData();
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }

}

void QREdit_p::clear() {
   if  (_type == QREdit::Line) {
      ((QLineEdit *) _widget)->clear();
   } else if (_type == QREdit::Box){
      ((QTextEdit *) _widget)->clear();
   } else {
      eAssert(0, ("internal error: invalid QREdit type!"));
   }
}   


void QREdit_p::changeEvent(const char *str) {
   emit QROSE::cbData<QREdit_p *>()->_editor->changeEvent(str);
}

void QREdit_p::changeEvent() {
   emit QROSE::cbData<QREdit_p *>()->_editor->changeEvent(QROSE::cbData<QREdit_p *>()->getText());
}


} // namespace qrs
