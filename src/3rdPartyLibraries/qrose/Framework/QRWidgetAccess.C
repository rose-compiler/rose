/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRWidgetAcesss
 ***************************************************************************/

#include <QRWidgetAccess.h>
#include <QRTiledWidget.h>
#include <QRException.h>

#include <QtUiTools/QUiLoader>
#include <QFile>

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

QWidget * QRWidgetAccess::loadWidget(const std::string& path)
{
    QUiLoader loader;
    
    QFile file(QString::fromStdString(path));
    file.open(QFile::ReadOnly);

    QWidget * newWidget = loader.load(&file);
    

    //use name from code (in [] brackets)
    newWidget->setObjectName(m_name.c_str());
    //ALTERNATIVE: use name out of qt-designer
    //m_twidget->addWidget(widget);
    
    m_twidget->addWidget(newWidget);  
            
    return newWidget;
}



} // namespace qrs
