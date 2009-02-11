/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRPANEL     
    Description:                                                     
 ***************************************************************************/

#include <QRPanel.h>
#include <QRException.h>

#include <QLayout>

using namespace std;

namespace qrs {

QRPanel::~QRPanel() {
   delete m_setOrientation;
   delete m_setSplitterMode;
}   

void QRPanel::initialize(string caption) {
   m_setOrientation = NULL;
   m_setSplitterMode = NULL;
   m_caption = caption;
}   

QRPanel::QRPanel(string caption) { 
   initialize(caption);
}

QRPanel::QRPanel(QROSE::Orientation orientation, string caption) {
   initialize(caption);
   m_setOrientation = new QROSE::Orientation; *m_setOrientation = orientation;
}

QRPanel::QRPanel(QROSE::SplitterMode splitterMode, string caption) {
   initialize(caption);
   m_setSplitterMode = new QROSE::SplitterMode; *m_setSplitterMode = splitterMode;  
}
   
QRPanel::QRPanel(QROSE::Orientation orientation, 
                 QROSE::SplitterMode splitterMode, string caption)
{
   initialize(caption);

   m_setOrientation = new QROSE::Orientation; *m_setOrientation = orientation;
   m_setSplitterMode = new QROSE::SplitterMode; *m_setSplitterMode = splitterMode;     
}
  
void QRPanel::childIsAttached(QRTiledWidget *parent) {
   QROSE::Orientation panelOrientation;
   QROSE::SplitterMode panelSplitterMode;

   if (m_setOrientation) {
      panelOrientation = *m_setOrientation;
   } else {
      if (parent->getOrientation() == QROSE::TopDown)
         panelOrientation = QROSE::LeftRight;
      else if (parent->getOrientation() == QROSE::LeftRight)
         panelOrientation = QROSE::TopDown;
      else
         throw QRException("QRPanel::QRPanel - parent has an unknown orientation!");
   }
   
   if (m_setSplitterMode) {
      panelSplitterMode = *m_setSplitterMode;
   } else {
      panelSplitterMode = QROSE::NoSplitter;
   }

   init(panelOrientation, panelSplitterMode, m_caption);
	
	
	if (m_caption == "") {
      m_layout->setContentsMargins(0,0,0,0);
	}
	
	QRTiledWidget::childIsAttached(parent);

}

} // namespace qrs
