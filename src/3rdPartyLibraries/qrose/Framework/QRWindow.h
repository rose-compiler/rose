/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRWindow
    Description:                                                     
 ***************************************************************************/

#ifndef QRWINDOWCLASS_H
#define QRWINDOWCLASS_H

#include <QRMacros.h>
#include <QRMain.h>
#include <QRTiledWidget.h>

#include <string>

namespace qrs {

class QRWindow: public QRTiledWidget {
    
   Q_OBJECT
         
   public:
      QRWindow(const std::string &name, std::string caption = "");
      QRWindow(const std::string &name, QROSE::Orientation orientation, std::string caption = "");
      QRWindow(const std::string &name, QROSE::SplitterMode splitterMode, std::string caption = "");
      QRWindow(const std::string &name, 
               QROSE::Orientation orientation, 
               QROSE::SplitterMode splitterMode, std::string caption = "");

      virtual  ~QRWindow();

      void setRoot(bool p);
      bool isRoot() const;
      void setStartAsHidden(bool p);
      bool startsAsHidden() const;
      void setTitle(std::string title);
      
      void setFixedSize();
      bool isFixedSize();
      
      void setDebugMode(bool p = true);
     
   protected:
      void initWindow (const std::string &name);
      void moveEvent ( QMoveEvent * event );
      virtual void resizeEvent ( QResizeEvent * event );      
      void outputSizeReport();

   protected:
      bool m_isStartAsHidden;
      bool m_isRoot;
      bool m_isFixedSize;
      bool m_isDebugMode;
};

}

#endif
