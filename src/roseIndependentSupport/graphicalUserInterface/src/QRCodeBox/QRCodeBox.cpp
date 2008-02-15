
#include <config.h>
#include <rose.h>

#include <QRLayout.h>
#include "QRGui.h"
#include "QRCodeBox.h"
#include "qeditor.h"
#include "linenumberwidget.h"
#include "levelwidget.h"
#include "markerwidget.h"
#include <qlayout.h>
#include "paragdata.h"
#include "QRUnparser.h"
#include <QRCoords.h>
#include <list>
#include <QRAstInfo.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <QRToolBar.h>
#include <qtabbar.h>
#include <qlabel.h>

using namespace std;

QRCodeBox::QRCodeBox(const char *name): QWidget(QRGUI::current_panel_frame(), name) {
   init();
}


QRCodeBox::QRCodeBox(QWidget *parent, const char *name): QWidget(parent, name) {
   init();
}

QRCodeBox::~QRCodeBox() {
   delete m_ast_coords;  
}

void QRCodeBox::init() {
    QHBoxLayout* lay = new QHBoxLayout(  );
    m_editor = new QEditor( this );
    m_lineNumberWidget = new LineNumberWidget( m_editor, this );
    m_levelWidget = new LevelWidget( m_editor, this );
//    m_markerWidget = new MarkerWidget (m_editor, this);
    connect( m_levelWidget, SIGNAL(expandBlock(QTextParagraph*)),
	     this, SLOT(expandBlock(QTextParagraph*)) );
    connect( m_levelWidget, SIGNAL(collapseBlock(QTextParagraph*)),
	    this, SLOT(collapseBlock(QTextParagraph*)) );
    connect(m_editor, SIGNAL(rightClicked(const QPoint &, int,int)),
            this, SLOT(listSgNodes(const QPoint &, int,int)));
    

    //lay->addWidget( m_markerWidget );
    lay->addWidget( m_lineNumberWidget );
    lay->addWidget( m_levelWidget );
    lay->addWidget( m_editor );
     
    // tab bar
    m_tabbar = new QTabBar(this);
    m_tabbar->addTab(new QTab());
    
    // popup menu
    m_popup = new QPopupMenu (this);
    connect(m_popup, SIGNAL(activated(int)),
	    this, SLOT(broadcastTargetNode(int)));
    
    // status bar
    m_statusbar = new QStatusBar(this);
    m_statusbar->setSizeGripEnabled(false);
    QLabel* label = new QLabel( m_statusbar );
    m_statusbar->addWidget(label, 1, true);
    connect(m_statusbar, SIGNAL(messageChanged(const QString&)),
 	   label, SLOT(setText(const QString&)));    
    
    // toolbar
    m_wgToolbar = new QRToolBar(this);
    m_wgToolbar->insertItem("cache", tb_cache_view, true);
    m_wgToolbar->insertItem("clear", tb_clear, false);    
    connect(m_wgToolbar,SIGNAL(clicked(int, bool)),this,SLOT(toolbarClicked(int,bool)));
    
    QVBoxLayout *layv = new QVBoxLayout(this);
    layv->addWidget(m_wgToolbar);
    layv->addWidget(m_tabbar);            
    layv->addLayout(lay);
    layv->addWidget(m_statusbar);

    m_editor->setReadOnly(TRUE);   
    m_ast_coords = NULL;
    m_wgToolbar->setState(tb_cache_view,false);
 
    doRepaint();

   if (QRGUI::autoGroup()) {
       QRGUI::currentGroup()->attach(this);
   }
   
   m_accumEnabled = true;


}

void QRCodeBox::loadCache() {
   m_ast_coords = NULL;
   set<SgNode *> *nodes = getCache();
   string code;
   for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
      SgNode *node = *iter;
      code += node->unparseToCompleteString() + "\n\n";
   }
   m_editor->setText(code);
   doRepaint();
}

void QRCodeBox::toolbarClicked(int id , bool isOn) {
    if (id == tb_cache_view) {
	delete m_ast_coords; 
	if (isOn) {
	    loadCache();	    	    	    
	} else {
	    m_ast_coords = new QRAstCoords();
            SgFile &file = ((SgProject *) QRGUI::get_root())->get_file(0);
            string code = QRUnparseToString(&file, m_ast_coords);
            m_editor->setText(code);
            char *ptr = file.getFileName(); char *aux = ptr;
            ptr += strlen(ptr);
            while (ptr != aux && *ptr != '/') ptr--;
            if (*ptr == '/') ptr++;
            m_tabbar->tabAt(0)->setText(ptr);
	    doRepaint();
	}
	
    } else if (id == tb_clear) {
	clearCache();
    }
}

void QRCodeBox::eventCacheModified() {
    if (m_wgToolbar->getState(tb_cache_view)) {
	loadCache();
    }
}

void QRCodeBox::internalExpandBlock( QTextParagraph* p )
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

void QRCodeBox::internalCollapseBlock( QTextParagraph* p )
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

	    // kdDebug(9032) << "hide parag " << p->paragId() << " level = " << data->level() << endl;
            p->hide();

            p = p->next();
        }
    }
}

void QRCodeBox::doRepaint()
{
   // m_markerWidget->doRepaint();
    m_lineNumberWidget->doRepaint();
    m_levelWidget->doRepaint();
}

void QRCodeBox::expandBlock( QTextParagraph* p )
{
    internalExpandBlock( p );

    m_editor->setCursorPosition( p->paragId(), 0 );
    m_editor->refresh();
    doRepaint();
}

void QRCodeBox::collapseBlock( QTextParagraph* p )
{
    internalCollapseBlock( p );

    m_editor->setCursorPosition( p->paragId(), 0 );
    m_editor->refresh();
    doRepaint();
}	

void QRCodeBox::receiveMsg(QRGroupWidget *emitter, QRMessage *msg) {
    switch (msg->id()) {
	case msg_node:
	   {
   	      SgNode *node = ((SgNodeMsg *) msg)->getNode();	    	    
	      if (!m_ast_coords) { insertNodeInCache(node); return; }
	      if (!m_ast_coords->node_has_multi_coord(node)) {
		  TypeNodeCoords::iterator iter = m_ast_coords->node_coords()->find(node);
		  if (iter != m_ast_coords->node_coords()->end()) {
		      QRCoordBox *box = iter->second;
		      m_editor->doSelectCode(box->line0(), box->col0(), 
					     box->line1(), box->col1());
		  } else {
		      printf("cannot find this node!\n");
		  }
	      } 	      
           }
	   break;
	case msg_cbox:
	   {
	      if(!m_ast_coords) return;
	      QRCoordBox *box = ((CBoxMsg *) msg)->getBox();	      
	      SgNode *node = box->getNode();
	      if (m_ast_coords->node_has_multi_coord(node)) {
	         m_editor->doSelectCode(box->line0(), box->col0(), 
		   		        box->line1(), box->col1());		  
	      }
	  }
	  break;
   }

 
}

void QRCodeBox::listSgNodes(const QPoint &point, int line, int col) {    
    if (!m_ast_coords) return;
    vector<QRCoordBox *> &boxes = *m_ast_coords->get_target_boxes(line, col);
    m_popup->clear();
    unsigned size = boxes.size();
    for (int i = size - 1; i >= 0; i--) {
	SgNode *node = boxes[i]->getNode();
	QPixmap *pixmap = QRAstInfo::get_pixmap(node);
	string test;
	test = boxes[i]->str();
	if (!pixmap) {	
   	   //m_popup->insertItem(QRAstInfo::get_info(node));
	    m_popup->insertItem(test, i);
        } else {
	   QIconSet *iconset = new QIconSet(*pixmap);
	   //m_popup->insertItem(*iconset, QRAstInfo::get_info(node));
	   m_popup->insertItem(*iconset, test, i);
	   
       }
    }
    m_popup->exec(point, 0);
    
}

void QRCodeBox::broadcastTargetNode(int id) {
    vector<QRCoordBox *> &boxes = *m_ast_coords->get_stored_targets();    
    ROSE_ASSERT((id >= 0) && (id < boxes.size()));
    
    QRCoordBox *box = boxes[id];
    broadcastNode(box->getNode());
    CBoxMsg boxMsg(box);
    broadcastMsg(&boxMsg);    
}

#include "QRCodeBox.moc"
