/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRSelect     
    Description: 
 ***************************************************************************/

#include <QRSelect.h>
#include <QRSelect_p.h>
#include <QRException.h>

#include <QComboBox>
#include <QListWidget>


using namespace std;

namespace qrs {

QRSelect::QRSelect(QRSelect::Type type, const char *caption) {
   _private = new QRSelect_p(this);
   _private->init(type, caption);
}

QRSelect::Type QRSelect::getType() const {
   return _private->_type;
}

void QRSelect::setList(const list<string> &lst, bool append) {
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      if (!append) cb->clear();
      for (std::list<string>::const_iterator iter = lst.begin(); iter != lst.end(); iter++) {
         const string &item = *iter;
         cb->addItem(item.c_str());
      }
   } else if ((_private->_type == QRSelect::Box) ||
              (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      if (!append) lv->clear();
      for (list<string>::const_iterator iter = lst.begin(); iter != lst.end(); iter++) {
         const string &item = *iter;
         QListWidgetItem *itm = new QListWidgetItem(item.c_str());
         lv->addItem(itm);
         if (_private->_type == QRSelect::Check) {
            itm->setCheckState(Qt::Unchecked);
         }
      }
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }
}

list<string> QRSelect::getList() const {
   list<string> lst;

   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      for (int i = 0; i < cb->count(); i++) {
         lst.push_back(cb->itemText(i).toAscii().constData());
      }
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      for (int i = 0; i < lv->count(); i++) {
         lst.push_back(lv->item(i)->text ().toAscii().constData());
      }
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }
      
   return lst;
}

unsigned QRSelect::count() const {
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      return cb->count();
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      return lv->count();
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }
}

void QRSelect::setText(int index, string text) {
   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));

   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      return cb->setItemText(index, text.c_str());
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      return lv->item(index)->setText(text.c_str());
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }
}

string QRSelect::getText(int index) const {
   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      return cb->itemText(index).toAscii().constData();
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      return lv->item(index)->text().toAscii().constData();
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }

}

void QRSelect::addItem(string text) {
   list<string> lst;
   lst.push_back(text);
   setList(lst, true);
}

void QRSelect::removeItem(int index) {
   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));
    
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      cb->removeItem(index);
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      delete lv->item(index);
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }   
}

void QRSelect::clear() {
   while (count()) removeItem(0);
}

void QRSelect::setPicture(int index, const char *filename) {
   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      cb->setItemIcon (index, QIcon(filename));
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      lv->item(index)->setIcon(QIcon(filename));
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }   
}

void QRSelect::setPicture(int index, const char* xpm[]) {
   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      cb->setItemIcon (index, QIcon(xpm));
   } else if ((_private->_type == QRSelect::Box) || (_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      lv->item(index)->setIcon(QIcon(xpm));
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   } 
}


void QRSelect::setSelected(int index, bool checked) {

   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));
   if (_private->_type == QRSelect::Combo) {
      eAssert(checked, ("cannot unselect combo type QRSelect!"));
      QComboBox *cb = (QComboBox *) _private->_widget;
      cb->setCurrentIndex(index);
   } else if ((_private->_type == QRSelect::Box)) {
      eAssert(checked, ("cannot unselect listbox type QRSelect!"));
      QListWidget *lv = (QListWidget *) _private->_widget;
      lv->setCurrentRow(index);
   } else if ((_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      if (checked) {
         lv->item(index)->setCheckState(Qt::Checked);
      } else {
         lv->item(index)->setCheckState(Qt::Unchecked);
      }
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }
}

list<int> QRSelect::getSelected() const {
   list<int> lst;

   for (int i = 0; i < (int) count(); i++) {
      if (isSelected(i))
         lst.push_back(i);
   }

   return lst;
}

bool QRSelect::isSelected(int index) const {

   eAssert((index >= 0) && (index < (int) count()), ("index out of bounds (%d): max elements (%d)!",
                                               index, count()));
   if (_private->_type == QRSelect::Combo) {
      QComboBox *cb = (QComboBox *) _private->_widget;
      return cb->currentIndex() == index;
   } else if ((_private->_type == QRSelect::Box)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      const QList<QListWidgetItem *>&lst = lv->selectedItems ();
      if (lst.count() == 1) {
         QListWidgetItem *item = lst.first();
         return (lv->row(item) == index);
      } else return false;
   } else if ((_private->_type == QRSelect::Check)) {
      QListWidget *lv = (QListWidget *) _private->_widget;
      return lv->item(index)->checkState() != Qt::Unchecked;
   } else {
      eAssert(0, ("invalid QRSelect type!"));
   }       
}


void QRSelect::selectEvent(int index) {
}

void QRSelect::clickEvent(int index) {
}

void QRSelect::dblClickEvent(int index) {
}

} // namespace qrs
