#ifndef ASMVIEW_H
#define ASMVIEW_H

#include "RoseTreeView.h"

class QContextMenuEvent;

class AstFilterInterface;
class ItemTreeModel;
class SgNode;

class AsmView : public RoseTreeView
{
    Q_OBJECT
    public:
        AsmView(QWidget * parent = NULL);
        virtual ~AsmView();


    public slots:
        /// Sets the Item corresponding to the node as currentIndex
        /// (i.e. selects it, scrolls that the item is visible)
        virtual bool gotoNode( SgNode *node );

    protected slots:
        void asmNodeDropped(SgNode * node);
        void on_nodeDropped(SgNode * node, const QPoint & pos);

        void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    protected:
        void addArrows(int drawColumn,int addressColumn, const QModelIndex & startInd);
        void updateModel();

        void contextMenuEvent( QContextMenuEvent *event );

        SgNode * curNode2;

};

#endif

