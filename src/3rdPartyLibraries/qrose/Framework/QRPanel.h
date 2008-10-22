/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRPANEL     
    Description:                                                     
 ***************************************************************************/

#ifndef QRPANELCLASS_H
#define QRPANELCLASS_H

#include <QRMacros.h>
#include <QRMain.h>
#include <QRTiledWidget.h>

#include <string>

namespace qrs {

// panel used to divide a window into an arbitrary number of tiles
class QRPanel: public QRTiledWidget {
   public:

      virtual ~QRPanel();

      QRPanel(std::string caption = "");
      QRPanel(QROSE::Orientation orientation, std::string caption = "");
      QRPanel(QROSE::SplitterMode splitterMode, std::string caption = "");
      QRPanel(QROSE::Orientation orientation, 
              QROSE::SplitterMode splitterMode, std::string caption = "");
      
      virtual void childIsAttached(QRTiledWidget *parent);
      
   protected:
      void initialize(std::string caption);      

   protected:
      QROSE::Orientation *m_setOrientation;
      QROSE::SplitterMode *m_setSplitterMode;
      std::string m_caption;
   };

} // namespace qrs;

#endif
