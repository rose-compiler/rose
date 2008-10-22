/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRTiledWidget
 ***************************************************************************/

#include <QRTiledWidget.h>
#include <QRException.h>
#include <QRWindow.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QList>
#include <stdarg.h>

#include <QPlastiqueStyle>

using namespace std;

namespace qrs {
     
QRTiledWidget::QRTiledWidget() {  }

QRTiledWidget::QRTiledWidget(QROSE::Orientation orientation, QROSE::SplitterMode splitterMode,
                             std::string caption) 
{
   init(orientation, splitterMode, caption);
}

void QRTiledWidget::init(QROSE::Orientation orientation, 
                         QROSE::SplitterMode splitterMode,
                         string caption) 
{
   
   m_orientation = orientation;
   
   if (caption != "") {
      m_gbox = new QGroupBox(caption.c_str());
      m_gbox->setStyle(new QPlastiqueStyle);
   } else {
      m_gbox = NULL;
   }
      
   if (m_orientation == QROSE::TopDown) {
      m_layout = new QVBoxLayout;
      m_splitter = (splitterMode == QROSE::UseSplitter)? new QSplitter(Qt::Vertical, this) : NULL;
   } else if (m_orientation == QROSE::LeftRight) {
      m_layout = new QHBoxLayout;
      m_splitter = (splitterMode == QROSE::UseSplitter)? new QSplitter(Qt::Horizontal, this) : NULL;

   } else {
      throw QRException("invalid orientation - use 'topDown' or 'leftRight'");
   }

   if (m_splitter) {
      m_layout->addWidget(m_splitter);
      

#ifdef __CYGWIN__
      // the splitter is invisible in windows, so
      // we change to Linux style
      m_splitter->setStyle(new QPlastiqueStyle);
#endif

   }
   
   if (m_gbox) {
      m_gbox->setLayout(m_layout);   
      QLayout *layout = new QVBoxLayout;
      layout->addWidget(m_gbox);
      setLayout(layout);
      QObject::connect(m_gbox, SIGNAL(toggled(bool)), this, SIGNAL(checkToggled(bool)));
   } else {
      setLayout(m_layout);
   }      
   m_tiledParent = NULL;
}


QRWidgetAccess QRTiledWidget::operator [] (const string &name) {
   QRWidgetAccess access(this, name);
   return access;
}

void QRTiledWidget::registerWidget(const string &name, QWidget *widget) {
   map<string, QWidget *>::iterator iter;
   iter = m_widgets.find(name);
   eAssert(iter == m_widgets.end(), ("widget with name [%s] already exists in [%s]: use another name!", name.c_str(), QROSE::getName(this).c_str()));
   m_widgets[name] = widget;
}

QRTiledWidget* QRTiledWidget::getTiledParent() {
   return m_tiledParent;
}

QWidget *QRTiledWidget::addWidget(QWidget *widget) {
   
   if (m_splitter) {
      m_splitter->addWidget(widget);
   } else {
      m_layout->addWidget(widget);
   } 
	   
	// connect only if widget has the right slot
   QROSE::connect_ex(this, SIGNAL(childAttached(QRTiledWidget *)), widget, SLOT(childIsAttached(QRTiledWidget *)));
   emit childAttached(this);
   QROSE::disconnect_ex(this, SIGNAL(childAttached(QRTiledWidget *)), widget, SLOT(childIsAttached(QRTiledWidget *)));

	string name = widget->objectName().toAscii().constData();
	if (name != "") { // propagate name in all ancestors panels
		QRTiledWidget *parent = this;
		while (parent) {
			parent->registerWidget(name, widget);
			parent = parent->getTiledParent();	
		}
	}
		
   return widget;
}

void QRTiledWidget::childIsAttached(QRTiledWidget *parent) {
	m_tiledParent = parent;

}

void QRTiledWidget::setCheckable(bool isCheckable) {
   eAssert(m_gbox, ("widget [%s] cannot be made checkable! To make it checkable, set a caption in the constructor!",
           QROSE::getName(this).c_str()));
   
   m_gbox->setCheckable(isCheckable);
}
   
              
bool QRTiledWidget::isChecked() const {
   eAssert(m_gbox, ("widget [%s] is not checkable! To make it checkable, set a caption in the constructor!",
            QROSE::getName(this).c_str()));
   
   return m_gbox->isChecked();
}

void QRTiledWidget::setChecked(bool checked) {
   eAssert(m_gbox, ("widget [%s] is not checkable! To make it checkable, set a caption in the constructor!",
            QROSE::getName(this).c_str()));
   
   m_gbox->setChecked(checked);
}

QWidget *QRTiledWidget::findWidget(const string &name)  {
   if (name == "") throw QRException("QRWindow::findWidget - name cannot be empty!");
   
   map<string, QWidget *>::iterator iter = m_widgets.find(name);
   
   if (iter != m_widgets.end())
      return iter->second;
   else
      return NULL; 
}

QWidget *QRTiledWidget::getWidget(const string &name)  {
   QWidget *widget = findWidget(name);
   if (!widget)
      throw QRException("cannot find widget [%s] inside [%s]!", name.c_str(), QROSE::getName(this).c_str());
   return widget;
}


QROSE::Orientation QRTiledWidget::getOrientation() const { 
    return m_orientation;  
}

void QRTiledWidget::setTileSize(int size0, ...) {
   if (!m_splitter)
      throw QRException("QRTiledWidget::setTileSize - invalid window/panel: must use splitter!");
    
   QRWindow *win = QROSE::getQRWindow(this);
   
   va_list lst;
   
   va_start(lst, size0);
   
   bool is_shown = win->isVisible();
   win->show();
  
   QList<int> rel_sizes;
   int total_rel_size = 0;
   QList<int> cur_sizes = m_splitter->sizes();
   
   int n = cur_sizes.count();    
   if (n == 0) return;

   QList<int>::Iterator it1 = cur_sizes.begin();
    
   int total_cur_size = 0;
   int size = size0;
   for (int i = 1; i < n; i++) {
      total_rel_size += size; 
      if (total_rel_size < 0) {
         total_rel_size = size = 0; 
      } else if (total_rel_size > 100) {
         total_rel_size -= size; size = 0;
      }       
      rel_sizes.push_back(size);
      size = va_arg(lst, int);
      total_cur_size += *it1; it1++;
   }

   rel_sizes.push_back(100 - total_rel_size);
   total_cur_size += *it1;    
    
   it1 = cur_sizes.begin();
   QList<int>::Iterator it2 = rel_sizes.begin();
   int count = 0;
   while( it1 != cur_sizes.end() ) {
      int &val = *it1;
      count += val;
      val = (int) ((float) total_cur_size * (float) (*it2 / 100.0));
      ++it1; ++it2;
      if (it1 == cur_sizes.end()) {
         if (count != total_cur_size) {
            val = total_cur_size - (count - val);
         }
      }
   }
       
   va_end(lst);
   m_splitter->setSizes(cur_sizes);    
   win->setShown(is_shown);
 
}

void QRTiledWidget::setSpacing(int size) {
   if (m_splitter) { 
      throw QRException("QRTiledWidget::setTileSize - setSpacing cannot be used when using splitter!");
   }
   m_layout->setSpacing(size);   
}

void QRTiledWidget::setMargins(int left, int top, int right, int bottom) {
   m_layout->setContentsMargins(left, top, right, bottom);
}

} // namespace qrs
