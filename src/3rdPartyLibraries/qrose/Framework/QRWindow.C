/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRWindow     
 ***************************************************************************/

#include <QRWindow.h>
#include <QRMain_p.h>
#include <QRException.h>

#include <QList>
#include <stack>

using namespace std;

namespace qrs {

QRWindow::QRWindow(const string &name, string caption):
      QRTiledWidget(QROSE::TopDown, QROSE::NoSplitter, caption)
{
   initWindow(name);
}
   

QRWindow::QRWindow(const string &name, QROSE::Orientation orientation, string caption):
   QRTiledWidget(orientation, QROSE::NoSplitter, caption)
{
   initWindow(name);
}

QRWindow::QRWindow(const std::string &name, QROSE::SplitterMode splitterMode, string caption):
      QRTiledWidget(QROSE::TopDown, splitterMode, caption)
{
   initWindow(name);
}

QRWindow::QRWindow(const std::string &name, QROSE::Orientation orientation, 
                   QROSE::SplitterMode splitterMode, string caption):
      QRTiledWidget(orientation, splitterMode, caption)
{
   initWindow(name);
}


QRWindow::~QRWindow() {
   QROSE_P::getWindowsMap()->unregisterWindow(QROSE::getName(this)); 
}

void QRWindow::initWindow(const string &name) {
   if (name == "") 
      throw QRException("Window/Panel name cannot be an empty string!");
   setObjectName(name.c_str());
   QROSE_P::getWindowsMap()->registerWindow(name, this);
   setStartAsHidden(false);
   setRoot(false);
   m_isFixedSize = false;
   m_isDebugMode = false;
}

void QRWindow::setTitle(std::string title) {
   setWindowTitle(title.c_str());
}

bool QRWindow::isRoot() const {
   return m_isRoot;
}

void QRWindow::setRoot(bool p) {
   m_isRoot = p;
}

void QRWindow::setStartAsHidden(bool p) {
   m_isStartAsHidden = p;
}

bool QRWindow::startsAsHidden() const {
   return m_isStartAsHidden;
}

void QRWindow::setFixedSize() {
   m_isFixedSize = true;
}

bool QRWindow::isFixedSize() {
   return m_isFixedSize;
}

void QRWindow::setDebugMode(bool p) {
   m_isDebugMode = p;
}

void QRWindow::moveEvent ( QMoveEvent * event ) {
   outputSizeReport();
}

void QRWindow::resizeEvent ( QResizeEvent * event ) {
   outputSizeReport();
}

void QRWindow::outputSizeReport() {
   if (!m_isDebugMode) return;
   printf("\n\n=============================================[Size Report]===\n");
   stack <QList<QWidget *> > stackWidgets;
   
   QList<QWidget *> listWidgets; listWidgets.append(this);
   stackWidgets.push(listWidgets);
    
   while (!stackWidgets.empty()) {
      QList<QWidget *> widgets = stackWidgets.top(); stackWidgets.pop();
      bool new_level = false;
      while (widgets.size() && !new_level) {
         QWidget *widget = widgets.takeFirst();
         if (QROSE::getName(widget) != "") {
            printf("%*c%s [w:%d,h:%d,x:%d,y:%d]\n", (int) (3*stackWidgets.size()), ' ', 
                QROSE::getName(widget).c_str(), 
                widget->width(), widget->height(),
                widget->x(), widget->y());            
         }
         QList<QWidget *> childrenW;
         int n = widget->children().size();
         for (int i = 0; i < n; i++) {
             QObject *obj = widget->children().at(i);
             if (obj->isWidgetType()) {
                childrenW.append((QWidget *)obj);
             }
         }
         
         if (childrenW.size() != 0) {
            new_level = true;
            stackWidgets.push(widgets);
            stackWidgets.push(childrenW);
         }       
      }
   }
   printf("=============================================================\n");
   
}

} // namespace qrs

