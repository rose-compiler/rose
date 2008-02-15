/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                          *
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

#ifndef QRTREEBOX_H
#define QRTREEBOX_H

#include <qlistview.h>
#include <QRoseGroup.h>
#include <QRAttribute.h>
#include <qwidget.h>
#include <map>

class QRListViewItem;
class QPoint;
class TreeVisitor;
class QPopupMenu;
class QStatusBar;
class QVBoxLayout;
class QRToolBar;

class QRTreeBox: public QWidget, public QRoseGroupWidget, public QRAttributeWidget {
    friend class QRListViewItem;
    friend class TreeVisitor;
    
    Q_OBJECT
    
public:
   QRTreeBox(const char *name = 0);
   QRTreeBox(QWidget *parent, const char *name = 0); 
   ~QRTreeBox();
        
   bool is_highlight_enabled();
   void set_highlight_enabled(bool is_on);
   
   bool is_lookup_enabled();
   void set_lookup_enabled(bool is_on);
   
   bool is_cache_view_enabled();
   void set_cache_view_enabled(bool is_on);
   
      
protected:
    virtual void init();    
    int compare_items(QRListViewItem *item1, QRListViewItem *item2, int col, bool ascending);   
    
    std::map<std::string, unsigned>* captured_names() { return &m_captured_names; }
    std::map<unsigned, std::string>* captured_indices() { return &m_captured_indices; }       
    
    std::map<QRListViewItem *, SgNode *>* view_items() {return &m_view_items; }
    std::map<SgNode *, QRListViewItem *>* node_items() { return &m_node_items; }
    
    QListView *listview() { return m_listview; }
    
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);    
    
    void receiveMsg(QRGroupWidget *emitter, QRMessage *msg);    
    
    void showEvent(QShowEvent *event);
       
public slots:
    virtual void load_tree();   
    
protected slots:    
    virtual void clicked_header(int column);
    virtual void double_click_cell(QListViewItem *item, const QPoint &point, int column);
    virtual void cell_renamed(QListViewItem *item, int column);
    virtual void show_popup(QListViewItem *item, const QPoint &point, int column);
    virtual void clicked_popmenu_nodes(int id);
    virtual void clicked_toolbar(int id, bool buttonDown);
    
protected:
   
   typedef enum toolbar_id { tb_target_mode, tb_cache_view, tb_highlight, tb_clear, tb_lookup, tb_accum };
   
   QRToolBar *m_toolbar;    
   QListView *m_listview;
   QStatusBar *m_statusbar;
   QVBoxLayout *m_layout;
   
   int m_sort_column;
   bool m_sort_ascending;
   
   bool m_highlight_enabled;
   bool m_lookup_enabled;
   bool m_cache_view_enabled;
   
   std::map<std::string, unsigned> m_captured_names;  
   std::map<unsigned, std::string> m_captured_indices;
   
   std::map<QRListViewItem *, SgNode *> m_view_items;
   std::map<SgNode *, QRListViewItem *> m_node_items;
   
   // popup menu: project tree nodes
   typedef enum popmenu_nodes_id { pm_set_root, pm_root_default, pm_expand, pm_collapse, pm_expand_code };   
   QPopupMenu *m_popmenu_nodes;
      
   int m_key;
};


#endif

