/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRWidgetAccess     
    Description:                                                     
 ***************************************************************************/

#ifndef QRWIDGETACCESS_H
#define QRWIDGETACCESS_H

#include <QRMacros.h>
#include <QWidget>
#include <string>

namespace qrs {

class QRTiledWidget;

class QRWidgetAccess {
   friend class QRTiledWidget;

public:
   // LHS[x] << widget
   template <class T> T* operator << (T* widget) {
      return (T*) insertWidget(widget);
   }

   template <class T> T& operator << (T& widget) {
      return (T&) *insertWidget(&widget);
   }

   // RHS widget = X["name"]
   template <class T> operator T ()  {
      return (T) getWidget();
   }

protected:
   QWidget* insertWidget(QWidget *widget);
   QWidget *getWidget();

private:
   // prevent the constructor from being invoked outside QRTiledWidget
   QRWidgetAccess(QRTiledWidget *twidget, const std::string &name);

   //  prevent from X["name1"] = Y["name2"] from being invoked
   template <class T> void operator = (T *obj);

protected:
   QRTiledWidget *m_twidget;
   std::string m_name;
};
   

} // namespace qrs

#endif
