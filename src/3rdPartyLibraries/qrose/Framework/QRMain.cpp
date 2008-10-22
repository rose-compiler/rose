/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QROSE                                                      
 ***************************************************************************/


#include <QRMain.h>
#include <QRMain_p.h>

#include <QRException.h>
#include <QRWindow.h>

#include <QObject>
#include <QApplication>
#include <QMessageBox>
#include <QAbstractEventDispatcher>
 #include <QMetaMethod>

using namespace std;

namespace qrs {

void QROSE::init(int &argc, char **argv) {
   new QApplication(argc, argv);
}

void QROSE::exit(int returnCode) {
   eAssert(app() && isExec(), ("You must call QROSE::init, and be inside the main event loop!"));
   app()->exit(returnCode);
}


QApplication* QROSE::app() { 
   return  (QApplication*) QApplication::instance(); 
}

int QROSE::exec() {
   if (isExec())
      throw QRException("QROSE::exec(): already inside the main loop!");
   QROSE_P::getExec()->beforeExec();
   QROSE_P::getWindowsMap()->preExec();
   int ret = app()->exec();
   QROSE_P::getExec()->afterExec();
   return ret;
}

bool QROSE::isExec() {
   return QROSE_P::getExec()->isExec();
}

void QROSE::processEvents() {
   QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance();
   dispatcher->processEvents(QEventLoop::AllEvents);
}

QRWindow* QROSE::getWindow(const std::string &name) {
   QRWindow *window = findWindow(name);
   if (!window)
      throw QRException("window [%s] does not exist!", name.c_str());
   return window;
}
    
QRWindow* QROSE::findWindow(const std::string &name) {
   return QROSE_P::getWindowsMap()->findWindow(name);
}


void QROSE::setWidth(QWidget *widget, int size) {
   setWidth(widget, size, size);
}

void QROSE::setWidth(QWidget *widget, int min, int max, int stretchFactor) {
   if (!widget)
      throw QRException("QROSE::setWidth - invalid widget (null)!");
     
   if ((min > 0 && max > 0) && (min > max)) {      
      throw QRException("QROSE::setWidth - min width cannot be larger than max!");
   } else {     
      if (min == Unlimited) {
         throw QRException("QROSE::setWidth - min width cannot be unlimited - minimum is 1");
      } else if (min != Ignore) {
         if (min <= 0)  {
            throw QRException("QROSE::setWidth - invalid min width (%d) - minimum is 1", min);
         } else {
            widget->setMinimumWidth(min);
         }
      }
      
      if (max == Unlimited) {
         QSizePolicy policy_in, policy_out;
         policy_in = widget->sizePolicy();
         policy_out = policy_in;
         policy_out.setHorizontalPolicy(QSizePolicy::Expanding);
         widget->setSizePolicy(policy_out);    
      } else if (max != Ignore) {
         if (max <= 0)  {
            throw QRException("QROSE::setWidth - invalid max width (%d)", min);
         } else {
            widget->setMaximumWidth(max);
         }
      }      
   }  
          
   if (stretchFactor != Ignore) {
      QSizePolicy policy_in, policy_out;
      policy_in = widget->sizePolicy();
      policy_out = policy_in;
      policy_out.setHorizontalStretch(stretchFactor);
      widget->setSizePolicy(policy_out);
   }
      
}
          
void QROSE::setHeight(QWidget *widget, int size) {
   setHeight(widget, size, size);
}

QRWindow* QROSE::getQRWindow(QWidget *widget) {
   
   while (widget && !widget->inherits("qrs::QRWindow")) {
      QObject *parent = widget->parent();
      if (!parent || !parent->isWidgetType())  
         throw QRException("QROSE::getWindowFromWidget - top-level parent is not a QRWindow!");
      else  
         widget = (QWidget *) parent;
   }
   return (QRWindow *) widget;
   
   
}

void QROSE::setHeight(QWidget *widget, int min, int max, int stretchFactor) {
   if (!widget)
      throw QRException("QROSE::setHeight - invalid widget (null)!");
     
   if ((min > 0 && max > 0) && (min > max)) {      
      throw QRException("QROSE::setHeight - min height cannot be larger than max!");
   } else {     
      if (min == Unlimited) {
         throw QRException("QROSE::setHeight - min height cannot be unlimited - minimum is 1");
      } else if (min != Ignore) {
         if (min <= 0)  {
            throw QRException("QROSE::setHeight - invalid min height (%d) - minimum is 1", min);
         } else {
            widget->setMinimumHeight(min);
         }
      }
      
      if (max == Unlimited) {
         QSizePolicy policy_in, policy_out;
         policy_in = widget->sizePolicy();
         policy_out = policy_in;
         policy_out.setVerticalPolicy(QSizePolicy::Expanding);
         widget->setSizePolicy(policy_out);    
      } else if (max != Ignore) {
         if (max <= 0)  {
            throw QRException("QROSE::setHeight - invalid max width (%d)", min);
         } else {
            widget->setMaximumHeight(max);
         }
      }      
   }    
        
   if (stretchFactor != Ignore) {
      QSizePolicy policy_in, policy_out;
      policy_in = widget->sizePolicy();
      policy_out = policy_in;
      policy_out.setVerticalStretch(stretchFactor);
      widget->setSizePolicy(policy_out);
   }
}
    
string QROSE::getName(const QWidget *widget) {
   if (!widget) {
      throw QRException("QROSE::getName - widget cannot be NULL!");
   }
   return widget->objectName().toAscii().constData();
}


int QROSE::msgBox(const char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);
   char buffer[1024];
   vsprintf(buffer, fmt, ap);

   QMessageBox box(QMessageBox::Information, "Info", buffer, QMessageBox::Ok);
   box.exec();

   va_end(ap);

   return 0;
}

string QROSE::format(const char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);
   static char buffer[10000];
   vsprintf(buffer, fmt, ap);
   va_end(ap);
   return buffer;
}
   


bool QROSE::canConnect(const QObject *sender, const char *signal, const QObject *receiver, const char *slot) {
	const QMetaObject *msnd = sender->metaObject ();
	const QMetaObject *mrec = receiver->metaObject ();

	char *slot_ex;	
	char *signal_ex;
	
	if (signal[0] >= '0' && signal[0] <= '9')
		signal_ex = (char *) &(signal[1]);
	else
		signal_ex = (char *) signal;
	
	
	if (slot[0] >= '0' && slot[0] <= '9')
		slot_ex = (char *) &(slot[1]);
	else
		slot_ex = (char *) slot;
      
	
	string normalised_signal = QMetaObject::normalizedSignature(signal_ex).data();	
	string normalised_slot = QMetaObject::normalizedSignature(slot_ex).data();
	
	int signal_index = msnd->indexOfSignal(normalised_signal.c_str());
	int slot_index = mrec->indexOfSlot(normalised_slot.c_str());
   
	if ((slot_index != -1) && (signal_index != -1)) {
		if (QMetaObject::checkConnectArgs(normalised_signal.c_str(), normalised_slot.c_str())) {
			return true;
		}
	}
	
	return false;
}
			
	
void QROSE::connect_ex(const QObject *sender, const char *signal, const QObject *receiver, const char *slot) {
	if (canConnect(sender, signal, receiver, slot)) 
		QObject::connect(sender, signal, receiver, slot);
}

void QROSE::disconnect_ex(const QObject *sender, const char *signal, const QObject *receiver, const char *slot) {
	if (canConnect(sender, signal, receiver, slot)) 
		QObject::disconnect(sender, signal, receiver, slot);

}

void QROSE::link(QObject *obj, const char *signal, CBVoidType cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBVoid, CBVoidType>(obj, signal, cbf, data);
}

void QROSE::link(QObject *obj, const char *signal, CBBoolType cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBBool, CBBoolType>(obj, signal, cbf, data);
}

void QROSE::link(QObject *obj, const char *signal, CBIntType cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBInt, CBIntType>(obj, signal, cbf, data);
}

void QROSE::link(QObject *obj, const char *signal, CBInt2Type cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBInt2, CBInt2Type>(obj, signal, cbf, data);
}

void QROSE::link(QObject *obj, const char *signal, CBInt3Type cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBInt3, CBInt3Type>(obj, signal, cbf, data);
}

void QROSE::link(QObject *obj, const char *signal, CBInt4Type cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBInt4, CBInt4Type>(obj, signal, cbf, data);
}

void QROSE::link(QObject *obj, const char *signal, CBStrType cbf, void *data) {
   QROSE_P::getCallbackMgr()->connect<QRCBStr, CBStrType>(obj, signal, cbf, data);
}

void QROSE::unlink(QObject *obj, const char *signal) {
   QROSE_P::getCallbackMgr()->disconnect(obj, signal);
}

QObject* QROSE::cbSender() {
   QRCBSession *cbSession = QROSE_P::getCBSession();
   if (!cbSession->isValid())
      throw QRException("QROSE::cbSender() can only be invoked within an event handler!");
   return cbSession->getCBSender();      
}

void* QROSE::cbData() {
   QRCBSession *cbSession = QROSE_P::getCBSession();
   if (!cbSession->isValid())
      throw QRException("QROSE::cbData() can only be invoked within an event handler!");
   return cbSession->getCBData();
}

} // namespace qrs
