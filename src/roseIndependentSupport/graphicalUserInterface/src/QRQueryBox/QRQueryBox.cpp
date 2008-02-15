/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QRQueryBox.h>
#include <QRGui.h>
#include <config.h>
#include <rose.h>
#include <QRAstInfo.h>
#include <QRPixmap.h>

#include <qpushbutton.h>
#include <qframe.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qstyle.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qprogressbar.h>
#include <qapplication.h>

#include <set>
#include <map>

using namespace std;

QRQueryBox::QRQueryBox(const char *name):
    QWidget((QWidget *) QRGUI::current_panel_frame(), name) 
{
    init();
}
     

QRQueryBox::QRQueryBox(QWidget *parent, const char *name):
   QWidget(parent, name) 
{     
    init();
}

static void queryProgress(bool init, unsigned count, void *arg) {
    QRQueryBox *queryBox = (QRQueryBox *) arg;
    if (init) {
        queryBox->getWidgetRangeInfo()->setText("Querying the AST...");   	
	queryBox->getWidgetRangeQueryProgress()->setTotalSteps(count);
	queryBox->getWidgetRangeQueryProgress()->setProgress(0);	
    } else {
	queryBox->getWidgetRangeQueryProgress()->setProgress(count);	
    }
    QRGUI::app()->processEvents();    
}
    
void QRQueryBox::init() {
    m_wgMasterLayout = new QVBoxLayout(this, 5, 15); 

    m_wgTopFrame = new QFrame( this, "m_wgTopFrame" );
    m_wgTopFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, m_wgTopFrame->sizePolicy().hasHeightForWidth() ) );
    m_wgTopFrame->setFrameShape( QFrame::NoFrame );
    m_wgTopFrame->setFrameShadow( QFrame::Raised );
    m_wgTopFrameLayout = new QVBoxLayout( m_wgTopFrame, 0, 0, "m_wgTopFrameLayout"); 

    m_wgDomainLabel = new QLabel( m_wgTopFrame, "m_wgDomainLabel1" );
    m_wgDomainLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, m_wgDomainLabel->sizePolicy().hasHeightForWidth() ) );
    m_wgDomainLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignBottom | QLabel::AlignLeft ) );
    m_wgTopFrameLayout->addWidget( m_wgDomainLabel );

    m_wgDomainFrame = new QFrame( m_wgTopFrame, "m_wgDomainFrame" );
    m_wgDomainFrame->setFrameShape( QFrame::GroupBoxPanel );
    m_wgDomainFrame->setFrameShadow( QFrame::Raised );
    m_wgDomainFrame->setLineWidth( 1 );
    m_wgDomainFrameLayout = new QVBoxLayout( m_wgDomainFrame, 11, 6, "m_wgDomainFrameLayout"); 
 
    m_wgDomainLayout1 = new QHBoxLayout( 0, 0, 6, "m_wgDomainLayout1"); 

    m_wgDomainView = new QComboBox( FALSE, m_wgDomainFrame, "m_wgDomainView" );
    m_wgDomainView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 10, 0, m_wgDomainView->sizePolicy().hasHeightForWidth() ) );
    m_wgDomainView->setMinimumHeight(30);    
    QPalette cbox_palette(m_wgDomainView->palette());
    cbox_palette.setColor(QPalette::Normal, QColorGroup::Highlight,cbox_palette.color(QPalette::Normal,QColorGroup::Base));    
    cbox_palette.setColor(QPalette::Normal, QColorGroup::HighlightedText,cbox_palette.color(QPalette::Normal,QColorGroup::Text));        
    m_wgDomainView->setPalette(cbox_palette);
    m_wgDomainLayout1->addWidget( m_wgDomainView );

    m_wgDomainInfo = new QLabel( m_wgDomainFrame, "m_wgDomainInfo" );
    m_wgDomainInfo->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 3, 0, m_wgDomainInfo->sizePolicy().hasHeightForWidth() ) );
    m_wgDomainInfo->setFrameShape( QLabel::StyledPanel );
    m_wgDomainInfo->setFrameShadow( QLabel::Sunken );
    m_wgDomainInfo->setLineWidth( 1 );
    m_wgDomainInfo->setScaledContents( TRUE );
    m_wgDomainLayout1->addWidget( m_wgDomainInfo );
    m_wgDomainFrameLayout->addLayout( m_wgDomainLayout1 );

    m_wgDomainLayout2 = new QHBoxLayout( 0, 0, 6, "m_wgDomainLayout2"); 

    m_wgDomainExpModes = new QButtonGroup( m_wgDomainFrame, "m_wgDomainExpModes" );
    m_wgDomainExpModes->setFrameShape( QButtonGroup::NoFrame );
    m_wgDomainExpModes->setAlignment( int( QButtonGroup::AlignVCenter | QButtonGroup::AlignLeft ) );
    m_wgDomainExpModes->setColumnLayout(0, Qt::Vertical );
    m_wgDomainExpModes->layout()->setSpacing( 4 );
    m_wgDomainExpModes->layout()->setMargin( 10 );
    m_wgDomainExpModesLayout = new QHBoxLayout( m_wgDomainExpModes->layout() );
    m_wgDomainExpModesLayout->setAlignment( Qt::AlignCenter );

    m_wgDomainNoExpansion = new QRadioButton( m_wgDomainExpModes, "m_wgDomainNoExpansion" );
    m_wgDomainNoExpansion->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_wgDomainNoExpansion->sizePolicy().hasHeightForWidth() ) );
    m_wgDomainExpModesLayout->addWidget( m_wgDomainNoExpansion );

    m_wgDomainImmedChildren = new QRadioButton( m_wgDomainExpModes, "m_wgDomainImmedChildren" );
    m_wgDomainExpModesLayout->addWidget( m_wgDomainImmedChildren );

    m_wgDomainAllDesc = new QRadioButton( m_wgDomainExpModes, "m_wgDomainAllDesc" );
    m_wgDomainExpModesLayout->addWidget( m_wgDomainAllDesc );
    m_wgDomainLayout2->addWidget( m_wgDomainExpModes );
    
    m_wgDomainSpacer = new QSpacerItem( 40, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );    
    m_wgDomainLayout2->addItem( m_wgDomainSpacer );

    m_wgDomainAdd = new QPushButton( m_wgDomainFrame, "m_wgDomainAdd" );
    m_wgDomainAdd->setPaletteBackgroundColor( QColor( 196, 208, 230 ) );
    m_wgDomainAdd->setToggleButton( TRUE );
    m_wgDomainAdd->setFlat( FALSE );
    m_wgDomainLayout2->addWidget( m_wgDomainAdd );

    m_wgDomainClear = new QPushButton( m_wgDomainFrame, "m_wgDomainClear" );
    m_wgDomainClear->setPaletteBackgroundColor( QColor( 196, 208, 230 ) );
    m_wgDomainClear->setFlat( FALSE );
    m_wgDomainLayout2->addWidget( m_wgDomainClear );

    m_wgDomainSetDefault = new QPushButton( m_wgDomainFrame, "m_wgDomainDefRoot" );
    m_wgDomainSetDefault->setPaletteBackgroundColor( QColor( 196, 208, 230 ) );
    m_wgDomainSetDefault->setFlat( FALSE );
    m_wgDomainLayout2->addWidget( m_wgDomainSetDefault );
    m_wgDomainFrameLayout->addLayout( m_wgDomainLayout2 );
    m_wgTopFrameLayout->addWidget( m_wgDomainFrame );
    m_wgMasterLayout->addWidget( m_wgTopFrame );

    m_wgBtmFrame = new QFrame( this, "m_wgBtmFrame" );
    m_wgBtmFrame->setFrameShape( QFrame::NoFrame ); 
    m_wgBtmFrame->setFrameShadow( QFrame::Raised );
    m_wgBtmFrameLayout = new QHBoxLayout( m_wgBtmFrame, 0, 15, "m_wgBtmFrameLayout"); 

    m_wgBtmLeftFrame = new QFrame( m_wgBtmFrame, "m_wgBtmLeftFrame" );
    m_wgBtmLeftFrame->setFrameShape( QFrame::NoFrame );
    m_wgBtmLeftFrame->setFrameShadow( QFrame::Raised );
    m_wgBtmLeftFrameLayout = new QVBoxLayout( m_wgBtmLeftFrame, 0, 0, "m_wgBtmLeftFrameLayout"); 

    m_wgOpLabel1 = new QLabel( m_wgBtmLeftFrame, "m_wgOpLabel1" );
    m_wgBtmLeftFrameLayout->addWidget( m_wgOpLabel1 );

    m_wgOpFrame = new QFrame( m_wgBtmLeftFrame, "m_wgOpFrame" );
    m_wgOpFrame->setFrameShape( QFrame::GroupBoxPanel );
    m_wgOpFrame->setFrameShadow( QFrame::Raised );
    m_wgOpFrameLayout = new QVBoxLayout( m_wgOpFrame, 11, 6, "m_wgOpFrameLayout"); 

    m_wgOpCustomQueries = new QPushButton( m_wgOpFrame, "m_wgOpCustomQueries" );
    m_wgOpCustomQueries->setPaletteBackgroundColor( QColor( 224, 215, 234 ) );
    m_wgOpCustomQueries->setToggleButton( TRUE );
    m_wgOpCustomQueries->setFlat( FALSE );
    m_wgOpFrameLayout->addWidget( m_wgOpCustomQueries );

    m_wgOpVariantQueryDesc = new QComboBox( FALSE, m_wgOpFrame, "m_wgOpVariantQueryDesc" );
    m_wgOpFrameLayout->addWidget( m_wgOpVariantQueryDesc );
    m_wgOpSpacer = new QSpacerItem( 21, 16, QSizePolicy::Minimum, QSizePolicy::Preferred );
    m_wgOpFrameLayout->addItem( m_wgOpSpacer );

    m_wgOpQueryList = new QListView( m_wgOpFrame, "m_wgOpQueryList" );
    m_wgOpQueryList->header()->hide();
    m_wgOpQueryList->addColumn("Node Variants");
    m_wgOpQueryList->setColumnAlignment( 1, Qt::AlignRight );    
    m_wgOpQueryList->setRootIsDecorated(true);
    m_wgOpQueryList->setSelectionMode(QListView::NoSelection); 
    
    m_wgOpFrameLayout->addWidget( m_wgOpQueryList );    

    m_wgOpPerformQuery = new QPushButton( m_wgOpFrame, "m_wgOpPerformQuery" );
    m_wgOpPerformQuery->setPaletteBackgroundColor( QColor( 179, 200, 181 ) );
    m_wgOpPerformQuery->setFlat( FALSE );
    m_wgOpFrameLayout->addWidget( m_wgOpPerformQuery );
    m_wgBtmLeftFrameLayout->addWidget( m_wgOpFrame );
    m_wgBtmFrameLayout->addWidget( m_wgBtmLeftFrame );

    m_wgBtmRightFrame = new QFrame( m_wgBtmFrame, "m_wgBtmRightFrame" );
    m_wgBtmRightFrame->setFrameShape( QFrame::NoFrame );
    m_wgBtmRightFrame->setFrameShadow( QFrame::Raised );
    m_wgBtmRightFrameLayout = new QVBoxLayout( m_wgBtmRightFrame, 0, 0, "m_wgBtmRightFrameLayout"); 

    m_wgRangeLabel1 = new QLabel( m_wgBtmRightFrame, "m_wgRangeLabel1" );
    m_wgBtmRightFrameLayout->addWidget( m_wgRangeLabel1 );

    m_wgRangeFrame = new QFrame( m_wgBtmRightFrame, "m_wgRangeFrame" );
    m_wgRangeFrame->setFrameShape( QFrame::GroupBoxPanel );
    m_wgRangeFrame->setFrameShadow( QFrame::Raised );
    m_wgRangeFrameLayout = new QVBoxLayout( m_wgRangeFrame, 11, 6, "m_wgRangeFrameLayout"); 

    m_wgRangeInfo = new QLabel( m_wgRangeFrame, "m_wgRangeInfo" );
    m_wgRangeInfo->setFrameShape( QLabel::StyledPanel );
    m_wgRangeInfo->setFrameShadow( QLabel::Sunken );
    m_wgRangeInfo->setLineWidth( 1 );
    m_wgRangeInfo->setScaledContents( TRUE );
    m_wgRangeFrameLayout->addWidget( m_wgRangeInfo );
    
    m_wgRangeQueryProgress = new QProgressBar(m_wgRangeFrame, "m_wgRangeQueryProgress");
    m_wgRangeQueryProgress->setFrameShape( QLabel::StyledPanel );
    m_wgRangeQueryProgress->setFrameShadow( QLabel::Sunken );
    m_wgRangeQueryProgress->setLineWidth( 1 );
    m_wgRangeQueryProgress->setHidden(true);    
    m_wgRangeFrameLayout->addWidget(m_wgRangeQueryProgress);

    m_wgRangeRes = new QListView( m_wgRangeFrame, "m_wgRangeRes" );
    m_wgRangeRes->addColumn("node");
    m_wgRangeRes->addColumn("info");
    m_wgRangeRes->setSortColumn(m_wgRangeRes->columns()+1);
    m_wgRangeRes->setSelectionMode(QListView::Extended);
    m_wgRangeFrameLayout->addWidget( m_wgRangeRes );

    m_wgRangeButtonLayout = new QHBoxLayout( 0, 0, 5, "m_wgRangeButtonLayout"); 

    m_wgRangeBroadcast = new QPushButton( m_wgRangeFrame, "m_wgRangeBroadcast" );
    m_wgRangeBroadcast->setPaletteBackgroundColor( QColor( 216, 216, 216 ) );
    m_wgRangeBroadcast->setFlat( FALSE );
    m_wgRangeButtonLayout->addWidget( m_wgRangeBroadcast );

    m_wgRangeSetDomain = new QPushButton( m_wgRangeFrame, "m_wgRangeSetDomain" );
    m_wgRangeSetDomain->setPaletteBackgroundColor( QColor( 216, 216, 216 ) );
    m_wgRangeSetDomain->setFlat( FALSE );
    m_wgRangeButtonLayout->addWidget( m_wgRangeSetDomain );

    m_wgRangeClear = new QPushButton( m_wgRangeFrame, "m_wgRangeClear" );
    m_wgRangeClear->setPaletteBackgroundColor( QColor( 216, 216, 216 ) );
    m_wgRangeClear->setFlat( FALSE );
    m_wgRangeButtonLayout->addWidget( m_wgRangeClear );
    m_wgRangeFrameLayout->addLayout( m_wgRangeButtonLayout );
    m_wgBtmRightFrameLayout->addWidget( m_wgRangeFrame );
    m_wgBtmFrameLayout->addWidget( m_wgBtmRightFrame );
    m_wgMasterLayout->addWidget( m_wgBtmFrame );
    resize( QSize(726, 431).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
    
    m_wgDomainLabel->setText( tr( "<b>Query Domain</b>" ) );
    m_wgDomainAdd->setText( tr( "add" ) );
    m_wgDomainClear->setText( tr( "clear" ) );
    m_wgDomainSetDefault->setText( tr( "set default" ) );
    m_wgOpLabel1->setText( tr( "<b>Query Operator</b>" ) );
    m_wgOpCustomQueries->setText( tr( "custom queries" ) );
    m_wgOpPerformQuery->setText( tr( "perform query" ) );
    m_wgRangeLabel1->setText( tr( "<b>Query Range</b>" ) );
    m_wgRangeBroadcast->setText( tr( "broadcast" ) );
    m_wgRangeSetDomain->setText( tr( "set domain" ) );
    m_wgRangeClear->setText( tr( "clear" ) );    
    m_wgDomainNoExpansion->setText( tr( "no expansion" ) );
    m_wgDomainImmedChildren->setText( tr( "immediate children" ) );
    m_wgDomainAllDesc->setText( tr( "all descendants" ) );    
            
    // ===================[connections]
    
    // get into current group, if auto-grouping is on.
    if (QRGUI::autoGroup()) {
	QRGUI::currentGroup()->attach(this);
    }
    
    // domain
    connect(m_wgDomainView,SIGNAL(activated(int)),this,SLOT(domainViewClick(int)));
    connect(m_wgDomainAdd,SIGNAL(toggled(bool)),this,SLOT(domainAdd(bool)));
    connect(m_wgDomainSetDefault,SIGNAL(clicked()),this,SLOT(domainSetDefault()));
    connect(m_wgDomainClear,SIGNAL(clicked()),this,SLOT(domainClear()));
    connect(m_wgDomainExpModes,SIGNAL(clicked(int)),this,SLOT(domainSetExpMode(int)));
    
    
    // op
    connect(m_wgOpPerformQuery,SIGNAL(clicked()),this,SLOT(opPerformQuery()));
    connect(m_wgOpCustomQueries,SIGNAL(toggled(bool)),this,SLOT(opLoadQueryList(bool)));
    connect(m_wgOpVariantQueryDesc,SIGNAL(activated(int)),this,SLOT(opSyncVariantQueryWithDesc(int)));
    connect(m_wgOpQueryList,SIGNAL(clicked(QListViewItem *, const QPoint &, int)),this,SLOT(opQueryListClick(QListViewItem*, const QPoint&, int)));
     
    // range
    connect(this,SIGNAL(newQueryResults()),SLOT(rangeShowResults()));
    connect(m_wgRangeRes,SIGNAL(doubleClicked(QListViewItem *,const QPoint&,int)),this,
	                 SLOT(rangeResDoubleClick(QListViewItem *,const QPoint&,int)));
    connect(m_wgRangeBroadcast,SIGNAL(clicked()),this,SLOT(rangeBroadcast()));
    connect(m_wgRangeSetDomain,SIGNAL(clicked()),this,SLOT(rangeSetDomain()));
    connect(m_wgRangeClear,SIGNAL(clicked()),this,SLOT(rangeClear()));
    //=====================[ some extra initialization code ]=======
    setCacheAccumEnabled(true);
    reset();
}  

QRQueryBox::~QRQueryBox() {
    clearCustomQueries();
}

void QRQueryBox::reset() {
    m_rootNode = QRGUI::get_root();  
    opLoadBuiltInVariantQueries();
    opLoadVariantViewDesc();
    domainSetDefault();  
    domainAdd(false);
    domainSetExpMode(QRQueryDomain::all_children);
    opLoadQueryList(false);
    m_wgOpCustomQueries->setHidden(m_customQueries.empty());  
    m_queryDomain.clear();    
    m_queryRange.clear(); emit newQueryResults();
}

void QRQueryBox::clearCustomQueries() {
    for (TypeCustomQueries::iterator iter = m_customQueries.begin(); 
         iter != m_customQueries.end(); iter++) 
    {
	delete iter->second.second;
    }
    m_customQueries.clear();
    m_wgOpCustomQueries->setHidden(true);        
}


void QRQueryBox::clearVariantQueries() {
    m_variantQueries.clear();
}


QRQueryBox::T_VariantNode::T_VariantNode(VariantT variant, T_VariantNode *parent) {
    m_variant = variant; setParent(parent);
}

void QRQueryBox::T_VariantNode::setParent(T_VariantNode *parent) {
    m_parent = parent;
    if (parent) { 
	parent->m_children.insert(this);
    }
}
    
void QRQueryBox::T_VariantNode::removeParent() {
   if (m_parent) {
       m_parent->m_children.erase(this);
       m_parent = NULL;
   }
}

unsigned QRQueryBox::T_VariantNode::getDepth() {
    unsigned depth = 0;
    T_VariantNode *parent = m_parent;
    while (parent) {
	parent = parent->m_parent;
	depth++;
    }
    return depth;
}
   
QRQueryBox::T_VariantNode::~T_VariantNode() {
    for (set<T_VariantNode *>::iterator iter = m_children.begin(); iter != m_children.end(); iter++) {
	delete *iter;
    }
}

void QRQueryBox::opLoadVariantQueryList() {	
    T_VariantNode root_node(V_SgNumVariants, NULL); 
    NodeQuery::VariantVector vvector;
    map<VariantT, T_VariantNode *> variant_nodes;
    for (int v = 0; v < V_SgNumVariants; v++) {
	VariantT variant;
	variant = (VariantT) v;
        T_VariantNode *node;	
	map<VariantT, T_VariantNode *>::iterator iter = variant_nodes.find(variant);
	if (iter == variant_nodes.end()) {
	    node = new T_VariantNode(variant, &root_node); 
	    variant_nodes[variant] = node;
	} else {
	    node = iter->second;
	}
	unsigned node_depth; node_depth = node->getDepth();
	vvector = NodeQuery::VariantVector(variant);	
	T_VariantNode *child_node;
	VariantT child_variant;
	
	for (int i = 1; i < vvector.size(); i++) {
	    child_variant = vvector[i];
	    iter = variant_nodes.find(child_variant);
	    if (iter == variant_nodes.end()) {
		child_node = new T_VariantNode(child_variant, node);	
		variant_nodes[child_variant] = child_node;
	    } else {
		child_node = iter->second;
		if (node_depth > child_node->m_parent->getDepth()) {
		    child_node->removeParent();
		    child_node->setParent(node);	
		}		
	    }
	} 
    }
      
    m_mapOpVariantItem.clear();
    m_mapOpItemVariant.clear();    
    opLoadVariantQueryListAux(root_node, NULL, NULL);    
}
    

QCheckListItem* QRQueryBox::opLoadVariantQueryListAux(T_VariantNode &node, QCheckListItem *ancestor, QCheckListItem *last_sibling) {
    QCheckListItem *item = NULL;
    
    if (!node.isRoot()) {
       if (ancestor) {
          item = new QCheckListItem(ancestor, last_sibling, getVariantName(node.m_variant), QCheckListItem::CheckBoxController);
       } else {
          item = new QCheckListItem(m_wgOpQueryList, last_sibling, getVariantName(node.m_variant), QCheckListItem::CheckBoxController);
       }
   } 
   m_mapOpVariantItem[node.m_variant] = item;
   m_mapOpItemVariant[item] = node.m_variant;
   last_sibling = NULL;
   for (set<T_VariantNode *>::iterator iter = node.m_children.begin();
        iter != node.m_children.end(); iter++)
   {
       last_sibling = opLoadVariantQueryListAux(**iter, item, last_sibling); 	
   }
   
   return item;
    
}
    
// use default root
void QRQueryBox::domainSetDefault() {
    clearCache(false);
    if (QRGUI::get_root()) {
       insertNodeInCache(QRGUI::get_root());        
   }
}

// cache modified: need to update DomainCBox and DomainInfo
void QRQueryBox::eventCacheModified() {
   int cache_size = cacheSize();

   if (cache_size == 0) {
       m_wgDomainInfo->setText("<empty>");
       m_wgDomainExpModes->setEnabled(false);
       m_wgOpPerformQuery->setEnabled(false);
   } else {
       m_wgDomainExpModes->setEnabled(true);       
       m_wgOpPerformQuery->setEnabled(true);       
       if (cache_size == 1) {
          m_wgDomainInfo->setText("1 node");
       } else {
          m_wgDomainInfo->setText(QString("%1 nodes").arg(cacheSize()));
       }
   }
   
   m_wgDomainView->clear();
   int count = 0;
       
   for (set<SgNode *>::iterator iter = m_cache.begin(); iter != m_cache.end(); iter++) {              
       QPixmap *pixmap;
       SgNode *node = *iter;
       pixmap = QRAstInfo::get_pixmap(node);
       if (!pixmap) pixmap = QRPixmap::get_pixmap(icons::empty);
       if (cache_size == 1) {
           m_wgDomainView->insertItem(*pixmap, QString("%2").arg(QRAstInfo::get_info(node)));
       } else {	   
          m_wgDomainView->insertItem(*pixmap, QString("[%1.] %2").arg(++count).arg(QRAstInfo::get_info(node)));
       }
   }
   m_domainNeedsToExpand = true;
}

bool QRQueryBox::isDomainAdd() {
    return m_wgDomainAdd->isOn();    
}

void QRQueryBox::domainAdd(bool isOn) {
    m_wgDomainAdd->setOn(isOn);   
    setCacheAutoInsert(isOn);    
}

// clear domain
void QRQueryBox::domainClear() {
    clearCache();
} 


void QRQueryBox::domainViewClick(int id) {
   if (id != 0)  {
       m_wgDomainView->setCurrentItem(0);
   }    
}


NodeQuery::VariantVector QRQueryBox::getOpVariantVectorFromList() {
    NodeQuery::VariantVector vector;
    
    ROSE_ASSERT(!isOpCustomQueries());
    
    if (isOpCustomQueries()) return vector;
    
    QCheckListItem *item = (QCheckListItem *) m_wgOpQueryList->firstChild();    
    while(item) {
	getOpVariantVectorFromListAux(item,vector);
	item = (QCheckListItem *) item->nextSibling();
    }
    
    return vector;
}

void QRQueryBox::getOpVariantVectorFromListAux(QCheckListItem *item, NodeQuery::VariantVector &vector) {
    if (item->state() == QCheckListItem::NoChange) {	
       QCheckListItem *child_item = (QCheckListItem *) item->firstChild();       
       while (child_item) {
          getOpVariantVectorFromListAux(child_item, vector);
	  child_item = (QCheckListItem *) child_item->nextSibling();
      }
    } else if (item->isOn()) {
       vector = vector + NodeQuery::VariantVector(m_mapOpItemVariant[item]);
    }
}

QRQueryOperator* QRQueryBox::getOpCustomQueryFromList() {
    ROSE_ASSERT(isOpCustomQueries());
    
     map<unsigned,QCheckListItem*>::iterator iter = m_mapOpCustomQueryItem.begin();
    
    while ((iter != m_mapOpCustomQueryItem.end()) && (!iter->second->isOn())) {
	iter++;
    }
    
    ROSE_ASSERT(iter != m_mapOpCustomQueryItem.end());
        
    return m_customQueries[iter->first].second;        
}
    

void QRQueryBox::opLoadBuiltInVariantQueries() {
   clearVariantQueries();


}

void QRQueryBox::opLoadVariantViewDesc() {
   m_wgOpVariantQueryDesc->clear();
   m_wgOpVariantQueryDesc->insertItem("<user-defined variant query>"); 
   for (TypeVariantQueries::iterator iter = m_variantQueries.begin(); iter != m_variantQueries.end(); iter++) {
       m_wgOpVariantQueryDesc->insertItem(iter->second.first);   
   }    
}

void QRQueryBox::insertVariantQuery(string desc, const NodeQuery::VariantVector &v) {
    m_variantQueries[m_variantQueries.size()] = pair<string,NodeQuery::VariantVector>(desc, v);
    opLoadVariantViewDesc();    
}

void QRQueryBox::insertCustomQuery(string desc, QRQueryOperator *op) {
    m_customQueries[m_customQueries.size()] = pair<string,QRQueryOperator *>(desc, op);
    m_wgOpCustomQueries->setHidden(false);        
}


void QRQueryBox::opPerformQuery() {
    QRQueryOperator *op = NULL;
    bool deleteOp = false;
    
    if (isOpCustomQueries()) {
	// get custom query
	op = getOpCustomQueryFromList();
    } else {	
	// build variant query
	NodeQuery::VariantVector variantVector = getOpVariantVectorFromList();
	if (!variantVector.empty()) {
       	   op = new QRQueryOpVariant(variantVector);
	   deleteOp = true;
       }
    }
    
    if (op) { 	  
       if (m_domainNeedsToExpand) {
          m_wgRangeInfo->setText("Expanding domain... please wait");   	
	  QRGUI::app()->processEvents();
          m_queryDomain.expand(getCache(), m_domainExpMode);  
          m_domainNeedsToExpand = false;
       }	
       m_wgRangeQueryProgress->setHidden(false); 	
       op->setProgressFunc(queryProgress,this);
       op->performQuery(&m_queryDomain, &m_queryRange);    
       emit newQueryResults();
       m_wgRangeQueryProgress->setHidden(true);          
    } 

    if (deleteOp) delete op;	        
    
}    

void QRQueryBox::opLoadCustomQueryList() {
    // make sure we are on the custom query view mode
    if (!isOpCustomQueries()) return    
    m_mapOpCustomQueryItem.clear();
    if (m_customQueries.empty()) return;    
    
    QCheckListItem *rootItem = new QCheckListItem(m_wgOpQueryList, "Custom Queries",QCheckListItem::RadioButtonController);
    QCheckListItem *lastSibling = NULL;
    for (TypeCustomQueries::iterator iter = m_customQueries.begin(); 
         iter != m_customQueries.end(); iter++)
    {
	string &desc = iter->second.first;
	lastSibling =  new QCheckListItem(rootItem,lastSibling, desc,QCheckListItem::RadioButton);
	m_mapOpCustomQueryItem[iter->first] = lastSibling;
    } 
    m_wgOpQueryList->setOpen(rootItem, true);    
    m_mapOpCustomQueryItem[0]->setOn(true);    
}
 
void QRQueryBox::opLoadQueryList(bool isOn) {
    m_wgOpVariantQueryDesc->setEnabled(!isOn);
    m_wgOpQueryList->clear();  
    m_wgOpQueryList->setRootIsDecorated(!isOn);
   if (isOn) {
        m_wgOpQueryList->setSorting(-1);
	opLoadCustomQueryList();
    } else {
        m_wgOpQueryList->setSorting(0);	
        opLoadVariantQueryList();
    } 
    m_wgOpCustomQueries->setOn(isOn);    
}

bool QRQueryBox::isOpCustomQueries() {
    return m_wgOpCustomQueries->isOn();
}
    

void QRQueryBox::opSyncVariantQueryWithDesc(int id) {
   if (id != 0) {
       NodeQuery::VariantVector &vector = m_variantQueries[id-1].second;
       setOpListFromVariantVector(vector);
   }
}

void QRQueryBox::setOpListFromVariantVector(const NodeQuery::VariantVector &vector) {
    // make sure we are on the variant view mode
    if (isOpCustomQueries()) return;
    
    // uncheck all items (children are unchecked automatically as you uncheck the root items)
    QCheckListItem *item = (QCheckListItem *) m_wgOpQueryList->firstChild();    
    while(item) {
	item->setOn(false);
	item = (QCheckListItem *) item->nextSibling();	
    }
    
    // now check(set) each variant 
    for (NodeQuery::VariantVector::const_iterator iter = vector.begin(); iter != vector.end(); iter++) {
	const VariantT variant = *iter;
	QCheckListItem *item = m_mapOpVariantItem[variant];
	item->setOn(true);	
    }  
}


void QRQueryBox::opSyncDescWithVariantQuery() {
    if (isOpCustomQueries()) return;
    
    // get variant vector from the list
    NodeQuery::VariantVector vector = getOpVariantVectorFromList();
    
    bool query_match = false;
    unsigned matchedId = 0;
    // traverse the list of builtin queries
    if (vector.size() > 0) {
	TypeVariantQueries::iterator iter = m_variantQueries.begin();    
	while (iter != m_variantQueries.end() && !query_match) {
	    pair<string,NodeQuery::VariantVector> &item = iter->second;
	    NodeQuery::VariantVector &queryVector = item.second;	
	    if (vector.size() == queryVector.size()) {
  	        bool differentVectors; differentVectors = false;		
		NodeQuery::VariantVector::iterator iter1 = vector.begin();
		while (iter1 != vector.end() && !differentVectors) {
		    NodeQuery::VariantVector::iterator iter2 = queryVector.begin();		
		    bool foundVariant; foundVariant = false;
		    while (iter2 != queryVector.end() && !foundVariant) {
			if (*iter1 == *iter2) foundVariant = true; else iter2++;
		    }
		    if (!foundVariant) differentVectors = true; else iter1++;
		}
		if (!differentVectors) {
		    query_match = true;
		    matchedId = iter->first;		
		}
	    }
	    iter++;
	}
    }
    
    if (query_match) {
	m_wgOpVariantQueryDesc->setCurrentItem(matchedId+1);
    } else {
	m_wgOpVariantQueryDesc->setCurrentItem(0);
    }
}

void QRQueryBox::opQueryListClick(QListViewItem *item, const QPoint &pnt, int c) {
    if (isOpCustomQueries()) return;
    if (item) {
       QListView *lv = item->listView();
       int checkBoxWidth = lv->style().pixelMetric(QStyle::PM_CheckListButtonSize, lv);
       QPoint topLeft = lv->itemRect( item ).topLeft(); //### inefficient?
       int xdepth = lv->treeStepSize() * (item->depth() + (lv->rootIsDecorated() ? 1 : 0))
		     + lv->itemMargin();
       xdepth += lv->header()->sectionPos( lv->header()->mapToSection( 0 ) );       
       QPoint itemPos = lv->viewport()->mapToGlobal( topLeft );
       if (pnt.x()-itemPos.x()-xdepth < checkBoxWidth) {
	   opSyncDescWithVariantQuery();
       }
    }
}

void QRQueryBox::domainSetExpMode(int id) {
   m_domainExpMode = (QRQueryDomain::TypeDomainExpansion) id;
   m_domainNeedsToExpand = true;
   m_wgDomainExpModes->setButton(id);
}

void QRQueryBox::domainSetExpMode(QRQueryDomain::TypeDomainExpansion id) {
    domainSetExpMode((int) id);
}

void QRQueryBox::rangeShowResults() {  
    m_wgRangeInfo->setText("Rendering results...");
    m_wgRangeRes->clear();    
    if (m_queryRange.countNodeStr() == 0) {
	m_wgRangeRes->hideColumn(1);
	m_wgRangeRes->header()->hide();
    } else {
	m_wgRangeRes->adjustColumn(1);
	m_wgRangeRes->header()->show();
    }
    
    set<SgNode *> *nodes = m_queryRange.getNodes();
    QListViewItem *lastSibling = NULL;
    
    m_mapRangeItemNode.clear();    
    m_wgRangeQueryProgress->setTotalSteps(nodes->size());
    QRGUI::app()->processEvents();
    int count = 0;
    for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
	SgNode *node = *iter;
	string desc;
	desc = QRAstInfo::get_info(node);	
	QPixmap *pixmap = QRAstInfo::get_pixmap(node);
	if (!pixmap) pixmap = QRPixmap::get_pixmap(icons::empty);
	lastSibling = new QListViewItem(m_wgRangeRes, lastSibling, desc);
	m_mapRangeItemNode[lastSibling] = node;
	lastSibling->setPixmap(0, *pixmap);
	string nodeStr;
	if (m_queryRange.getNodeStr(node, nodeStr)) {
	    lastSibling->setText(1, nodeStr);
	}
	m_wgRangeQueryProgress->setProgress(count);
	count++;
    }
    QString info = QString("total=%1, hits=%2").arg(m_queryDomain.countDomain())
                                     .arg(m_queryRange.countRange());
    
    m_wgRangeInfo->setText(info);    
}

void QRQueryBox::rangeResDoubleClick(QListViewItem *item, const QPoint&, int) {
    map<QListViewItem *,SgNode *>::iterator iter = m_mapRangeItemNode.find(item);
    ROSE_ASSERT(iter != m_mapRangeItemNode.end());
    SgNode *node = iter->second;
    broadcastNode(node);
}    

void QRQueryBox::rangeBroadcast() {
    set<SgNode *> allNodesToBroadcast;
    
    bool insertAllNodes = true;
    QListViewItem *item = m_wgRangeRes->firstChild();
    while (item) {
	bool itemIsSelected = m_wgRangeRes->isSelected(item);
	if (insertAllNodes || itemIsSelected) {
           map<QListViewItem *,SgNode *>::iterator iter = m_mapRangeItemNode.find(item);		
           ROSE_ASSERT(iter != m_mapRangeItemNode.end());		
   	   SgNode *node = iter->second;
	   if (itemIsSelected) {
	       insertAllNodes = false;
	       broadcastNode(node);	       
	   } else {
   	      if (insertAllNodes) allNodesToBroadcast.insert(node);
	  }
       }
       item = item->nextSibling();
    }
    /*
    if (insertAllNodes) {
	for (set<SgNode *>::iterator iter = allNodesToBroadcast.begin(); iter != allNodesToBroadcast.end(); iter++) {
	    broadcastNode(*iter);
	}
    } */
}
	   
void QRQueryBox::rangeSetDomain() {
    set<SgNode *> *nodes = m_queryRange.getNodes();
    clearCache(nodes->empty());
    for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); ) {
	SgNode *node = *iter; iter++;
	bool isLastItem = iter == nodes->end();
	insertNodeInCache(node, isLastItem);
   }
}
	
void QRQueryBox::rangeClear() {
    m_wgRangeRes->clear();    
    m_queryRange.clear();
    m_wgRangeRes->hideColumn(1);
    m_wgRangeRes->header()->hide();
    m_mapRangeItemNode.clear();         
}

#include "QRQueryBox.moc"

