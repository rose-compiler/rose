/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>

#include "qeditor.h"
#include "linenumberwidget.h"
#include "levelwidget.h"
#include "markerwidget.h"
#include "paragdata.h"

#include <QRMain.h>
#include "QRSourceBox.h"
#include <QRToolBar.h>
#include <QRAstInfo.h>
#include <QRCoords.h>
#include "QRUnparser.h"
#include <QRIcons.h>
#include <qrose.h>

using namespace std;

namespace qrs {

int QRSourceBox::statusTimer = 2000;

QRSourceBox::QRSourceBox(): QWidget() {
   init(0);
}

QRSourceBox::QRSourceBox(SgNode *root): QWidget() {
   init(root);
}


QRSourceBox::~QRSourceBox() {
   delete m_astCoords;
}

void QRSourceBox::init(SgNode *root) {
    m_rootNode = root;

    QHBoxLayout* lay = new QHBoxLayout(  );

    m_wgEditor = new QEditor(this) ;

    m_wgLineNumberWidget = new LineNumberWidget( m_wgEditor, this );
    m_wgLevelWidget = new LevelWidget( m_wgEditor, this );
//    m_markerWidget = new MarkerWidget (m_wgEditor, this);
    connect( m_wgLevelWidget, SIGNAL(expandBlock(Q3TextParagraph*)),
      this, SLOT(expandBlock(Q3TextParagraph*)) );
    connect( m_wgLevelWidget, SIGNAL(collapseBlock(Q3TextParagraph*)),
     this, SLOT(collapseBlock(Q3TextParagraph*)) );
    connect(m_wgEditor, SIGNAL(rightClicked(const QPoint &, int,int)),
            this, SLOT(showPopupMenu(const QPoint &, int,int)));
    connect(m_wgEditor, SIGNAL(doubleClicked(int,int)),
            this, SLOT(removeHighlight(int,int)));

    //lay->addWidget( m_markerWidget );
    lay->addWidget( m_wgLineNumberWidget );
    lay->addWidget( m_wgLevelWidget );
    lay->addWidget( m_wgEditor );

    // tab bar
    /*
    m_tabbar = new QTabBar(this);
    m_tabbar->insertTab(new QTab(), 0);
    m_tabbar->insertTab(new QTab("CacheView"), 1);
    */

    // popup menu
    m_wgPopup = new Q3PopupMenu (this);
    connect(m_wgPopup, SIGNAL(activated(int)),
                  this, SLOT(broadcastTargetNode(int)));

    connect(m_wgPopup,SIGNAL(highlighted(int)),
                  this, SLOT(highlightTargetNode(int)));

    // status bar
    m_wgStatusbar = new QStatusBar(this);
    m_wgStatusbar->setSizeGripEnabled(false);
    QLabel* label = new QLabel( m_wgStatusbar );
    m_wgStatusbar->addWidget(label, 1, true);
    connect(this,SIGNAL(setInfo(const QString&,int)),m_wgStatusbar,
            SLOT(message(const QString&,int)));
    connect(m_wgStatusbar, SIGNAL(messageChanged(const QString&)),
            label, SLOT(setText(const QString&)));

    // toolbar
    m_wgToolbar = new QRToolBar(QROSE::LeftRight);
    m_wgToolbar->addToggleButton("target", RoseIcons::target);
    m_wgToolbar->insertSeparator();
    m_wgToolbar->addToggleButton("cache", RoseIcons::cacheview);
    m_wgToolbar->addToggleButton("collect", RoseIcons::cachecollect);
    m_wgToolbar->addButton("clear", RoseIcons::cacheclear);
    m_wgToolbar->addButton("broadcast", RoseIcons::cachebroadcast);
    m_wgToolbar->insertSeparator();
    m_wgToolbar->addButton("go to", RoseIcons::gotoline);
    m_wgToolbar->addToggleButton("lookup", RoseIcons::lookup);
    m_wgToolbar->insertSeparator();
    m_wgToolbar->addButton("zoom+", RoseIcons::zoomin);
    m_wgToolbar->addButton("zoom-", RoseIcons::zoomout);

    QROSE::link(m_wgToolbar, SIGNAL(clicked(int)), QRSourceBox::toolbarClicked, this);


    Q3VBoxLayout *layv = new Q3VBoxLayout(this);
    layv->addWidget(m_wgToolbar);
   // layv->addWidget(m_tabbar);
    layv->addLayout(lay);
    layv->addWidget(m_wgStatusbar);

    m_wgEditor->setReadOnly(TRUE);

    if (m_rootNode == 0) {
       setCacheView(true);
       m_astCoords = NULL;
       m_wgToolbar->setEnabled(tbCacheView, false);
    } else {
       // get source coordinates
       m_astCoords = new QRAstCoords();
       SgFile *file;
       if (isSgProject(m_rootNode))
           file = &((SgProject *) m_rootNode)->get_file(0);
       else {
           file = (SgFile *) m_rootNode;
       }
       m_sourceCode = unparseFileToString(file, true);
       setCacheView(false);
       m_wgToolbar->setEnabled(tbCacheView, true);
    }

    // defaults
    setTargetMode(true);
    setCollectMode(false);
    setLookup(true);
    setLookupWithHighlight(true);
    m_wgEditor->zoomTo(10);
    emit setInfo("",0);
    doRepaint();
}

void QRSourceBox::toolbarClicked(int index) {
	QRSourceBox *sourcebox = QROSE::cbData<QRSourceBox *>();
	QRToolBar *tb = QROSE::cbSender<QRToolBar *>();
	switch (index) {
	case tbTargetMode: sourcebox->setTargetMode(tb->isChecked(index)); break;
	case tbCacheView: sourcebox->setCacheView(tb->isChecked(index)); break;
	case tbCollectMode: sourcebox->setCollectMode(tb->isChecked(index)); break;
	case tbClearCache: sourcebox->clearCache(); break;
	case tbBroadcastCache: sourcebox->broadcastCache(); break;
	case tbGoto: sourcebox->gotoLine(); break;
	case tbLookup: sourcebox->setLookup(tb->isChecked(index)); break;
	case tbZoomIn: sourcebox->zoomIn(); break;
	case tbZoomOut: sourcebox->zoomOut(); break;
	}
}


SgNode *QRSourceBox::getRoot() {
   return m_rootNode;
}

void QRSourceBox::showToolbar(bool setOn) {
   m_wgToolbar->setHidden(!setOn);
}

void QRSourceBox::eventCacheClear() {
   m_cacheSource.clear();
   if (m_isCacheView)
      updateCacheView();
   updateCacheViewButton();
   emit setInfo("cache cleared", statusTimer);
}

void QRSourceBox::eventCacheNewNodes(set<SgNode *> &nodes) {
   int i;
   i = cacheSize() - nodes.size() + 1;
   for (set<SgNode *>::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
	   string code;
	   SgNode *node = *iter;
      // get source-file
	   string filename;
	   SgNode *file = node;

	   while (file && !isSgFile(file)) file = file->get_parent();
	   if (file) {
		   int pos;
		   filename = ((SgFile *) file)->getFileName();
		   if ((pos = filename.find_last_of("\\/")) != string::npos) {
		      filename = filename.substr(pos + 1);
		   }
		   filename = string(" <") + filename + ">";
	   }

	  if (QRAstInfo::isUnparseBlackList(node)) {
	      code = QROSE::format("\n/***** [ %d: %s%s(note: node has not been unparsed) ] ****/\n",
	    	  i, node->sage_class_name(), filename.c_str());
	  } else {
	      code += QROSE::format("\n/***** [ %d: %s%s] ****/\n",
      	    	  i, node->sage_class_name(), filename.c_str()) + node->unparseToCompleteString();
	  }
      i++;
      m_cacheSource += code;
   }
   if (m_isCacheView) updateCacheView();
   updateCacheViewButton();
   emit setInfo(QString("%1 %2 been received [total=%3]")
                .arg(nodes.size()).arg((nodes.size() == 1)? "node has":
                                 "nodes have").arg(cacheSize()), statusTimer);
}

void QRSourceBox::internalExpandBlock( Q3TextParagraph* p )
{
    ParagData* data = (ParagData*) p->extraData();
    if( !data ){
        return;
    }
    int lev = QMAX( data->level() - 1, 0 );

    data->setOpen( true );

    p = p->next();
    while( p ){
        ParagData* data = (ParagData*) p->extraData();
        if( data ){
            p->show();
            data->setOpen( true );

            if( data->level() == lev ){
                break;
            }
            p = p->next();
        }
    }
}

void QRSourceBox::internalCollapseBlock( Q3TextParagraph* p )
{
    ParagData* data = (ParagData*) p->extraData();
    if( !data ){
        return;
    }

    int lev = QMAX( data->level() - 1, 0 );
    data->setOpen( false );

    p = p->next();
    while( p ){
        ParagData* data = (ParagData*) p->extraData();
        if( data ){

            if( data->level() == lev ){
                break;
            }

            p->hide();

            p = p->next();
        }
    }
}

void QRSourceBox::doRepaint()
{
   // m_markerWidget->doRepaint();
    m_wgLineNumberWidget->doRepaint();
    m_wgLevelWidget->doRepaint();
    m_wgEditor->refresh();
}

void QRSourceBox::expandBlock( Q3TextParagraph* p )
{
    internalExpandBlock( p );

    m_wgEditor->setCursorPosition( p->paragId(), 0 );
    m_wgEditor->refresh();
    doRepaint();
}

void QRSourceBox::collapseBlock( Q3TextParagraph* p )
{
    internalCollapseBlock( p );

    m_wgEditor->setCursorPosition( p->paragId(), 0 );
    m_wgEditor->refresh();
    doRepaint();
}

void QRSourceBox::showPopupMenu(const QPoint &point, int line, int col) {

    m_wgPopup->clear();

    if (m_isCacheView) {
       m_wgPopup->insertItem
             ("CodeTouch is disabled (reason: cache view is enabled)");
    } else if (!m_astCoords) {
       m_wgPopup->insertItem
             ("CodeTouch is disabled");
    } else {
    	m_targetCoords = m_astCoords->get_target_boxes(line, col);

        unsigned size = m_targetCoords.size();
        for (int i = size - 1; i >= 0; i--) {
          SgNode *node = m_targetCoords[i]->getNode();
          QPixmap *pixmap = QRAstInfo::getPixmap(node);
          /*
          string tmp, str;
          tmp = m_targetCoords[i]->str();
          const char *chr;
          chr = tmp.c_str();
          int n = tmp.size();
          int j; j = 0;
          while (j++ < n) {
             if (*chr == '&') str += "&&"; else str += *chr; chr++;
           }
*/
          if (!pixmap) {
        	m_wgPopup->insertItem(node->class_name().c_str(), i);
          } else {
            QIconSet *iconset = new QIconSet(*pixmap);
             m_wgPopup->insertItem(*iconset, node->class_name().c_str(), i);
          }

       }
   }

   QPoint pos = QCursor::pos();
   pos.setY(pos.y()+10);
   m_wgPopup->exec(pos, 0);
}

void QRSourceBox::broadcastTargetNode(int id) {
    if (!m_astCoords || !m_astCoords->getMainBox())
       return;

    ROSE_ASSERT((id >= 0) && (id < (int) m_targetCoords.size()));

    QRCoordBox *box = m_targetCoords[id];
    broadcastNode(box->getNode());
}

void QRSourceBox::highlightTargetNode(int id) {
    if (!m_astCoords || !m_astCoords->getMainBox())
       return;

    ROSE_ASSERT((id >= 0) && (id < (int) m_targetCoords.size()));

    QRCoordBox *box = m_targetCoords[id];

    m_wgEditor->setSelection(box->line0(), box->col0(), box->line1(),
                           box->col1(), QEditor::sel1);
    m_wgEditor->gotoLine(box->line0()+1);
}

void QRSourceBox::setLookupWithHighlight(bool isOn) {
   m_lookupWithHighlight = isOn;
}

void QRSourceBox::removeHighlight(int, int) {
   m_wgEditor->removeSelection(QEditor::sel0);
   m_wgEditor->removeSelection(QEditor::sel1);
}


bool QRSourceBox::isCacheView() {
   return m_isCacheView;
}

void QRSourceBox::setTargetMode(bool isOn) {
   QRoseComponent::setTargetMode(isOn);
   emit setInfo(QString("target mode: %1 [%2]")
                         .arg(isOn? "ON": "OFF")
                         .arg(isOn? "accepting incoming nodes":
                                    "rejecting incoming nodes"),statusTimer);
   m_wgToolbar->setChecked(tbTargetMode, isOn);
}


void QRSourceBox::setCollectMode(bool isOn) {
   QRoseComponent::setCollectMode(isOn);
   emit setInfo(QString("accumulator mode: %1 [%2]")
               .arg(isOn? "ON": "OFF")
               .arg(isOn? "new nodes are appended to the cache":
                          "new nodes overwrite existing nodes in the cache"), statusTimer);
   m_wgToolbar->setChecked(tbCollectMode, isOn);
}

void QRSourceBox::clearCache() {
   QRoseComponent::clearCache(true);
}

void QRSourceBox::broadcastCache() {
   if (cacheSize() == 0) return;
   QRoseComponent::broadcastCache();
   emit setInfo(QString("cache broadcasted [%1 %2]!").arg(cacheSize()).arg(cacheSize() == 1? "node": "nodes"), statusTimer);
}

void QRSourceBox::setCacheView(bool isOn) {

   m_isCacheView = isOn;
   if (isOn) {
     updateCacheView();
   } else {
     updateSourceView();
   }
   emit setInfo(QString("cache view mode: %1")
                        .arg(isOn? "ON": "OFF"), statusTimer);
}

void QRSourceBox::updateCacheViewButton() {
   unsigned n = cacheSize();
   if (n == 0) {
      m_wgToolbar->setCaption(tbCacheView, "cache");
   } else {
      m_wgToolbar->setCaption(tbCacheView, QString("cache [%1]").arg(n).toStdString());
   }
}


void QRSourceBox::updateCacheView() {
   int size = m_wgEditor->pointSize();

   m_wgEditor->hide();
   QString code;

   if (cacheSize() > 1) {
     code = QString("// [Cache View (%1 %2)]\n %3")
                .arg(cacheSize())
                .arg((cacheSize() == 1)? "element" : "elements")
                .arg(m_cacheSource.c_str());
   } else {
     code = m_cacheSource.c_str();
   }

   m_wgEditor->setText(code);

   m_wgEditor->zoomTo(size-1); m_wgEditor->refresh();
   m_wgEditor->zoomTo(size);
   doRepaint();
   m_wgEditor->show();
}

void QRSourceBox::updateSourceView() {
   m_wgEditor->hide();
   int size = m_wgEditor->pointSize();
   m_wgEditor->setText(m_sourceCode.c_str()); m_wgEditor->refresh();
   m_wgEditor->zoomTo(size-1); m_wgEditor->refresh();
   m_wgEditor->zoomTo(size);
   doRepaint();
   m_wgEditor->show();
}


bool QRSourceBox::isLookup() {
   return m_isLookup;
}

void QRSourceBox::setLookup(bool isOn) {
  m_isLookup = isOn;
   emit setInfo(QString("lookup: %1 [%2]")
                         .arg(isOn? "ON": "OFF")
                         .arg(isOn? "searching incoming node":
                                    "not searching incoming node"), statusTimer);
 }

void QRSourceBox::gotoLine() {
   string line_num;
   if (QROSE::inputBox("Goto Line", "Specify line number:", line_num)) {
       int num = atoi(line_num.c_str()) -1;
       m_wgEditor->gotoLine(num, true);
   }
}

void QRSourceBox::zoomIn() {
   int size = m_wgEditor->pointSize();
   m_wgEditor->zoomTo(size+1);
   emit setInfo(QString("font size: %1")
                        .arg(size+1), statusTimer);
   doRepaint();
}

void QRSourceBox::zoomOut() {
   int size = m_wgEditor->pointSize();
   if (size > 1) {
      m_wgEditor->zoomTo(size-1);
      emit setInfo(QString("font size: %1")
                          .arg(size-1), statusTimer);
      doRepaint();
   }
}

void QRSourceBox::receiveMsg(QRGroupWidget *emitter, QRMessage *msg) {
   switch (msg->id()) {
      case msg_node:
          {
	     SgNode *node = ((QRoseComponent::SgNodeMsg *) msg)->getNode();
             if (isTargetMode()) {
                insertNodeInCache(node);
             }
             lookupNode(node);
	  }
	  break;
      case msg_nodes:
          {
             set<SgNode *> *nodes = ((SgNodesMsg *) msg)->getNodes();
             if (isTargetMode()) {
                insertNodesInCache(nodes);
             }
          }
	  break;
   }
}

void QRSourceBox::lookupNode(SgNode *node) {
   if (isCacheView()) return;
   if (!isLookup()) return;
   if (!m_astCoords) return;
   map<SgNode *, QRCoordBox *>* coords = m_astCoords->nodeCoords();
   if (!coords) return;
   std::map<SgNode *, QRCoordBox *>::iterator iter = coords->find(node);
   if (iter == coords->end()) return;
   QRCoordBox *box = iter->second;
   if (!box) return;

   m_wgEditor->setSelection(box->line0(), box->col0(), box->line1(),
                              box->col1(), QEditor::sel1);
   m_wgEditor->gotoLine(box->line0()+1);
   // the reason why we select code in the first place is that it better positions the text.
   if (!m_lookupWithHighlight) {
     m_wgEditor->removeSelection(QEditor::sel1);
   }

}

// adapted from unparseToCompleteString
string QRSourceBox::unparseFileToString (SgFile *file, bool use_coords) {
     SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
     inputUnparseInfoPointer->unset_SkipComments();    // generate comments
     inputUnparseInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code

     ostringstream outputString;

     string fileName = file->getFileName();

     Unparser_Opt roseOptions;

     QRUnparser *roseUnparser = new QRUnparser(m_astCoords, &outputString, fileName, roseOptions);
     switch (file->variantT())
        {
          case V_SgSourceFile:
             {
               SgSourceFile* sourceFile = isSgSourceFile(file);
               roseUnparser->unparseFile(sourceFile, *inputUnparseInfoPointer);

               break;
             }

          case V_SgBinaryFile:
             {
               SgBinaryFile* binaryFile = isSgBinaryFile(file);
               roseUnparser->unparseFile(binaryFile, *inputUnparseInfoPointer);
               break;
             }

          case V_SgUnknownFile:
             {
               SgUnknownFile* unknownFile = isSgUnknownFile(file);

               unknownFile->set_skipfinalCompileStep(true);

               printf ("Warning: Unclear what to unparse from a SgUnknownFile (set skipfinalCompileStep) \n");
               break;
             }

          default:
             {
               printf ("Error: default reached in unparser: file = %s \n",file->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     outputString.put(0);
     return outputString.str();
}

}
