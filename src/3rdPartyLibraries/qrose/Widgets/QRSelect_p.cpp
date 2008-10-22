#include <QRSelect_p.h>
#include <QRException.h>
#include <QRMain.h>

#include <QComboBox>
#include <QListWidget>

using namespace std;

namespace qrs {

QRSelect_p::QRSelect_p(QRSelect *sel) {
   _sel = sel;
   QObject::connect(_sel, SIGNAL(selected(int)), _sel, SLOT(selectEvent(int)));
}

void QRSelect_p::init(QRSelect::Type type, const char *caption) {
   _type = type;

   if (_type == QRSelect::Combo) { // combo-box
      _layout = new QHBoxLayout;
      _widget = new QComboBox;
      QROSE::link(_widget, SIGNAL(currentIndexChanged(int)), (void (*) (int)) &QRSelect_p::selectEvent, _sel);

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
      _sel->setLayout(_layout);
   } else if ((_type == QRSelect::Box) || (_type == QRSelect::Check)) {
      _layout = new QVBoxLayout;
      _widget = new QListWidget;
      if (_type == QRSelect::Box) 
         QROSE::link(_widget, SIGNAL(itemSelectionChanged()), (void (*) ()) &QRSelect_p::selectEvent, _sel);
      QObject::connect(_widget, SIGNAL(itemClicked (QListWidgetItem *)),
                       this, SLOT(clickEvent(QListWidgetItem*)));
      QObject::connect(_widget, SIGNAL(itemDoubleClicked (QListWidgetItem *)),
                       this, SLOT(dblClickEvent(QListWidgetItem*)));
      QObject::connect(_sel, SIGNAL(clicked(int)), _sel, SLOT(clickEvent(int)));
      QObject::connect(_sel, SIGNAL(dblClicked(int)), _sel, SLOT(dblClickEvent(int)));

      QROSE::link(_widget, SIGNAL(itemSelectionChanged()), (void (*) ()) &QRSelect_p::selectEvent, _sel);

      if (caption) {
         _label = new QLabel(caption);
         _layout->addWidget(_label);
         _label->setBuddy(_widget);
      } else {
         _label = NULL;
      }
      _layout->addWidget(_widget);
      _layout->setSpacing(3);
      _sel->setLayout(_layout);
      if (_type == QRSelect::Box) {
         ((QListWidget *) _widget)->setSelectionMode(QAbstractItemView::SingleSelection);
      } else if (_type == QRSelect::Check) {
         ((QListWidget *) _widget)->setSelectionMode(QAbstractItemView::NoSelection);
      }  
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }
}

void QRSelect_p::selectEvent() {
   if (!QROSE::isExec()) return;
   QRSelect *sel = QROSE::cbData<QRSelect *>();
   const list<int> &lst = sel->getSelected();
   if (lst.size() == 1) {
      int idx = lst.front();
      emit sel->selected(idx);
   }
}

void QRSelect_p::selectEvent(int idx) {
   if (!QROSE::isExec()) return;
   QRSelect *sel = QROSE::cbData<QRSelect *>();
   emit sel->selected(idx);
}

void QRSelect_p::clickEvent(QListWidgetItem *item) {
   if (!QROSE::isExec()) return;
   emit _sel->clicked(((QListWidget *)_widget)->row(item));
}

void QRSelect_p::dblClickEvent(QListWidgetItem *item) {
   if (!QROSE::isExec()) return;
   emit _sel->dblClicked(((QListWidget *)_widget)->row(item));
}


} // namespace qrs
