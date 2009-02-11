/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho
    jgfc@doc.ic.ac.uk
    Class: QRToolBar
    Description:
 ***************************************************************************/

#include <QRToolBar.h>
#include <QRException.h>
#include <QAction>

using namespace std;

namespace qrs {

   
class QRToolBar_p {
   friend class QRToolBar;
   private:
      static void clickSignal() {
         QRToolBar *tb = QROSE::cbData<QRToolBar *>();
         int id = tb->getId(QROSE::cbSender<QAction *>());
         eAssert(id != -1, ("internal error - this should not have happened!"));
         emit tb->clicked(id);
      }

      int addButton(QRToolBar *tb) {
         int id = (int) numButtons();
         QAction *action = tb->addAction("");
         QROSE::link(action, SIGNAL(triggered ()), &QRToolBar_p::clickSignal, tb);
         _actions.push_back(action);
         return id;
      }

      QAction* getAction(int id) const {
         eAssert(id < (int) _actions.size(), ("index (%d) out of bounds (0-%d) in QRToolBar::getAction", id, _actions.size()));

         return _actions[id];
      }

      int getId(QAction *action) const {
         bool found = false;
         int id = -1;
         unsigned i = 0; unsigned n = _actions.size();
         while (!found && (i < n)) {
            if (_actions[i] == action) {
               found = true;
               id = i;
            } else {
               i++;
            }
         }
         return id;
      }

      unsigned numButtons() const {
         return _actions.size();
      }
     
   protected:
      std::vector<QAction *> _actions;
};

QRToolBar::QRToolBar(QROSE::Orientation orientation, bool showText,
                     bool showPic, bool picBesidesText)
{
  if (orientation == QROSE::TopDown)
    setOrientation(Qt::Vertical);
  else if (orientation == QROSE::LeftRight)
    setOrientation(Qt::Horizontal);
  else
    eAssert(0, (("invalid orientation specified in QRToolBar constructor!")));

  if (!showText && !showPic) {
    eAssert(0, (("must either show text or pic in QRToolBar constructor!")));
  } else if (showText && !showPic) {
    setToolButtonStyle(Qt::ToolButtonTextOnly);
  } else if (!showText && showPic) {
    setToolButtonStyle(Qt::ToolButtonIconOnly);
  } else if (picBesidesText) {
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  } else { /* !picBesidesText */
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  }
  _private = new QRToolBar_p;
  QObject::connect(this, SIGNAL(clicked(int)), SLOT(clickEvent(int)));
}

QRToolBar::~QRToolBar() {
   delete _private;
}

int QRToolBar::addButton(string caption, string icon_filename) {
  int id = _private->addButton(this);
  if (caption != "") {
     setCaption(id, caption);
  }
  if (icon_filename != "") {
    setPicture(id, icon_filename);
  }

  return id;
}
  
int QRToolBar::addButton(string caption, const char *icon_xpm[]) {
  int id = _private->addButton(this);

  if (caption != "") {
    setCaption(id, caption);
  }
  if (icon_xpm) {
    setPicture(id, icon_xpm);
  }

  return id;
}
    
int QRToolBar::addToggleButton(string caption, string icon_filename) {
  int id = addButton(caption, icon_filename);
  QAction *action = getAction(id);
  action->setCheckable(true);
  return id;
}

int QRToolBar::addToggleButton(std::string caption, const char *icon_xpm[]) {
   int id = addButton(caption, icon_xpm);
   QAction *action = getAction(id);
   action->setCheckable(true);
   return id;
}  

void QRToolBar::insertSeparator() {
   addSeparator();
}


void QRToolBar::setCaption(int id, string caption) {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::setCaption!",
                               id, numButtons()-1)); 
   QAction *action = getAction(id);
   action->setText(caption.c_str());
}

void QRToolBar::setPicture(int id, string filename) {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::setPicture!",
           id, numButtons()-1)); 
   QAction *action = getAction(id);
   QIcon icon(filename.c_str());
   action->setIcon(icon);
}

void QRToolBar::setPicture(int id, const char *xpm[]) {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::setPicture!",
           id, numButtons()-1));   
   QAction *action = getAction(id);
   QIcon icon(xpm);
   action->setIcon(icon);
}

void QRToolBar::setEnabled(int id, bool enable) {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::setEnabled!",
           id, numButtons()-1));
   QAction *action = getAction(id);
   action->setEnabled(enable);
}
   
bool QRToolBar::isEnabled(int id) const {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::isEnabled!",
           id, numButtons()-1)); 
   QAction *action = getAction(id);
   return action->isEnabled();
}

void QRToolBar::setChecked(int id, bool checked) {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::setChecked!",
           id, numButtons()-1)); 
   QAction *action = getAction(id);
   eAssert(action->isCheckable(), ("cannot set checked state to button %s[%d], as it is not-checkable type!", QROSE::getName(this).c_str(), id));
   action->setChecked(checked);
}

bool QRToolBar::isChecked(int id) const {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::isEnabled!",
           id, numButtons()-1));
   QAction *action = getAction(id);
   eAssert(action->isCheckable(), ("cannot verify checked state to button %s[%d], as it is not-checkable type!", QROSE::getName(this).c_str(), id));

   return action->isChecked();
}

bool QRToolBar::isCheckable(int id) const {
   eAssert(id < (int) numButtons(), ("index (%d) out of bounds (0..%d) for QRToolBar::isCheckable!",
           id, numButtons()-1)); 
   QAction *action = getAction(id);
   return action->isCheckable();
}

unsigned QRToolBar::numButtons() const {
   return _private->numButtons();
}

QAction* QRToolBar::getAction(int id) const {
   return _private->getAction(id);
}  

int QRToolBar::getId(QAction *action) const {
   return _private->getId(action);
}
 
void QRToolBar::clickEvent(int id) {
   
}

} /* namespace qrs */
