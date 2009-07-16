/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho
    jgfc@doc.ic.ac.uk
    Class: QROSE
    Description: Frontend class for the framework module
 ***************************************************************************/

#ifndef QROSECLASS_H
#define QROSECLASS_H

#include <QRMacros.h>
#include <string>

class QApplication;
class QWidget;
class QObject;

namespace qrs {

class QRWindow;
class QRGroup;

class QROSE {

public:
    // used to define whether orientation of widgets goes from top to down, or
    // left to right.
   typedef enum {TopDown, LeftRight} Orientation;
   // defines whether a splitter (slider) is used or not.
   typedef enum {NoSplitter, UseSplitter} SplitterMode;
   // layout size specification
   typedef enum {Ignore = -1, Unlimited = -2} Size;

//=============================================================[ window management ]======
   // initializes QROSE - this method must be called before any other call to QROSE
   static void init(int &argc, char **argv);
   // returns an instance of the current QApplication
   static QApplication* app();
   // starts the main-event loop
   static int exec();
   // are we in the main-event loop?
   static bool isExec();
   // exits the main-event loop with return code
   static void exit(int returnCode = 0);
   // process GUI event
   static void processEvents();

   // gets window (an exception is throwed if not found)
   static QRWindow* getWindow(const std::string &name);
   // gets window, returns NULL if not found
   static QRWindow* findWindow(const std::string &name);
   // gets parent window for a particular widget
   static QRWindow* getQRWindow(QWidget *widget);
   // show a message box
   static int msgBox(const char *fmt, ...);
   // converts printf style arguments to a string
   static std::string format(const char *fmt, ...);
   // input box
   static bool inputBox(std::string caption, std::string label, std::string &return_text, std::string default_text = std::string());


//  ============================================================[ widget attributes ]======
   static std::string getName(const QWidget *widget);
   static void setName(const QWidget *widget, std::string);
   static void setWidth(QWidget *widget, int size);
   static void setWidth(QWidget *widget, int min, int max, int stretchFactor = Ignore);
   static void setHeight(QWidget *widget, int size);
   static void setHeight(QWidget *widget, int min, int max, int stretchFactor = Ignore);

//  =============================================================[ event handling ]======
   static bool canConnect(const QObject *sender, const char *signal, const QObject *receiver,
                           const char *slot);

   // like Qt's connect, but will not complain if slot is not available.
   static void connect_ex(const QObject *sender, const char *signal, const QObject *receiver,
                           const char *slot);
   static void disconnect_ex(const QObject *sender, const char *signal, const QObject *receiver,
                           const char *slot);

   // QROSE's version of "connect"- allows to connect a signal to a method/function instead of
   // a slot. This means that is not necessary to run Qt's metacompiler.
   static void link(QObject *obj, const char *signal, CBVoidType cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBBoolType cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBIntType cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBIntBoolType cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBInt2Type cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBInt3Type cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBInt4Type cbf, void *data = 0);
   static void link(QObject *obj, const char *signal, CBStrType cbf, void *data = 0);
   static void unlink(QObject *obj, const char *signal);

   //  ============================================================[ group management ]======
   static QRGroup* getGroup(unsigned id);

   // whenever inside a callback, cbSender returns the object
   // that emitted the signal
   static QObject* cbSender();
   template <class T>
         static T cbSender() {
            return (T) cbSender();
         }

   // returns an arbitrary data object provided in the link method (4th argument).
   static void* cbData();
   template <class T>
      static T cbData() {
         return (T) cbData();
      }
};


} // namespace qrs

#endif


