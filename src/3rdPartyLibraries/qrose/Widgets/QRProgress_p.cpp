#include <QRProgress_p.h>
#include <QRMain.h>
#include <QRException.h>
#include <QVBoxLayout>

using namespace std;

namespace qrs {

QRProgress_p::QRProgress_p(QRProgress *prg) {
   _prg = prg;
   QObject::connect(_prg, SIGNAL(ticked(int)), _prg, SLOT(tickEvent(int)));
   QObject::connect(_prg, SIGNAL(started()), _prg, SLOT(startEvent()));
   QObject::connect(_prg, SIGNAL(ended()), _prg, SLOT(endEvent()));
}

void QRProgress_p::init(bool useLabel, bool autoHide) {

   QVBoxLayout *layout = new QVBoxLayout;

   _pbar = new QProgressBar;   
   _pbar->setMinimum(0);
   _pbar->setMaximum(0);
   _pbar->setTextVisible (true);
   layout->addWidget(_pbar);
   if (useLabel) {
      _label = new QLabel;
      layout->addWidget(_label);
   } else {
      _label = NULL;
   }

   _prg->setLayout(layout);
   _autoHide = autoHide;
   
   if (_autoHide)
      _prg->hide();

   QROSE::link(_pbar, SIGNAL(valueChanged (int)), &QRProgress_p::tickEvent, this);
}

void QRProgress_p::setText(string text) {
   eAssert(_label, ("No label associated to QRProgress in constructor!"));
   _label->setText(text.c_str());
}


QRProgress_p* QRProgress_p::init(QRProgress *prg, bool useLabel, bool autoHide) {
   QRProgress_p *p = new QRProgress_p(prg);
   p->init(useLabel, autoHide);
   return p;
}

void QRProgress_p::tickEvent(int value) {
   QRProgress_p *p = QROSE::cbData<QRProgress_p *>();
   if (value == 0)
      emit p->_prg->started();
   else if (value == p->_pbar->maximum())
      emit p->_prg->ended();
   else 
      emit p->_prg->ticked(value);
}

} // namespace qrs
