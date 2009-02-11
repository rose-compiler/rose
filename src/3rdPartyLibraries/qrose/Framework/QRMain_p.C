/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Classes:     
    Description:                                                     
 ***************************************************************************/

#include <QRMain.h>
#include <QRMain_p.h>
#include <QRWindow.h>
#include <QRException.h>

#include <QApplication>
#include <QMetaMethod>

using namespace std;

namespace qrs {

QRExec::QRExec() {
   m_isExec = false;
}
   
bool QRExec::isExec() {
   return m_isExec;
}

void QRExec::beforeExec() {
   m_isExec = true;
}
   
void QRExec::afterExec() {
   m_isExec = false;
}
      
QRCBSession::QRCBSession() {
   m_valid = false;
}

void QRCBSession::setSession(QObject *sender, void *data) {
   m_valid = (sender != NULL);
   m_data = data;
   m_sender = sender;
}

QObject *QRCBSession::getCBSender() {
   return m_sender;
}

void* QRCBSession::getCBData() {
   return m_data;
}

bool QRCBSession::isValid() {
   return m_valid;
}

QRCallbackMgr::QRCallbackMgr() {
}

QRCallback::QRCallback(QObject *obj, CBNormalizedType cbf, void *data) { 
   m_sender = obj; 
   m_cbf = cbf;
   m_data = data;
}

void QRCallback::prepareDispatcher() {
   QROSE_P::getCBSession()->setSession(m_sender, m_data);
}

void QRCallback::endDispatcher() {
   QROSE_P::getCBSession()->setSession(0, 0);
}

QRCallbackMgr::~QRCallbackMgr() {
   map<QObject*, map<int, QRCallback *> >::iterator iter0;
   for (iter0 = m_cbs.begin(); iter0 != m_cbs.end(); iter0++) {
      map<int, QRCallback *> &map_c = iter0->second;
      map<int, QRCallback *>::iterator iter1;
      for (iter1 = map_c.begin(); iter1 != map_c.end(); iter1++) {
         QRCallback *cb = iter1->second;
         delete cb;
      }
   } 
}

int QRCallbackMgr::disconnect(QObject *obj, const char *signal) {
   const QMetaObject *mtobj = obj->metaObject();

   if (!mtobj) {
      throw QRException("callback error: no metaobject information! [%s]", signal);
   }

	const char *sgn;
	if (signal[0] >= '0' && signal[0] <= '9') 
		sgn =  &(signal[1]);
	else
		sgn = signal;
	
   string normalized_signal = QMetaObject::normalizedSignature(sgn).data();
   
   int index_signal = mtobj->indexOfSignal(normalized_signal.c_str());
   if (index_signal == -1) {
      throw QRException("callback error: invalid signal %s", signal);
   }

   map<QObject*, map<int, QRCallback *> >::iterator iter0 = m_cbs.find(obj);
   if (iter0 != m_cbs.end()) {
      map<int, QRCallback *> &map_c = iter0->second;
      map<int, QRCallback *>::iterator iter1 = map_c.find(index_signal);
      if (iter1 != map_c.end()) {
         QRCallback *cb = iter1->second;
         delete cb;
         map_c.erase(iter1);
      }
   }

   return index_signal;
}

void QRCallbackMgr::connect(QObject *obj, const char *signal, int id, QRCallback *cb) {
   // registering cb
   map<int, QRCallback *> &map_c = m_cbs[obj];
   map_c[id] = cb;
   
   // preparing signal
	const char *sgn;
	if (signal[0] >= '0' && signal[0] <= '9') 
		sgn =  &(signal[1]);
	else
		sgn = signal;
		
   string normalized_signal = QMetaObject::normalizedSignature(sgn).data();
   string sg = string(SIGNAL()) + normalized_signal;

   // preparing slot
   string slt;
   const QMetaObject *mtdobj = cb->metaObject();
   int i = 0; bool found_slot = false;
   while (!found_slot && (i < mtdobj->methodCount())) {
      QMetaMethod method = mtdobj->method(i);
      // check if method is a slot
      if (method.methodType() == QMetaMethod::Slot) {
         const string target_slot = "dispatcher";
         string slot_signature = method.signature();
         if (target_slot.size() < slot_signature.size()) {
            char c = slot_signature[target_slot.size()];
            if (c == '(') {
               slot_signature[target_slot.size()] = 0;
               string trunc_signature = slot_signature.c_str();
               slot_signature[target_slot.size()] = c;
               if (trunc_signature == target_slot) {
                  if (QMetaObject::checkConnectArgs(normalized_signal.c_str(), slot_signature.c_str())) {
                     found_slot = true;
                     slt = string(SLOT()) + slot_signature;
                  }
               }
            }
         }
      }
      if (!found_slot) i++;
   }
   if (!found_slot) {
      throw QRException("connect callback error: cannot find a compatible dispatcher slot for signal %s", sgn);
   }
       
   QObject::connect(obj, sg.c_str(), cb, slt.c_str());
}

QRWindowsMap::QRWindowsMap() {
}

QRWindowsMap::~QRWindowsMap() {
   /* BUG:

         QROSE::init(argc, argv);
         QRWindow *mainWindow = new QRWindow("mainWindow",  QROSE::TopDown);
         (*mainWindow) << new QTableView;
   
   map<string, QRWindow *>::iterator iter = m_windows.begin();
   while (iter != m_windows.end()) {
      delete iter->second;
      iter = m_windows.begin();
   } */
}

void QRWindowsMap::registerWindow(const string &name, QRWindow *win) {
   if (name == "")
      throw QRException("registering window - window name cannot be an empty string!");

   if (m_windows.find(name) != m_windows.end())
      throw QRException("registering window - window [%s] already exists!", name.c_str());
   m_windows[name] = win;
}

void QRWindowsMap::unregisterWindow(const string &name) {  
   map<string, QRWindow *>::iterator iter = m_windows.find(name);
   if (iter == m_windows.end())
      throw QRException("unregistering window - window [%s] does not exist!", name.c_str());

   m_windows.erase(iter);
}

QRWindow* QRWindowsMap::findWindow(const string &name) {
   map<string, QRWindow *>::iterator iter = m_windows.find(name);
   QRWindow *window = NULL;
   if (iter != m_windows.end()) {
      window = iter->second;
   }

   return window;
}

void QRWindowsMap::preExec() {
   map<string, QRWindow *>::iterator iter = m_windows.begin();
   while (iter != m_windows.end()) {
      QRWindow *win = iter->second;
      if (win->startsAsHidden()) {
         win->hide();
      } else {
         win->show();
         if (win->isFixedSize()) {
            QROSE::setWidth(win, win->width());
            QROSE::setHeight(win, win->height());
         }
      }
      iter++;
   }
}

QRExec QROSE_P::m_exec;
QRWindowsMap QROSE_P::m_windows;
QRCallbackMgr QROSE_P::m_callbackMgr;
QRCBSession QROSE_P::m_cbSession;

} // namespace qrs
