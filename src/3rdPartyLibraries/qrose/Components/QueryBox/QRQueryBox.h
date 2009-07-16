/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRQUERYBOX_H
#define QRQUERYBOX_H

#include <Qt3Support>
#include <QtGui>
#include <QtCore>

#include <QRGroup.h>
#include <QRoseComponent.h>
#include <QRMacros.h>
#include <QRQueryDomain.h>
#include <QRQueryOp.h>
#include <QRQueryRange.h>

#include <rose.h>
#include <set>
#include <map>

#include <QObject>
#include <QWidget>


namespace qrs {

/**
* \ingroup qrose_comps
*/

/// Component that implements ROSE IR Queries
class QRQueryBox: public QWidget, public QRoseComponent {

    Q_OBJECT

public:

   //@{
        /// @name Initialization

        /// Creates a query-box component with @a node as domain and associated to group 0
   QRQueryBox(SgNode *node = 0);

        /// Initializes component with default settings
   virtual void reset();

   ~QRQueryBox();

   //@}


   //@{

          /// @name Properties

         /// Returns true if domain is in target mode (all IR nodes received from other components go into the domain)
   bool isDomainTarget();
         /// Returns true if component is showing custom queries, false if showing variant queries
   bool isOpCustomQueries();
         /// Returns domain expansion type
   QRQueryDomain::TypeDomainExpansion typeDomainExp() { return m_domainExpMode; }

   //@}

   //@{

          /// @name Query operators

          /// Inserts a query based on a variant vector
   void insertVariantQuery(std::string desc, const VariantVector &v);
          /// Inserts a custom query based on a function
   void insertCustomQuery(std::string desc, QRQueryOpFunc::TypeQueryFuncPtr queryFunction){
       insertCustomQuery(desc, new QRQueryOpFunc(queryFunction));
   }
          /// Inserts a custom query based on a class [T] method
   template <class T>
       void insertCustomQuery(std::string desc, typename QRQueryOp<T>::TypeQueryMethodPtr queryMethod) {
           insertCustomQuery(desc, new QRQueryOp<T>(queryMethod));
       }

         /// Clears all custom queries
   void clearCustomQueries();
         /// Clears all variant queries
   void clearVariantQueries();

   //@}


public slots:
          /// Sets default domain (root node passed in the constructor)
   virtual void domainSetDefault();
          /// Removes every element in the domain
   virtual void domainClear();
          /// Sets domain to target mode (every node received from other components is part of the domain)
   virtual void domainTarget(bool isOn);
          /// Sets domain expansion mode type
   virtual void domainSetExpMode(QRQueryDomain::TypeDomainExpansion id);
          /// Performs query with specified domain
   virtual void opPerformQuery();
          /// Shows results in range window
   virtual void rangeShowResults();
          /// Broadcasts range items
   virtual void rangeBroadcast();
          /// Sets domain with range items (for further filtering)
   virtual void rangeSetDomain();
          /// Clears range window
   virtual void rangeClear();

signals:
          /// Signal emitted after query is performed
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

   virtual void init(SgNode *node);

   void rangeSelection(std::set<SgNode *> &range);

   VariantVector getOpVariantVectorFromList();
   QRQueryOperator* getOpCustomQueryFromList();
   void setOpListFromVariantVector(const VariantVector &vector);

   // helper methods
   void getOpVariantVectorFromListAux(Q3CheckListItem *item, VariantVector &v);
   Q3CheckListItem *opLoadVariantQueryListAux(T_VariantNode &node,
                                             Q3CheckListItem *ancestor, Q3CheckListItem *last_sibling);

   virtual void eventCacheModified();
   virtual void eventCacheClear();
   virtual void eventCacheNewNodes(std::set<SgNode *> &nodes);
   virtual void eventCacheRemovedNodes(std::set<SgNode *> &nodes);

   //TypeVariantQueries* getBuiltInOps() { return &m_variantQueries; }
   void insertCustomQuery(std::string desc, QRQueryOperator *op);
   virtual void opLoadVariantQueryList();
   virtual void opLoadCustomQueryList();

protected slots:
    void opLoadQueryList(bool isOn);
    void opLoadBuiltInVariantQueries();
    void opLoadVariantViewDesc();
    void domainViewClick(int id);
    void opQueryListClick(Q3ListViewItem*,const QPoint &,int);
    void domainSetExpMode(int id);
    void rangeResDoubleClick(Q3ListViewItem*,const QPoint&,int);
    void opSyncVariantQueryWithDesc(int id);
    void opSyncDescWithVariantQuery();

protected:

   typedef std::map<unsigned, std::pair<std::string,VariantVector> > TypeVariantQueries;
   typedef std::map<unsigned, std::pair<std::string,QRQueryOperator *> > TypeCustomQueries;

   std::map<VariantT,Q3CheckListItem*> m_mapOpVariantItem;
   std::map<Q3CheckListItem*,VariantT> m_mapOpItemVariant;
   TypeVariantQueries m_variantQueries;
   TypeCustomQueries m_customQueries;
   std::map<unsigned,Q3CheckListItem*> m_mapOpCustomQueryItem;
   QRQueryDomain::TypeDomainExpansion m_domainExpMode;
   std::map<Q3ListViewItem *,SgNode *> m_mapRangeItemNode;

   bool m_domainNeedsToExpand;
   QRQueryDomain m_queryDomain;
   QRQueryRange m_queryRange;

   SgNode *m_rootNode;

protected:
   QR_EXPORT(Q3Frame,TopFrame);
   QR_EXPORT(QLabel,DomainLabel);
   QR_EXPORT(Q3Frame,DomainFrame);
   QR_EXPORT(Q3ComboBox,DomainView);
   QR_EXPORT(QLabel,DomainInfo);
   QR_EXPORT(Q3ButtonGroup,DomainExpModes);
   QR_EXPORT(QRadioButton,DomainNoExpansion);
   QR_EXPORT(QRadioButton,DomainImmedChildren);
   QR_EXPORT(QRadioButton,DomainAllDesc);
   QR_EXPORT(QPushButton,DomainAdd);
   QR_EXPORT(QPushButton,DomainClear);
   QR_EXPORT(QPushButton,DomainSetDefault);
   QR_EXPORT(QSplitter,BtmFrame);
   QR_EXPORT(Q3Frame,BtmLeftFrame);
   QR_EXPORT(QLabel,OpLabel1);
   QR_EXPORT(Q3Frame,OpFrame);
   QR_EXPORT(QPushButton,OpCustomQueries);
   QR_EXPORT(Q3ComboBox,OpVariantQueryDesc);
   QR_EXPORT(Q3ListView,OpQueryList);
   QR_EXPORT(QPushButton,OpPerformQuery);
   QR_EXPORT(Q3Frame,BtmRightFrame);
   QR_EXPORT(QLabel,RangeLabel1);
   QR_EXPORT(Q3Frame,RangeFrame);
   QR_EXPORT(QLabel,RangeInfo);
   QR_EXPORT(Q3ListView,RangeRes);
   QR_EXPORT(QPushButton,RangeBroadcast);
   QR_EXPORT(QPushButton,RangeSetDomain);
   QR_EXPORT(QPushButton,RangeClear);
   QR_EXPORT(Q3ProgressBar,RangeQueryProgress);

   // layouts and spacers
   QR_EXPORT(Q3VBoxLayout,MasterLayout);
   QR_EXPORT(Q3VBoxLayout,TopFrameLayout);
   QR_EXPORT(Q3VBoxLayout,DomainFrameLayout);
   QR_EXPORT(Q3HBoxLayout,DomainLayout1);
   QR_EXPORT(Q3HBoxLayout,DomainLayout2);
   QR_EXPORT(Q3HBoxLayout,DomainExpModesLayout);
   QR_EXPORT(QSpacerItem,DomainSpacer);
   QR_EXPORT(Q3HBoxLayout,BtmFrameLayout);
   QR_EXPORT(Q3VBoxLayout,BtmLeftFrameLayout);
   QR_EXPORT(Q3VBoxLayout,OpFrameLayout);
   QR_EXPORT(QSpacerItem, OpSpacer);
   QR_EXPORT(Q3VBoxLayout,BtmRightFrameLayout);
   QR_EXPORT(Q3VBoxLayout,RangeFrameLayout);
   QR_EXPORT(Q3HBoxLayout,RangeButtonLayout);
};
}
#endif
