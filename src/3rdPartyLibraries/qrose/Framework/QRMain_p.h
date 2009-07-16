/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho
    jgfc@doc.ic.ac.uk
    Description: the low-level details of the QROSE class go here
 ***************************************************************************/

#ifndef QRMAIN_P_H
#define QRMAIN_P_H

#include <QRMacros.h>
#include <QObject>
#include <QPoint>
#include <string>
#include <map>


namespace qrs {

class QRWindow;
class QRGroup;

// state of execution
// it records whether execution
// is inside the main-event loop or not
class QRExec: public QObject {
   friend class QROSE_P;

private:
   QRExec();

public:
   bool isExec();

public:
   void beforeExec();
   void afterExec();

protected:
   bool m_isExec;
};


/*******************************************************************************************[Callback mechanism]************/
class QRCBSession {
   friend class QROSE_P;

private:
   QRCBSession();

public:
   void setSession(QObject *sender, void *data);
   QObject *getCBSender();
   void* getCBData();
   bool isValid();

protected:
   QObject *m_sender;
   void *m_data;
   bool m_valid;
};

typedef void (*CBNormalizedType)();

class QRCallback: public QObject {
   friend class QRCallbackMgr;

protected:
   QRCallback(QObject *obj, CBNormalizedType cbf, void *data);

protected:
   void prepareDispatcher();
   void endDispatcher();

protected:
   QObject *m_sender;
   CBNormalizedType m_cbf;
   void *m_data;
};

class QRCBVoid: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBVoid(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher() {
      prepareDispatcher();
	  ((CBVoidType) m_cbf) ();
	  endDispatcher();
   }
};

class QRCBBool: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBBool(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(bool b) {
      prepareDispatcher();
      ((CBBoolType) m_cbf) (b);
      endDispatcher();
   }
};

class QRCBIntBool: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBIntBool(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(int i, bool b) {
      prepareDispatcher();
      ((CBIntBoolType) m_cbf) (i, b);
      endDispatcher();
   }
};

class QRCBInt: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBInt(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(int i) {
      prepareDispatcher();
      ((CBIntType) m_cbf) (i);
      endDispatcher();
   }
};

class QRCBInt2: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBInt2(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(int i, int j) {
      prepareDispatcher();
     ((CBInt2Type) m_cbf) (i, j);
     endDispatcher();
   }
   void dispatcher(const QPoint &p) {
      prepareDispatcher();
      ((CBInt2Type) m_cbf) (p.x(), p.y());
      endDispatcher();
   }
};

class QRCBInt3: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBInt3(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(int i, int j, int k) {
      prepareDispatcher();
      ((CBInt3Type) m_cbf) (i, j, k);
      endDispatcher();
   }
};


class QRCBInt4: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBInt4(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(int i, int j, int k, int l) {
      prepareDispatcher();
      ((CBInt4Type) m_cbf) (i, j, k, l);
      endDispatcher();
   }
};

class QRCBStr: public QRCallback {
   Q_OBJECT

   friend class QRCallbackMgr;

private:
   QRCBStr(QObject *obj, CBNormalizedType cbf, void *data): QRCallback(obj, cbf, data) { }

protected slots:
   void dispatcher(const QString &str) {
      prepareDispatcher();
      ((CBStrType) m_cbf) (str.toAscii().constData());
      endDispatcher();
   }
   void dispatcher(const char *str) {
      prepareDispatcher();
      ((CBStrType) m_cbf) (str);
      endDispatcher();
   }
};

class QRCallbackMgr {

   friend class QROSE_P;

private:
   QRCallbackMgr();

public:
   ~QRCallbackMgr();

   int disconnect(QObject *obj, const char *signal);

   template <class T, class F>
   int connect(QObject *obj, const char *signal, F cbf, void *data) {
      int id = disconnect(obj, signal);
		T* cb = new T(obj, (CBNormalizedType) cbf, data);
      connect(obj, signal, id, cb);
      return id;
   }

   void connect(QObject *obj, const char *signal, int id, QRCallback *cb);

protected:
   std::map<QObject*, std::map<int, QRCallback *> > m_cbs;
};

// contains the list of windows
class QRWindowsMap {

   friend class QROSE_P;

private:
   QRWindowsMap();

public:
   ~QRWindowsMap();
   void registerWindow(const std::string &name, QRWindow *win);
   void unregisterWindow(const std::string &name);
   QRWindow* findWindow(const std::string &name);
   void preExec();

protected:
   std::map<std::string, QRWindow *> m_windows;
};

class QROSE_P {

public:
    static QRExec *getExec() { return &m_exec; }
    static QRWindowsMap *getWindowsMap() { return &m_windows; }
    static QRCallbackMgr *getCallbackMgr() { return &m_callbackMgr; }
    static QRCBSession *getCBSession() { return &m_cbSession; }

    static QRGroup* getGroup(unsigned id);

protected:
    static QRWindowsMap m_windows;
    static QRExec m_exec;
    static QRCallbackMgr m_callbackMgr;
    static QRCBSession m_cbSession;
    static std::map<unsigned, QRGroup *> m_groups;

};

} // namespace qrs;

#endif
