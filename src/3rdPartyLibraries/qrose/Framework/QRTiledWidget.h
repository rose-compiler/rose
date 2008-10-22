/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRTileManager     
    Description:                                                     
 ***************************************************************************/

#ifndef QRTILEDWIDGETCLASS_H
#define QRTILEDWIDGETCLASS_H

#include <QRMacros.h>
#include <QRWidgetAccess.h>
#include <QRMain.h>

#include <QGroupBox>

#include <map>
#include <string>

class QLayout;
class QSplitter;

namespace qrs {

   
class QRTiledWidget: public QWidget {

   Q_OBJECT

   friend class QRWidgetAccess;

   public:
      QRTiledWidget();
      QRTiledWidget(QROSE::Orientation orientation, 
                    QROSE::SplitterMode splitterMode,
                    std::string caption);
      QRWidgetAccess operator [] (const std::string &name);
		template <class T> T* operator << (T* widget) {
         return (T*) addWidget(widget);
      }
		template <class T> T& operator << (T& widget) {
         return (T&) *addWidget(&widget);
      }
      
      QWidget *findWidget(const std::string &name);
      QROSE::Orientation getOrientation() const;
           
      void setTileSize(int size0, ...);
      void setSpacing (int size);
      void setMargins (int left, int top, int right, int bottom);
      
      void setCheckable(bool isCheckable);
      bool isChecked() const;

   public slots:
      virtual void setChecked(bool checked);
    
   protected slots:
      virtual void childIsAttached(QRTiledWidget *parent);
     
   signals:
      void childAttached(QRTiledWidget *);
      void checkToggled(bool on);
      
   protected:
      void init(QROSE::Orientation orientation, 
                QROSE::SplitterMode splitterMode, 
                std::string caption);
      void registerWidget(const std::string &name, QWidget *widget);
      QRTiledWidget *getTiledParent();
      QWidget *addWidget(QWidget *widget);      
      QWidget *getWidget(const std::string &name);
      
   protected:
      std::map<std::string, QWidget *> m_widgets;
      QRTiledWidget *m_tiledParent;
      QSplitter *m_splitter;
      QLayout *m_layout;
      QROSE::Orientation m_orientation;  
      QGroupBox *m_gbox;

};
  
} // namespace qrs

#endif
