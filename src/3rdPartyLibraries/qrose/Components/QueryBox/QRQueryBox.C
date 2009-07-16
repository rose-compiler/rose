/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <QRMain.h>
#include <QRQueryBox.h>
#include <QRAstInfo.h>
#include <QRPixmap.h>
#include <QRIcons.h>

#include <set>
#include <map>

using namespace std;

namespace qrs {

QRQueryBox::QRQueryBox(SgNode *node)
{
    init(node);
}

static void queryProgress(bool init, unsigned count, void *arg) {
    QRQueryBox *queryBox = (QRQueryBox *) arg;
    if (init) {
        queryBox->getWidgetRangeInfo()->setText(QROSE::format("Querying the AST... ",
           count, (count == 1)? "hit" : "hits").c_str());
	queryBox->getWidgetRangeQueryProgress()->setTotalSteps(count);
	queryBox->getWidgetRangeQueryProgress()->setProgress(0);
    } else {
	queryBox->getWidgetRangeQueryProgress()->setProgress(count);
        queryBox->getWidgetRangeInfo()->setText(QROSE::format("Querying the AST... [%d %s]",
           count, (count == 1)? "hit" : "hits").c_str());

    }
    QROSE::app()->processEvents();
}

void QRQueryBox::init(SgNode *node) {
    m_wgMasterLayout = new Q3VBoxLayout(this, 5, 15);
    m_rootNode = node;

    m_wgTopFrame = new Q3Frame( this, "m_wgTopFrame" );
    m_wgTopFrame->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)5, (QSizePolicy::Policy)0, 0, 0, m_wgTopFrame->sizePolicy().hasHeightForWidth() ) );
    m_wgTopFrame->setFrameShape( Q3Frame::NoFrame );
    m_wgTopFrame->setFrameShadow( Q3Frame::Raised );
    m_wgTopFrameLayout = new Q3VBoxLayout( m_wgTopFrame, 0, 0, "m_wgTopFrameLayout");

    m_wgDomainLabel = new QLabel;
    m_wgDomainLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, m_wgDomainLabel->sizePolicy().hasHeightForWidth() ) );
    m_wgDomainLabel->setAlignment( int( Qt::WordBreak | Qt::AlignBottom | Qt::AlignLeft ) );
    m_wgTopFrameLayout->addWidget( m_wgDomainLabel );

    m_wgDomainFrame = new Q3Frame( m_wgTopFrame, "m_wgDomainFrame" );
    m_wgDomainFrame->setFrameShape( Q3Frame::GroupBoxPanel );
    m_wgDomainFrame->setFrameShadow( Q3Frame::Raised );
    m_wgDomainFrame->setLineWidth( 1 );
    m_wgDomainFrameLayout = new Q3VBoxLayout( m_wgDomainFrame, 11, 6, "m_wgDomainFrameLayout");

    m_wgDomainLayout1 = new Q3HBoxLayout( 0, 0, 6, "m_wgDomainLayout1");

    m_wgDomainView = new Q3ComboBox( FALSE, m_wgDomainFrame, "m_wgDomainView" );
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

    m_wgDomainLayout2 = new Q3HBoxLayout( 0, 0, 6, "m_wgDomainLayout2");

    m_wgDomainExpModes = new Q3ButtonGroup( m_wgDomainFrame, "m_wgDomainExpModes" );
    m_wgDomainExpModes->setFrameShape( (Q3ButtonGroup::FrameShape) QFrame::NoFrame );
    m_wgDomainExpModes->setAlignment( int( Qt::AlignVCenter | Qt::AlignLeft ) );
    m_wgDomainExpModes->setColumnLayout(0, Qt::Vertical );
    m_wgDomainExpModes->layout()->setSpacing( 4 );
    m_wgDomainExpModes->layout()->setMargin( 10 );
    m_wgDomainExpModesLayout = new Q3HBoxLayout( m_wgDomainExpModes->layout() );
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

    m_wgBtmFrame = new QSplitter( this, "m_wgBtmFrame" );
    m_wgBtmFrame->setFrameShape( Q3Frame::NoFrame );
    m_wgBtmFrame->setFrameShadow( Q3Frame::Raised );
    m_wgBtmFrameLayout = new Q3HBoxLayout( m_wgBtmFrame, 0, 15, "m_wgBtmFrameLayout");

    m_wgBtmLeftFrame = new Q3Frame( m_wgBtmFrame, "m_wgBtmLeftFrame" );
    m_wgBtmLeftFrame->setFrameShape( Q3Frame::NoFrame );
    m_wgBtmLeftFrame->setFrameShadow( Q3Frame::Raised );
    m_wgBtmLeftFrameLayout = new Q3VBoxLayout( m_wgBtmLeftFrame, 0, 0, "m_wgBtmLeftFrameLayout");

    m_wgOpLabel1 = new QLabel( m_wgBtmLeftFrame, "m_wgOpLabel1" );
    m_wgBtmLeftFrameLayout->addWidget( m_wgOpLabel1 );

    m_wgOpFrame = new Q3Frame( m_wgBtmLeftFrame, "m_wgOpFrame" );
    m_wgOpFrame->setFrameShape( Q3Frame::GroupBoxPanel );
    m_wgOpFrame->setFrameShadow( Q3Frame::Raised );
    m_wgOpFrameLayout = new Q3VBoxLayout( m_wgOpFrame, 11, 6, "m_wgOpFrameLayout");

    m_wgOpCustomQueries = new QPushButton( m_wgOpFrame, "m_wgOpCustomQueries" );
    m_wgOpCustomQueries->setPaletteBackgroundColor( QColor( 224, 215, 234 ) );
    m_wgOpCustomQueries->setToggleButton( TRUE );
    m_wgOpCustomQueries->setFlat( FALSE );
    m_wgOpFrameLayout->addWidget( m_wgOpCustomQueries );

    m_wgOpVariantQueryDesc = new Q3ComboBox( FALSE, m_wgOpFrame, "m_wgOpVariantQueryDesc" );
    m_wgOpFrameLayout->addWidget( m_wgOpVariantQueryDesc );
    m_wgOpSpacer = new QSpacerItem( 21, 16, QSizePolicy::Minimum, QSizePolicy::Preferred );
    m_wgOpFrameLayout->addItem( m_wgOpSpacer );

    m_wgOpQueryList = new Q3ListView( m_wgOpFrame, "m_wgOpQueryList" );
    m_wgOpQueryList->header()->hide();
    m_wgOpQueryList->addColumn("Node Variants");
    m_wgOpQueryList->setColumnAlignment( 1, Qt::AlignRight );
    m_wgOpQueryList->setRootIsDecorated(true);
    m_wgOpQueryList->setSelectionMode(Q3ListView::NoSelection);

    m_wgOpFrameLayout->addWidget( m_wgOpQueryList );

    m_wgOpPerformQuery = new QPushButton( m_wgOpFrame, "m_wgOpPerformQuery" );
    m_wgOpPerformQuery->setPaletteBackgroundColor( QColor( 179, 200, 181 ) );
    m_wgOpPerformQuery->setFlat( FALSE );
    m_wgOpFrameLayout->addWidget( m_wgOpPerformQuery );
    m_wgBtmLeftFrameLayout->addWidget( m_wgOpFrame );
    m_wgBtmFrameLayout->addWidget( m_wgBtmLeftFrame );

    m_wgBtmRightFrame = new Q3Frame( m_wgBtmFrame, "m_wgBtmRightFrame" );
    m_wgBtmRightFrame->setFrameShape( Q3Frame::NoFrame );
    m_wgBtmRightFrame->setFrameShadow( Q3Frame::Raised );
    m_wgBtmRightFrameLayout = new Q3VBoxLayout( m_wgBtmRightFrame, 0, 0, "m_wgBtmRightFrameLayout");

    m_wgRangeLabel1 = new QLabel( m_wgBtmRightFrame, "m_wgRangeLabel1" );
    m_wgBtmRightFrameLayout->addWidget( m_wgRangeLabel1 );

    m_wgRangeFrame = new Q3Frame( m_wgBtmRightFrame, "m_wgRangeFrame" );
    m_wgRangeFrame->setFrameShape( Q3Frame::GroupBoxPanel );
    m_wgRangeFrame->setFrameShadow( Q3Frame::Raised );
    m_wgRangeFrameLayout = new Q3VBoxLayout( m_wgRangeFrame, 11, 6, "m_wgRangeFrameLayout");

    m_wgRangeInfo = new QLabel( m_wgRangeFrame, "m_wgRangeInfo" );
    m_wgRangeInfo->setFrameShape( QLabel::StyledPanel );
    m_wgRangeInfo->setFrameShadow( QLabel::Sunken );
    m_wgRangeInfo->setLineWidth( 1 );
    m_wgRangeInfo->setScaledContents( TRUE );
    m_wgRangeFrameLayout->addWidget( m_wgRangeInfo );

    m_wgRangeQueryProgress = new Q3ProgressBar(m_wgRangeFrame, "m_wgRangeQueryProgress");
    m_wgRangeQueryProgress->setFrameShape( QLabel::StyledPanel );
    m_wgRangeQueryProgress->setFrameShadow( QLabel::Sunken );
    m_wgRangeQueryProgress->setLineWidth( 1 );
    m_wgRangeQueryProgress->setHidden(true);
    m_wgRangeFrameLayout->addWidget(m_wgRangeQueryProgress);

    m_wgRangeRes = new Q3ListView( m_wgRangeFrame, "m_wgRangeRes" );
    m_wgRangeRes->addColumn("node");
    m_wgRangeRes->setSortColumn(m_wgRangeRes->columns()+1);
    m_wgRangeRes->setSelectionMode(Q3ListView::Extended);
    m_wgRangeRes->setShowSortIndicator (true);
    m_wgRangeFrameLayout->addWidget( m_wgRangeRes );

    m_wgRangeButtonLayout = new Q3HBoxLayout( 0, 0, 5, "m_wgRangeButtonLayout");

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
    //setWindowState( Qt::WState_Polished );

    m_wgDomainLabel->setText( tr( "<b>Query Domain</b>" ) );
    m_wgDomainAdd->setText( tr( "target" ) );
    m_wgDomainAdd->setIcon(QIcon(RoseIcons::target));
    m_wgDomainClear->setText( tr( "clear" ) );
    m_wgDomainClear->setIcon(QIcon(RoseIcons::cacheclear));
    m_wgDomainSetDefault->setText( tr( "default" ) );
    m_wgDomainSetDefault->setIcon(QIcon(RoseIcons::idefault));
    m_wgOpLabel1->setText( tr( "<b>Query Operator</b>" ) );
    m_wgOpCustomQueries->setText( tr( "custom queries" ) );
    m_wgOpPerformQuery->setText( tr( "perform query" ) );
    m_wgOpPerformQuery->setIcon(QIcon(RoseIcons::doquery));
    m_wgRangeLabel1->setText( tr( "<b>Query Range</b>" ) );
    m_wgRangeBroadcast->setText( tr( "broadcast" ) );
    m_wgRangeBroadcast->setIcon(QIcon(RoseIcons::cachebroadcast));
    m_wgRangeSetDomain->setText( tr( "set domain" ) );
    m_wgRangeSetDomain->setIcon(QIcon(RoseIcons::setdomain));
    m_wgRangeClear->setText( tr( "clear" ) );
    m_wgRangeClear->setIcon(QIcon(RoseIcons::clear_results));
    m_wgDomainNoExpansion->setText( tr( "no expansion" ) );
    m_wgDomainImmedChildren->setText( tr( "immediate children" ) );
    m_wgDomainAllDesc->setText( tr( "all descendants" ) );

    // ===================[connections]

    // domain
    connect(m_wgDomainView,SIGNAL(activated(int)),this,SLOT(domainViewClick(int)));
    connect(m_wgDomainAdd,SIGNAL(toggled(bool)),this,SLOT(domainTarget(bool)));
    connect(m_wgDomainSetDefault,SIGNAL(clicked()),this,SLOT(domainSetDefault()));
    connect(m_wgDomainClear,SIGNAL(clicked()),this,SLOT(domainClear()));
    connect(m_wgDomainExpModes,SIGNAL(clicked(int)),this,SLOT(domainSetExpMode(int)));


    // op
    connect(m_wgOpPerformQuery,SIGNAL(clicked()),this,SLOT(opPerformQuery()));
    connect(m_wgOpCustomQueries,SIGNAL(toggled(bool)),this,SLOT(opLoadQueryList(bool)));
    connect(m_wgOpVariantQueryDesc,SIGNAL(activated(int)),this,SLOT(opSyncVariantQueryWithDesc(int)));
    connect(m_wgOpQueryList,SIGNAL(clicked(Q3ListViewItem *, const QPoint &, int)),this,SLOT(opQueryListClick(Q3ListViewItem*, const QPoint&, int)));

    // range
    connect(this,SIGNAL(newQueryResults()),SLOT(rangeShowResults()));
    connect(m_wgRangeRes,SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint&,int)),this,
	                 SLOT(rangeResDoubleClick(Q3ListViewItem *,const QPoint&,int)));
    connect(m_wgRangeBroadcast,SIGNAL(clicked()),this,SLOT(rangeBroadcast()));
    connect(m_wgRangeSetDomain,SIGNAL(clicked()),this,SLOT(rangeSetDomain()));
    connect(m_wgRangeClear,SIGNAL(clicked()),this,SLOT(rangeClear()));
    //=====================[ some extra initialization code ]=======
    setCollectMode(true);
    reset();
}

QRQueryBox::~QRQueryBox() {
    clearCustomQueries();
}

void QRQueryBox::reset() {
    opLoadBuiltInVariantQueries();
    opLoadVariantViewDesc();
    domainSetDefault();
    domainTarget(false);
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
	// GAB(07/04/09): This code was written back in 2005, and I am not sure how
	// it figures out the SgNode class hierarchy. I had to hack it a bit in order
	// to work with the current version of ROSE.

    T_VariantNode root_node(V_SgNumVariants, NULL);
    T_VariantNode *tSgNode = 0;
    VariantVector vvector;
    map<VariantT, T_VariantNode *> variant_nodes;
    for (int v = 0; v < V_SgNumVariants; v++) {
    	VariantT variant;
    	variant = (VariantT) v;
        T_VariantNode *node;
        map<VariantT, T_VariantNode *>::iterator iter = variant_nodes.find(variant);
        if (iter == variant_nodes.end()) {
        	node = new T_VariantNode(variant, &root_node);
        	variant_nodes[variant] = node;
        	if (variant == V_SgNode) {
            	ROSE_ASSERT(!tSgNode);
        		tSgNode = node;
        	}
        } else {
        	node = iter->second;
        }
        unsigned node_depth = node->getDepth();
        vvector = VariantVector(variant);
        T_VariantNode *child_node;
        VariantT child_variant;

        for (unsigned i = 1; i < vvector.size(); i++) {
        	child_variant = vvector[i];
        	iter = variant_nodes.find(child_variant);
        	if (iter == variant_nodes.end()) {
        		child_node = new T_VariantNode(child_variant, node);
            	if (child_variant == V_SgNode) {
                	ROSE_ASSERT(!tSgNode);
            		tSgNode = child_node;
            	}
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

    ROSE_ASSERT(tSgNode);


    m_mapOpVariantItem.clear();
    m_mapOpItemVariant.clear();



    opLoadVariantQueryListAux(*tSgNode, NULL, NULL);
    Q3CheckListItem *root_item = m_mapOpVariantItem[tSgNode->m_variant];
    root_item->setOpen(true);
}


Q3CheckListItem* QRQueryBox::opLoadVariantQueryListAux(T_VariantNode &node, Q3CheckListItem *ancestor, Q3CheckListItem *last_sibling) {
    Q3CheckListItem *item = NULL;

    if (!node.isRoot()) {
       if (ancestor) {
          item = new Q3CheckListItem(ancestor, last_sibling, getVariantName(node.m_variant).c_str(), Q3CheckListItem::CheckBoxController);
       } else {
          item = new Q3CheckListItem(m_wgOpQueryList, last_sibling, getVariantName(node.m_variant).c_str(), Q3CheckListItem::CheckBoxController);
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

void QRQueryBox::domainSetDefault() {
    clearCache(!m_rootNode);
    if (m_rootNode) {insertNodeInCache(m_rootNode);}
}

void QRQueryBox::eventCacheClear() {
   eventCacheModified();
}
void QRQueryBox::eventCacheNewNodes(std::set<SgNode *> &nodes) {
   eventCacheModified();
}
void QRQueryBox::eventCacheRemovedNodes(std::set<SgNode *> &nodes) {
    eventCacheModified();
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
       pixmap = QRAstInfo::getPixmap(node);
       if (!pixmap) pixmap = QRPixmap::getPixmap(RoseIcons::empty);
       if (cache_size == 1) {
           m_wgDomainView->insertItem(*pixmap, QString("%2").arg(QRAstInfo::getInfo(node).c_str()));
       } else {
          m_wgDomainView->insertItem(*pixmap, QString("[%1.] %2").arg(++count).arg(QRAstInfo::getInfo(node).c_str()));
       }
   }
   m_domainNeedsToExpand = true;
}

bool QRQueryBox::isDomainTarget() {
    return m_wgDomainAdd->isOn();
}

void QRQueryBox::domainTarget(bool isOn) {
    m_wgDomainAdd->setOn(isOn);
    setTargetMode(isOn);
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


VariantVector QRQueryBox::getOpVariantVectorFromList() {
    VariantVector vector;

    ROSE_ASSERT(!isOpCustomQueries());

    if (isOpCustomQueries()) return vector;

    Q3CheckListItem *item = (Q3CheckListItem *) m_wgOpQueryList->firstChild();
    while(item) {
	getOpVariantVectorFromListAux(item,vector);
	item = (Q3CheckListItem *) item->nextSibling();
    }

    return vector;
}

void QRQueryBox::getOpVariantVectorFromListAux(Q3CheckListItem *item, VariantVector &vector) {
    if (item->state() == Q3CheckListItem::NoChange) {
       Q3CheckListItem *child_item = (Q3CheckListItem *) item->firstChild();
       while (child_item) {
          getOpVariantVectorFromListAux(child_item, vector);
	  child_item = (Q3CheckListItem *) child_item->nextSibling();
      }
    } else if (item->isOn()) {
       vector = vector + VariantVector(m_mapOpItemVariant[item]);
    }
}

QRQueryOperator* QRQueryBox::getOpCustomQueryFromList() {
    ROSE_ASSERT(isOpCustomQueries());

     map<unsigned,Q3CheckListItem*>::iterator iter = m_mapOpCustomQueryItem.begin();

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
       m_wgOpVariantQueryDesc->insertItem(iter->second.first.c_str());
   }
}

void QRQueryBox::insertVariantQuery(string desc, const VariantVector &v) {
    m_variantQueries[m_variantQueries.size()] = pair<string,VariantVector>(desc, v);
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
	VariantVector variantVector = getOpVariantVectorFromList();
	if (!variantVector.empty()) {
       	   op = new QRQueryOpVariant(variantVector);
	   deleteOp = true;
       }
    }

    if (op) {
       if (m_domainNeedsToExpand) {
          m_wgRangeInfo->setText("Expanding domain... please wait");
          QROSE::app()->processEvents();
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

    Q3CheckListItem *rootItem = new Q3CheckListItem(m_wgOpQueryList, "Custom Queries",Q3CheckListItem::RadioButtonController);
    Q3CheckListItem *lastSibling = NULL;
    for (TypeCustomQueries::iterator iter = m_customQueries.begin();
         iter != m_customQueries.end(); iter++)
    {
	string &desc = iter->second.first;
	lastSibling =  new Q3CheckListItem(rootItem,lastSibling, desc.c_str(), Q3CheckListItem::RadioButton);
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
    m_wgOpVariantQueryDesc->setCurrentItem(0);
}

bool QRQueryBox::isOpCustomQueries() {
    return m_wgOpCustomQueries->isOn();
}


void QRQueryBox::opSyncVariantQueryWithDesc(int id) {
   if (id != 0) {
       VariantVector &vector = m_variantQueries[id-1].second;
       setOpListFromVariantVector(vector);
   }
}

void QRQueryBox::setOpListFromVariantVector(const VariantVector &vector) {
    // make sure we are on the variant view mode
    if (isOpCustomQueries()) return;

    // uncheck all items (children are unchecked automatically as you uncheck the root items)
    Q3CheckListItem *item = (Q3CheckListItem *) m_wgOpQueryList->firstChild();
    while(item) {
	item->setOn(false);
	item = (Q3CheckListItem *) item->nextSibling();
    }

    // now check(set) each variant
    for (VariantVector::const_iterator iter = vector.begin(); iter != vector.end(); iter++) {
	const VariantT variant = *iter;
	Q3CheckListItem *item = m_mapOpVariantItem[variant];
	item->setOn(true);
    }
}


void QRQueryBox::opSyncDescWithVariantQuery() {
    if (isOpCustomQueries()) return;

    // get variant vector from the list
    VariantVector vector = getOpVariantVectorFromList();

    bool query_match = false;
    unsigned matchedId = 0;
    // traverse the list of builtin queries
    if (vector.size() > 0) {
	TypeVariantQueries::iterator iter = m_variantQueries.begin();
	while (iter != m_variantQueries.end() && !query_match) {
	    pair<string,VariantVector> &item = iter->second;
	    VariantVector &queryVector = item.second;
	    if (vector.size() == queryVector.size()) {
  	        bool differentVectors; differentVectors = false;
		VariantVector::iterator iter1 = vector.begin();
		while (iter1 != vector.end() && !differentVectors) {
		    VariantVector::iterator iter2 = queryVector.begin();
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

void QRQueryBox::opQueryListClick(Q3ListViewItem *item, const QPoint &pnt, int c) {
    if (isOpCustomQueries()) return;
    if (item) {
       Q3ListView *lv = item->listView();
       int checkBoxWidth = lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv);
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
    m_wgRangeRes->removeColumn(1);

    bool showInfo = false;
    set<SgNode *> *nodes = m_queryRange.getNodes();
    Q3ListViewItem *lastSibling = NULL;

    m_mapRangeItemNode.clear();
    m_wgRangeQueryProgress->setTotalSteps(nodes->size());
    QROSE::app()->processEvents();
    int count = 0;
    for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
	SgNode *node = *iter;
	string desc;
        desc = node->sage_class_name();
	string info = QRAstInfo::getInfo(node);
        if (desc != info)
            desc = desc + " <" + info + ">";
	QPixmap *pixmap = QRAstInfo::getPixmap(node);
	if (!pixmap) pixmap = QRPixmap::getPixmap(RoseIcons::empty);
	lastSibling = new Q3ListViewItem(m_wgRangeRes, lastSibling, desc.c_str());
	m_mapRangeItemNode[lastSibling] = node;
	lastSibling->setPixmap(0, *pixmap);
	string nodeStr;
	if (m_queryRange.getNodeStr(node, nodeStr)) {
           if (!showInfo) {
              m_wgRangeRes->addColumn("info");
              showInfo = true;
           }
           lastSibling->setText(1, nodeStr.c_str());
	}
	m_wgRangeQueryProgress->setProgress(count);
	count++;
    }
    QString info = QString("total=%1, hits=%2").arg(m_queryDomain.countDomain())
                                     .arg(m_queryRange.countRange());

    m_wgRangeInfo->setText(info);

    if (showInfo) {
      m_wgRangeRes->setColumnWidth(0,80);
      m_wgRangeRes->setColumnWidth(1,100);

    }
}

void QRQueryBox::rangeResDoubleClick(Q3ListViewItem *item, const QPoint&, int) {
    map<Q3ListViewItem *,SgNode *>::iterator iter = m_mapRangeItemNode.find(item);
    ROSE_ASSERT(iter != m_mapRangeItemNode.end());
    SgNode *node = iter->second;
    broadcastNode(node);
}

void QRQueryBox::rangeSelection(set<SgNode *> &range) {
   bool insertAllNodes = true;
   Q3ListViewItem *item = m_wgRangeRes->firstChild();
   while (item) {
      bool itemIsSelected = m_wgRangeRes->isSelected(item);

      if (itemIsSelected || insertAllNodes) {
          map<Q3ListViewItem *,SgNode *>::iterator iter = m_mapRangeItemNode.find(item);
          ROSE_ASSERT(iter != m_mapRangeItemNode.end());
   	  SgNode *node = iter->second;

          if (itemIsSelected && insertAllNodes) {
             insertAllNodes = false;
             range.clear();
          }
          range.insert(node);
      }
      item = item->nextSibling();
   }
}

void QRQueryBox::rangeBroadcast() {
    set<SgNode *> range;
    rangeSelection(range);
    if (range.size() == 1)
       broadcastNode(*(range.begin()));
    else {
       QRoseComponent::SgNodesMsg msg(&range);
       broadcastMsg(&msg);
    }
}

void QRQueryBox::rangeSetDomain() {
    set<SgNode *> range;
    rangeSelection(range);

    clearCache(range.empty());
    insertNodesInCache(&range);
}

void QRQueryBox::rangeClear() {
    m_wgRangeRes->clear();
    m_queryRange.clear();
    m_mapRangeItemNode.clear();
    m_queryDomain.clear();
    emit newQueryResults();
}


}
