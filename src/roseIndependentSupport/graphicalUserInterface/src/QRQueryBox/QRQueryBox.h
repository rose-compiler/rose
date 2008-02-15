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

#ifndef QRQUERYBOX_H
#define QRQUERYBOX_H

#include <qwidget.h>
#include <QRoseGroup.h>
#include <QRMacros.h>
#include <QRQueryDomain.h>
#include <QRQueryOp.h>
#include <QRQueryRange.h>
#include <set>
#include <qobject.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QLabel;
class QComboBox;
class QPushButton;
class QListBoxItem;
class QRadioButton;
class QButtonGroup;
class QListView;
class QCheckListItem;
class QListViewItem;
class QProgressBar;

typedef map<unsigned, pair<string,NodeQuery::VariantVector> > TypeVariantQueries;
typedef map<unsigned, pair<string,QRQueryOperator *> > TypeCustomQueries;
            
class QRQueryBox: public QWidget, public QRoseGroupWidget {
   
    Q_OBJECT
    
public:    
   QRQueryBox(const char *name = 0);
   QRQueryBox(QWidget *parent, const char *name = 0); 
   ~QRQueryBox();
   
   virtual void reset(); 
   
   void clearCustomQueries();
   void clearVariantQueries();
   
   void insertVariantQuery(string desc, const NodeQuery::VariantVector &v);    
   void insertCustomQuery(string desc, TypeQueryFuncPtr queryFunction){
       insertCustomQuery(desc, new QRQueryOpFunc(queryFunction));
   }
   
   template <class T>
       void insertCustomQuery(string desc, typename QRQueryOp<T>::TypeQueryMethodPtr queryMethod) {
           insertCustomQuery(desc, new QRQueryOp<T>(queryMethod));
       }
       
   bool isDomainAdd();   
   bool isOpCustomQueries();     
   QRQueryDomain::TypeDomainExpansion typeDomainExp() { return m_domainExpMode; }
   
public slots:
   virtual void domainSetDefault();
   virtual void domainClear();
   virtual void domainAdd(bool isOn);
   virtual void domainViewClick(int id);
   virtual void domainSetExpMode(QRQueryDomain::TypeDomainExpansion id);   
   virtual void opPerformQuery();
   virtual void opLoadQueryList(bool isOn);
   virtual void opLoadBuiltInVariantQueries();      
   virtual void opLoadVariantViewDesc();  
   virtual void opSyncVariantQueryWithDesc(int id);
   virtual void opSyncDescWithVariantQuery();
   virtual void rangeShowResults();
   virtual void rangeBroadcast();
   virtual void rangeSetDomain();
   virtual void rangeClear();
     
signals:
   void newQueryResults();
   
protected:      
    class T_VariantNode {
	friend class QRQueryBox;
    private:
	T_VariantNode(VariantT variant, T_VariantNode *parent);
	~T_VariantNode();
	bool isRoot() { return m_variant == V_SgNumVariants; }
	void setParent(T_VariantNode *parent);	
	void removeParent();
	unsigned getDepth();
	VariantT m_variant;
	T_VariantNode *m_parent;
	std::set<T_VariantNode *> m_children;
    };    
    
   virtual void init();
   
   NodeQuery::VariantVector getOpVariantVectorFromList();
   QRQueryOperator* getOpCustomQueryFromList();   
   void setOpListFromVariantVector(const NodeQuery::VariantVector &vector);
   
   // helper methods
   void getOpVariantVectorFromListAux(QCheckListItem *item, NodeQuery::VariantVector &v);
   QCheckListItem *opLoadVariantQueryListAux(T_VariantNode &node, QCheckListItem *ancestor, QCheckListItem *last_sibling);   
   
   virtual void eventCacheModified();       
   TypeVariantQueries* getBuiltInOps() { return &m_variantQueries; } 
   void insertCustomQuery(string desc, QRQueryOperator *op);
   virtual void opLoadVariantQueryList();         
   virtual void opLoadCustomQueryList();            
         
protected slots:
    void opQueryListClick(QListViewItem*,const QPoint &,int);
    void domainSetExpMode(int id);
    void rangeResDoubleClick(QListViewItem*,const QPoint&,int);

protected:
   map<VariantT,QCheckListItem*> m_mapOpVariantItem;
   map<QCheckListItem*,VariantT> m_mapOpItemVariant;   
   TypeVariantQueries m_variantQueries;
   TypeCustomQueries m_customQueries;
   map<unsigned,QCheckListItem*> m_mapOpCustomQueryItem;
   QRQueryDomain::TypeDomainExpansion m_domainExpMode;
   map<QListViewItem *,SgNode *> m_mapRangeItemNode;
   
   bool m_domainNeedsToExpand;
   QRQueryDomain m_queryDomain;
   QRQueryRange m_queryRange;
   
protected:
   QR_EXPORT(QFrame,TopFrame);
   QR_EXPORT(QLabel,DomainLabel);
   QR_EXPORT(QFrame,DomainFrame);
   QR_EXPORT(QComboBox,DomainView);
   QR_EXPORT(QLabel,DomainInfo);
   QR_EXPORT(QButtonGroup,DomainExpModes);
   QR_EXPORT(QRadioButton,DomainNoExpansion);
   QR_EXPORT(QRadioButton,DomainImmedChildren);
   QR_EXPORT(QRadioButton,DomainAllDesc);   
   QR_EXPORT(QPushButton,DomainAdd);
   QR_EXPORT(QPushButton,DomainClear);
   QR_EXPORT(QPushButton,DomainSetDefault);
   QR_EXPORT(QFrame,BtmFrame);
   QR_EXPORT(QFrame,BtmLeftFrame);
   QR_EXPORT(QLabel,OpLabel1);
   QR_EXPORT(QFrame,OpFrame);
   QR_EXPORT(QPushButton,OpCustomQueries);
   QR_EXPORT(QComboBox,OpVariantQueryDesc);
   QR_EXPORT(QListView,OpQueryList);
   QR_EXPORT(QPushButton,OpPerformQuery);
   QR_EXPORT(QFrame,BtmRightFrame);
   QR_EXPORT(QLabel,RangeLabel1);
   QR_EXPORT(QFrame,RangeFrame);
   QR_EXPORT(QLabel,RangeInfo);
   QR_EXPORT(QListView,RangeRes);
   QR_EXPORT(QPushButton,RangeBroadcast);
   QR_EXPORT(QPushButton,RangeSetDomain);
   QR_EXPORT(QPushButton,RangeClear);
   QR_EXPORT(QProgressBar,RangeQueryProgress);

   // layouts and spacers
   QR_EXPORT(QVBoxLayout,MasterLayout);
   QR_EXPORT(QVBoxLayout,TopFrameLayout);
   QR_EXPORT(QVBoxLayout,DomainFrameLayout);
   QR_EXPORT(QHBoxLayout,DomainLayout1);
   QR_EXPORT(QHBoxLayout,DomainLayout2);
   QR_EXPORT(QHBoxLayout,DomainExpModesLayout);
   QR_EXPORT(QSpacerItem,DomainSpacer);
   QR_EXPORT(QHBoxLayout,BtmFrameLayout);
   QR_EXPORT(QVBoxLayout,BtmLeftFrameLayout);
   QR_EXPORT(QVBoxLayout,OpFrameLayout);
   QR_EXPORT(QSpacerItem,OpSpacer);
   QR_EXPORT(QVBoxLayout,BtmRightFrameLayout);
   QR_EXPORT(QVBoxLayout,RangeFrameLayout);
   QR_EXPORT(QHBoxLayout,RangeButtonLayout);
};
#endif
