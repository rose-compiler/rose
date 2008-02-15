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

#define HAVE_EXPLICIT_TEMPLATE_INSTANTIATION
#include <qheader.h>
#include <QRAstInfo.h>
#include <QRGui.h>
#include <QRTreeBox.h>
#include <QRAstInfo.h>
#include <config.h>
#include <rose.h>
#include <map> 	
#include <qpainter.h>
#include <QRLayout.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <queue>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <QRToolBar.h>
#include <string>
#include <qlabel.h>

using namespace std;
/*****************************************[ ListViewItem ]*****************************************/

class QRListViewItem : public QListViewItem
{
public:
    QRListViewItem( QListView *parent, const char *name): QListViewItem(parent, name), m_node(NULL) {  }    
    QRListViewItem( QListViewItem *parent, const char *name): QListViewItem(parent, name), m_node(NULL) { }
    QRListViewItem( QListViewItem *parent, QListViewItem *sibling, const char *name): 
	QListViewItem(parent, sibling, name), m_node(NULL) { }

    virtual void paintCell( QPainter *p, const QColorGroup &cg,
                            int column, int width, int alignment) 
	
    {		
	p->save();		
        QColorGroup cgn(cg);

	if (is_writable(column)) {
	    cgn.setColor(QColorGroup::Base, QColor(231,255,216));	    
	}
	
	if (((QRTreeBox *) listView()->parent())->is_highlight_enabled()) {
	    if (column == 0 && m_node && ((QRTreeBox *) listView()->parent())->isNodeInCache(m_node)) {
		cgn.setColor(QColorGroup::Base, QColor(231,255,216));
	    }
	}
	
        QListViewItem::paintCell(p,cgn,column,width,alignment);
	
        // Draw a box around the Cell
	int nheight = height() - 1;
	int nwidth = width - 1;
        p->setPen(cg.color(QColorGroup::Background));
        p->drawLine(0,nheight,nwidth,nheight);
        p->lineTo(nwidth, 0);
	p->restore();
	
    } 
    
    int compare ( QListViewItem * i, int col, bool ascending ) const {
	return ((QRTreeBox *) listView()->parent())->compare_items((QRListViewItem *) this, 
						       (QRListViewItem *) i, col, ascending);	
    }
    
    unsigned get_id() { return m_id; }
    void set_id(unsigned id) { m_id = id; }
    
    void set_node(SgNode *node) { m_node = node; }
    SgNode* get_node() { return m_node; }
    
    AstAttribute* get_ast_attribute(string attrib_name) {
	if (!m_node) return NULL;
	AstAttributeMechanism &attrib_mech = m_node->attribute;
	if (attrib_mech.exists(attrib_name)) {
	    return attrib_mech[attrib_name];	    
	} else {
	    return NULL;
	}	
    }
    
    QRAttribute* get_qrattrib(int column) {
	if (column == 0) return NULL;
	QRTreeBox *tree = (QRTreeBox *) listView()->parent();
	string attrib_name = (*tree->captured_indices())[column];
	QRAttribute *qrattrib = tree->get_attribute(attrib_name);
	return qrattrib;
    }	
    
    
    bool is_writable(int column, QRAttribute *qrattrib = NULL) {
	if (column == 0) return false;
	
	if (qrattrib == NULL) {
  	   qrattrib = get_qrattrib(column);
       }
	
 	return get_ast_attribute(qrattrib->name()) && qrattrib->can_modify();
    }
    
    void set_cell_value(int column, QRAttribute *qrattrib, string str) {
	QPixmap *pixmap = qrattrib->get_pixmap(str);
	setText(column, str);
	if (pixmap) {
	    setPixmap(column, *pixmap);
	}
	    
    }

               	
protected:
    unsigned m_id;
    SgNode *m_node;
}; 


/*************************************************************************************************/

/*****************************************[ TreeVisitor ]*****************************************/
struct TreeVisitorInfo  {
    int level;
}; 

class QRTreeBox;
 
class TreeVisitor: public AstTopDownProcessing<TreeVisitorInfo> {
    
public:
    TreeVisitor(QRTreeBox *tree) { m_tree = tree; m_id_counter = 0; }
        
protected:
    TreeVisitorInfo virtual evaluateInheritedAttribute(SgNode *node, TreeVisitorInfo info) {
       QRListViewItem *li;

       
       // possible traversal bug if this conditional yields false
       if ((info.level == 0) || (m_ids.find(node->get_parent()) != m_ids.end())) {
	   m_ids[node] = m_id_counter;      
	   
	   if (!m_tree->is_cache_view_enabled()) 
	       process_normal_view(node, info); else
		   process_cache_view(node, info);       
	   m_id_counter++;
       } 
       
       info.level++;
       return info;	
    } 
    
    void register_node (QRListViewItem *li, SgNode *node) {
	map<SgNode *, QRListViewItem *>::iterator iter = m_tree->node_items()->find(node);
	if (iter != m_tree->node_items()->end())
	    delete iter->second;
       (*m_tree->node_items())[node] = li;
        m_last_child[node] = NULL;
	
       if (li) {	
           li->set_id(m_ids[node]); 
	   li->set_node(node);
  	   	   	   
	   QPixmap *pixmap = QRAstInfo::get_pixmap(node);
	   if (pixmap) {
	       li->setPixmap(0, *pixmap);
	   } 
	   
	   (*m_tree->view_items())[li] = node;
	   	   
	   if (!m_tree->is_cache_view_enabled()) {
   	      li->setOpen(QRAstInfo::is_relevant(node)); 
	   }	 
	   
	   // handling attributes 	   
	   if (m_tree->is_capture_enabled()) {
	       map<string, AstAttribute *> attributes;
	       m_tree->matched_attributes(node, attributes);
	       map<string, unsigned> *column_names = m_tree->captured_names();
	       map<unsigned, string> *column_indices = m_tree->captured_indices();
	       
	       for (map<string, AstAttribute *>::iterator iter = attributes.begin();
   	            iter != attributes.end(); iter++)	       
	       {
		   // check if we have a column for this attribute
		   string attrib_name = iter->first;
		   map<string, unsigned>::iterator column_iter = column_names->find(attrib_name);
		   unsigned column_index;
		   if (column_iter != column_names->end()) {
		       column_index = column_iter->second;   
		   } else {
		       column_index = m_tree->listview()->addColumn(attrib_name.c_str());
		       (*column_names)[attrib_name] = column_index;
		       (*column_indices)[column_index] = attrib_name;
		   }
		   QRAttribute *qrattrib = m_tree->get_attribute(attrib_name);
		   li->set_cell_value(column_index, qrattrib, iter->second->toString());

	       }
	   } 
	   	   
       } 
   }
    
    
    void process_normal_view(SgNode *node, TreeVisitorInfo &info) {
       QRListViewItem *li = NULL;
       if (info.level == 0) {
          li = new QRListViewItem(m_tree->listview(), QRAstInfo::get_info(node).c_str());	
       } else {
	   SgNode *parent = node->get_parent();
           map<SgNode *, QRListViewItem *>::iterator iter = m_last_child.find(parent);
	   if (iter != m_last_child.end()) {	   
	      QRListViewItem *&last_child_item = iter->second;
	      if (!last_child_item) { // first child
                  li = new QRListViewItem((*m_tree->node_items())[parent], QRAstInfo::get_info(node).c_str());  	   
	      } else {
		  li = new QRListViewItem((*m_tree->node_items())[parent], last_child_item, QRAstInfo::get_info(node).c_str());
	      }
	      last_child_item = li;	       
	  }
       } 
       register_node(li, node);
   }
        
    void process_cache_view(SgNode *node, TreeVisitorInfo &info) {
	QRListViewItem *li = NULL;
	
       if (m_tree->isNodeInCache(node)) {       
          li = new QRListViewItem(m_tree->listview(), QRAstInfo::get_info(node).c_str());	  
       } else { 
          SgNode *parent = node->get_parent();
	  QRListViewItem *parent_item = m_tree->node_items()->find(parent)->second;
	  if (parent_item) {
	      map<SgNode *, QRListViewItem *>::iterator iter = m_last_child.find(parent);
	      if (iter != m_last_child.end()) {	   
		  QRListViewItem *&last_child_item = iter->second;
		  if (!last_child_item) { // first child
		      li = new QRListViewItem((*m_tree->node_items())[parent], QRAstInfo::get_info(node).c_str());  	   
		  } else {
		      li = new QRListViewItem((*m_tree->node_items())[parent], last_child_item, QRAstInfo::get_info(node).c_str());
		  }
		  last_child_item = li;	       
	      } 
	  }
     }
      register_node(li, node);
    }
           
    map<SgNode *, unsigned> m_ids;
    // associate an sgNode with its last child, speeds up the inserts
    map<SgNode *, QRListViewItem *> m_last_child;
    QRTreeBox *m_tree;
    unsigned m_id_counter;
};

/*******************************************************************************************************/

QRTreeBox::QRTreeBox(const char *name): 
    QWidget((QWidget *) QRGUI::current_panel_frame(), name) 
{
    init();
}
 
QRTreeBox::QRTreeBox(QWidget *parent, const char *name): 
   QWidget(parent, name) 
{     
    init();
}

QRTreeBox::~QRTreeBox() {
    delete m_popmenu_nodes;    
}

void QRTreeBox::init() {

   // toolbar
    m_toolbar = new QRToolBar(this);
       
    m_toolbar->insertItem("target", tb_target_mode, true);
    m_toolbar->insertSeparator();
    m_toolbar->insertItem("accum", tb_accum, true);        
    m_toolbar->insertItem("cache", tb_cache_view, true);
    m_toolbar->insertItem("clear", tb_clear, false);
    m_toolbar->insertItem("highlight", tb_highlight, true);        
    m_toolbar->insertSeparator();
    m_toolbar->insertItem("lookup", tb_lookup, true);
    connect(m_toolbar,SIGNAL(clicked(int, bool)), 
            this, SLOT(clicked_toolbar(int, bool)));
                      
   // listview
   m_listview = new QListView(this);   
   m_listview->setShowSortIndicator(true);   
   
   connect(m_listview->header(), SIGNAL(clicked(int)), this, SLOT(clicked_header(int)));     
   connect(m_listview, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
	   this, SLOT(double_click_cell(QListViewItem*, const QPoint&, int )));
   connect(m_listview, SIGNAL(itemRenamed(QListViewItem *, int)),
           this, SLOT(cell_renamed(QListViewItem *, int)));
   connect(m_listview, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
           this, SLOT(show_popup(QListViewItem *, const QPoint &, int))); 
   if (QRGUI::autoGroup()) {
       QRGUI::currentGroup()->attach(this);
   }    
   m_listview->setDefaultRenameAction (QListView::Accept); 
   
   // status bar
   m_statusbar = new QStatusBar(this);
   m_statusbar->setSizeGripEnabled(false);
   QLabel* label = new QLabel( m_statusbar );
   m_statusbar->addWidget(label, 1, true);
   connect(m_statusbar, SIGNAL(messageChanged(const QString&)),
	   label, SLOT(setText(const QString&)));
   
   // setting the layout
   m_layout = new QVBoxLayout(this);    
   m_layout->add(m_toolbar);
   m_layout->add(m_listview);
   m_layout->add(m_statusbar);
   
   // popup menu for Project Tree Column
   m_popmenu_nodes = new QPopupMenu(this);
   m_popmenu_nodes->insertItem("set root", pm_set_root);
   m_popmenu_nodes->insertItem("default root", pm_root_default);
   m_popmenu_nodes->insertSeparator();
   m_popmenu_nodes->insertItem("expand children", pm_expand);
   m_popmenu_nodes->insertItem("collapse children", pm_collapse);
   m_popmenu_nodes->insertItem("expand code style", pm_expand_code);   
   connect(m_popmenu_nodes, SIGNAL(activated(int)), 
           this, SLOT(clicked_popmenu_nodes(int)));     
   
   m_key = 0; 
   
    //toolbar defaults
    m_toolbar->setState(tb_target_mode, true);
    m_toolbar->setState(tb_accum, true);   
    m_toolbar->setState(tb_cache_view, false);
    m_toolbar->setState(tb_lookup, false);
    m_toolbar->setState(tb_highlight, false);
}    
    
void QRTreeBox::load_tree() {  
   
   m_view_items.clear();
   m_node_items.clear();   
   m_captured_names.clear();
   m_captured_indices.clear();
   m_listview->clear();        
   while (m_listview->columns())
      m_listview->removeColumn(0); 
   
   m_listview->addColumn("Project Tree",600);
   m_listview->setAllColumnsShowFocus (true);    
   m_listview->setSorting(0);
   m_listview->setShowSortIndicator(false);   
   m_listview->setRootIsDecorated(true);
   m_sort_column = 0;
      
   TreeVisitor visitor(this);
   TreeVisitorInfo info;
               
   info.level = 0;

   visitor.traverse(m_rootNode, info);      
} 


void QRTreeBox::clicked_header(int column) {
    if (column == 0) {
	if (m_sort_column != 0) {
	    m_listview->setSorting(0, true);
	    m_listview->sort();
            m_listview->setShowSortIndicator(false);	    	    
	} else {
	    m_listview->setSorting(-1);
	}
    } else {		
        if (column != m_sort_column) {
	    m_sort_ascending = true;
        } else {
	    m_sort_ascending = !m_sort_ascending; 	
        }
        m_listview->header()->setSortIndicator(column, m_sort_ascending);		
        m_listview->setSorting(column, m_sort_ascending);
        m_listview->sort();	
    }
    m_sort_column = column;    
}

int QRTreeBox::compare_items(QRListViewItem *item1, QRListViewItem *item2, int col, bool ascending) {
    if (col == 0) {
        // sorting for column 0 (Project Tree) is just the order in which they were created
	if (item1->get_id() < item2->get_id()) {
	    return -1; 
	} else if (item1->get_id() == item2->get_id()) {
	    return 0;
	} else {
	    return 1;
	}
    } else {	
	// find attribute name corresponding to 'col' index
	std::map<unsigned, std::string>::iterator iter = m_captured_indices.find(col);
	ASSERT(iter != m_captured_indices.end());
	
        string attrib_name = iter->second;
	QRAttribute *qrattrib = get_attribute(attrib_name);
			
	AstAttribute *att_ast1 = item1->get_ast_attribute(attrib_name);
	AstAttribute *att_ast2 = item2->get_ast_attribute(attrib_name);
	
	return qrattrib->compare(att_ast1, att_ast2, ascending);
    }
}

void QRTreeBox::double_click_cell(QListViewItem *item, const QPoint &point, int column) {
    if (column == 0) {
	if (m_key == Qt::Key_Control) {
   	   broadcastNode(((QRListViewItem *) item)->get_node());		
           item->setOpen(!item->isOpen());              	
        }
    } else {	
	QRAttribute *qrattrib = ((QRListViewItem *) item)->get_qrattrib(column);
	if (!qrattrib) return;
	
	AstAttribute *ast_attrib = ((QRListViewItem *) item)->get_ast_attribute(qrattrib->name());
	
	if (ast_attrib && qrattrib->can_modify()) {	
	    if (qrattrib->can_edit()) {
		item->setRenameEnabled(column, true);
		item->startRename(column);       
		item->setRenameEnabled(column, false);   	   
	    } else {
		qrattrib->fromString(ast_attrib, qrattrib->get_next_value(ast_attrib->toString()));
		((QRListViewItem *) item)->set_cell_value(column, qrattrib, ast_attrib->toString());
	    }
	    item->setOpen(!item->isOpen());       
	}
    }
}
   
void QRTreeBox::cell_renamed(QListViewItem *item, int column) {
    QString text = item->text(column);
    QRAttribute *qrattrib = ((QRListViewItem *) item)->get_qrattrib(column);  
    AstAttribute *ast_attrib = ((QRListViewItem *) item)->get_ast_attribute(qrattrib->name());
    
    if (ast_attrib) {  
       qrattrib->fromString(ast_attrib, text);
    }
 
   ((QRListViewItem *) item)->set_cell_value(column, qrattrib, ast_attrib->toString());
}  
 

void QRTreeBox::show_popup(QListViewItem *item, const QPoint &point, int column) {
    if (item && column == 0) {
	m_popmenu_nodes->exec(point);
    }	
}


void QRTreeBox::clicked_popmenu_nodes(int id) {

    QRListViewItem *selected_item = (QRListViewItem *) m_listview->selectedItem();
    SgNode *selected_node = selected_item->get_node();
    switch (id) {
	case pm_set_root:
	    m_rootNode = selected_item->get_node();
	    load_tree();
	    selected_item = m_node_items[selected_node];  	    
	    m_listview->setSelected(selected_item, true);    	    
	    m_listview->setOpen(selected_item, true);
   	    break;
	case pm_root_default:
	    m_rootNode = QRGUI::get_root();	    
	    load_tree();
	    selected_item = m_node_items[selected_node];  
	    m_listview->setSelected(selected_item, true);    
	    m_listview->ensureItemVisible(selected_item);
	    break;
	case pm_expand:
	case pm_collapse:
	case pm_expand_code:
	    {
		queue<QRListViewItem *> items;
		items.push(selected_item); 

		while (items.size() != 0) {		    
   		    QRListViewItem *item;		    
		    item = items.front(); items.pop();
		    if ((item == selected_item) && (id != pm_collapse)) {
			item->setOpen(true);
		    } else {		
   		       item->setOpen((id == pm_expand_code)? QRAstInfo::is_relevant(item->get_node()) : id == pm_expand);
		    }
		    QRListViewItem *child_item = (QRListViewItem *) item->firstChild();
		    while (child_item) { items.push(child_item); child_item = (QRListViewItem *) child_item->nextSibling(); }
		} 
	    }
	    break;
    }
}

void QRTreeBox::clicked_toolbar(int id, bool buttonDown) {
    switch (id) {
	case tb_target_mode: 
	    setTargetEnabled(buttonDown);
	    m_statusbar->message(buttonDown? "target enabled" : "target disabled", 2000);
	    break;
	case tb_accum:
	    setCacheAccumEnabled(buttonDown);
	    m_statusbar->message(buttonDown? "cache accumulate enabled" : "cache accumulate disabled", 2000);	    
	    break;
	case tb_clear:
	    clearCache();
	    m_statusbar->message("cache empty", 2000);	    	    
	    break;
	case tb_cache_view:
	    set_cache_view_enabled(buttonDown);
	    m_statusbar->message(buttonDown? "cache view enabled" : "cache view disabled", 2000);
	    break;
	case tb_lookup:
	    set_lookup_enabled(buttonDown);
	    m_statusbar->message(buttonDown? "lookup enabled" : "lookup disabled", 2000);
	    break;	
	case tb_highlight:
	    set_highlight_enabled(buttonDown);
	    m_statusbar->message(buttonDown? "highlight enabled" : "highlight disabled", 2000);
	    break;	
    }	    
}


void QRTreeBox::keyPressEvent(QKeyEvent *event) {
    m_key = event->key();
}

void QRTreeBox::keyReleaseEvent(QKeyEvent *event) {
    m_key = 0;
}

void QRTreeBox::receiveMsg(QRGroupWidget *emitter, QRMessage *msg) {
    switch (msg->id()) {
	case msg_node:
	   {

   	      SgNode *node = ((QRoseGroupWidget::SgNodeMsg *) msg)->getNode();
  	      if (m_node_items.find(node) != m_node_items.end()) {
   	         insertNodeInCache(node);
	         static char text[100];
		 sprintf(text, "cached items: %zu", m_cache.size());
   	         m_statusbar->message(text);
		 
		 if (is_cache_view_enabled() && emitter != this) {
		     //load_tree();
		 }
		 if (is_lookup_enabled()) {
        	    QRListViewItem *selected_item = m_node_items[node];  
		    if (selected_item) {
   	               m_listview->setSelected(selected_item, true);    	    
           	       m_listview->ensureItemVisible(selected_item);		    
		    }
		}
		 
		 if (is_highlight_enabled()) {
		     m_listview->viewport()->update();
		 }
		 
	     }	
           }
	   break;
   }
}

bool QRTreeBox::is_highlight_enabled() {
    return m_highlight_enabled;
}

void QRTreeBox::set_highlight_enabled(bool is_on) {
    m_highlight_enabled = is_on;
    m_listview->viewport()->update(); 
}
   
bool QRTreeBox::is_lookup_enabled() {
    return m_lookup_enabled;
}

void QRTreeBox::set_lookup_enabled(bool is_on) {
   m_lookup_enabled = is_on;
}
   
bool QRTreeBox::is_cache_view_enabled() {
    return m_cache_view_enabled;
}
 
void QRTreeBox::set_cache_view_enabled(bool is_on) {
    m_cache_view_enabled = is_on;
}

void QRTreeBox::showEvent(QShowEvent *event) {
   // if (QRGUI::is_exec()) {
	m_rootNode = QRGUI::get_root();
       load_tree();
  // }
}

#include "QRTreeBox.moc"
