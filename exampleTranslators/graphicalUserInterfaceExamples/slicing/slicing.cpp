/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
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
#include <rose.h>
#include <config.h>

#include <qrose.h>

#include <qlistview.h>
#include <qapplication.h>
#include <qwidget.h>


#include <set>

using namespace std;

void list2Symbols(QListView *list, SgNode *node) {
    QRQueryDomain domain; domain.expand(node, QRQueryDomain::all_children);
    NodeQuery::VariantVector v(V_SgVariableDeclaration);
    QRQueryOpVariant op(v);  
    QRQueryRange range; op.performQuery(&domain, &range);
    
    list->clear();
    set<SgNode *> *nodes = range.getNodes();
    for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
       SgNode *node = *iter;
       QCheckListItem *item = new QCheckListItem(list,QRAstInfo::get_info(node), QCheckListItem::CheckBox);
       item->setPixmap(0, *QRPixmap::get_pixmap(icons::var));
   }
}

void initList2(QListView *list2) {
   list2->addColumn("2. select symbols");
   list2->setEnabled(false);
}

void list1ItemSelected(QObject *emmiter, void *arg) {
   QListView *list1 = (QListView *) emmiter;
   QListView *list2 = QRGUI::getWidget<QListView> ("list2");
   
   map<QListViewItem *, SgNode *> &items = *((map<QListViewItem *, SgNode *> *) arg);
   
   // get selected item in list1, 
   // and disable list2 if no item is selected
   QListViewItem *selectedItem = list1->selectedItem();
   list2->setEnabled(selectedItem != NULL);
   
   if (!selectedItem) { list2->clear(); QRGUI::dialog(1)->hide(); return; }
   
   QRGUI::dialog(1)->show();
   
   QRCodeBox *box = QRGUI::getWidget<QRCodeBox>("codebox", 1);
      
   SgNode *funcDef = items[selectedItem];
   box->insertNodeInCache(funcDef);   
   list2Symbols(list2, funcDef);    
   
}

void initList1(QListView *list) {    
   static map<QListViewItem *, SgNode *> items;
   
   list->addColumn("1. select a function");
   // expand project domain
   QRQueryDomain domain; domain.expand(QRGUI::get_root(), QRQueryDomain::all_children);
   
   // get variant op 
   NodeQuery::VariantVector v(V_SgFunctionDeclaration);
   QRQueryOpVariant op(v);   
   
   // perform query and get results into the range
   QRQueryRange range; op.performQuery(&domain, &range);
   
   // fills the listbox with all defined functions
   set<SgNode *> *nodes = range.getNodes();
   for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
       SgNode *node = *iter;
       if ( ((SgFunctionDeclaration *) (node))->get_definition()) {
           QListViewItem *item = new QListViewItem(list,QRAstInfo::get_info(node));
	   item->setPixmap(0, *QRPixmap::get_pixmap(icons::func_defn));
	   items[item] = *iter;
       }       
   }
   
   QRGUI::callback()->link(list,SIGNAL(selectionChanged()), &list1ItemSelected, &items);
}


int main(int argc, char **argv) {
    SgProject *project = frontend(argc, argv);
    
    QRGUI::panel_type(QRGUI::right);    
    QRGUI::init(argc, argv, project);    
   
    QListView *list1 = new QListView(QRGUI::current_panel()->frame(), "list1");     
    initList1(list1);
        
    QListView *list2 = new QListView(QRGUI::current_panel()->frame(), "list2"); 
    initList2(list2);
    
    QRGUI::dialog(0)->setGeometry(0,0,800,400);
    QRGUI::dialog(0)->sizes(50);
    
    // new dialog
    QRGUI::panel_type(QRGUI::down);
    QRGUI::dialog(1)->setGeometry(820,0,600,900);
    
    QRCodeBox *box = new QRCodeBox("codebox"); 
    box->getWidgetToolbar()->setState(QRCodeBox::tb_cache_view, true); 
    box->getWidgetToolbar()->setHidden(true);    
    box->setCacheAccumEnabled(false);
    
    QRCodeBox *box2 = new QRCodeBox("transformed code");
    box2->getWidgetToolbar()->setState(QRCodeBox::tb_cache_view, true);     
    box2->getWidgetToolbar()->setHidden(true);        
    
    QRGUI::exec();
}


