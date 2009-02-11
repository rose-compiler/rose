/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRWidgetAcesss
 ***************************************************************************/

#include <QRWidgetAccess.h>
#include <QRTiledWidget.h>
#include <QRException.h>

using namespace std;

namespace qrs {

QRWidgetAccess::QRWidgetAccess(QRTiledWidget *twidget, const string &name) {
   m_twidget = twidget;
   m_name = name;
}

QWidget* QRWidgetAccess::insertWidget(QWidget *widget) {
   if (m_name == "") 
      throw QRException("statement in the form (window[\"name\"] = widget) invalid: name cannot be an empty string!");
   if (widget->objectName() != "") 
      throw QRException("statement in the form (window[\"name\"] = widget) invalid: widget already has a name!");
   widget->setObjectName(m_name.c_str());
   m_twidget->addWidget(widget);
   return widget;
}

QWidget *QRWidgetAccess::getWidget() {
   return m_twidget->getWidget(m_name);
}

} // namespace qrs
