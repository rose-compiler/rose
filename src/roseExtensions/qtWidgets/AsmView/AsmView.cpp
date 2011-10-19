#include "sage3basic.h"
#include "AsmView.h"
#include "ItemTreeModel.h"
#include "ItemModelHelper.h"

#include "MetricsConfig.h"
#include "MetricBgDelegate.h"
#include "Project.h"

#include "AsmTreeNode.h"

#include "ui_ConfigureAsmViewDelegate.h"

#include <QHeaderView>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "MetricBgDelegate.h"

#include "SgNodeUtil.h"

AsmView::AsmView(QWidget * par)
    : RoseTreeView(par),curNode2(NULL)
{
    setRootIsDecorated(false);
    setAcceptDrops(true);

    connect(this, SIGNAL(nodeDropped(SgNode*,const QPoint&)),
            this, SLOT(on_nodeDropped(SgNode*,const QPoint&)));

}

AsmView::~AsmView()
{
}

void AsmView::asmNodeDropped(SgNode * node)
{
    if(isSgAsmFunction(node))
    {
        SgNode * par = node->get_parent();
        while( !isSgFile(par) && par !=0)
            par=par->get_parent();

        if(!par)
        {
            qWarning() << "AsmView::asmNodeDropped - Func Decl which has no File parent?!";
            return;
        }

        setNode(par);    //load file
        gotoNode(node);  //goto function
    }
    else
        setNode(node);

}

void AsmView::updateModel()
{
    ItemTreeNode * root;
    if(curNode2 == NULL)
        root = buildAsmTree(curNode,curFilter);
    else
    {
        Q_ASSERT(curNode && curNode2);
        root = buildAsmTreeDiff(curNode,curNode2,curFilter,curFilter);
    }

    model->setRoot(root);
    ItemTreeModel::setupView(this);


    expandAll();

    header()->resizeSection(0,0);
    header()->resizeSection(1,0);
    header()->resizeSection(2,0);
    header()->resizeSection(3,0);
    header()->resizeSections(QHeaderView::ResizeToContents);

    header()->resizeSection(4,3);

    ProjectManager * pm = ProjectManager::instance();
    if(curNode)
    {
        SgProject * project = getProjectOf(curNode);
        for(int i=0; i < 4; i++)
        {
            MetricBgDelegate * del = new MetricBgDelegate(this, "MetricsBgDelegate", pm->getMetricsConfig(project) );
            setItemDelegateForColumn(i,del);
        }
    }
    if(curNode2)
    {
        header()->setResizeMode(4,QHeaderView::Fixed);
        SgProject * project = getProjectOf(curNode2);
        for(int i=5; i < 9; i++)
        {
            MetricBgDelegate * del = new MetricBgDelegate(this, "MetricsBgDelegate", pm->getMetricsConfig(project) );
            setItemDelegateForColumn(i,del);
        }
    }

}

bool AsmView::gotoNode(SgNode * node)
{
    if(! isSgAsmFunction(node) && ! isSgAsmInstruction(node) )
        return false;


    // Traverse tree an look for item
    if(node==NULL)
        return false;

    QModelIndex res = findSgNodeInModel(node,*model);
    if(! res.isValid())
        return false;
    else
    {
        //jump to the bottom
        //setCurrentIndex(model->index(model->rowCount()-1,0));
        // then to current index --> current index is displayed at the top
        setCurrentIndex(res);
        return true;
    }
}

void AsmView::on_nodeDropped(SgNode * node, const QPoint & pos)
{
    QList<QAction *> actions;

    if(!curNode && !curNode2)
    {
        setNode(node);
        return;
    }

    QObject * actionParent = new QObject();

    QAction * actionReplaceFirst  = new QAction(tr("Replace first"),actionParent);
    QAction * actionReplaceSecond = new QAction(tr("Replace second"),actionParent);
    QAction * actionSetAsOnly =     new QAction(tr("Set as only source (no diff)"),actionParent);

    QAction * actionReplace   = new QAction(tr("Replace"),actionParent);
    QAction * actionNewSecond = new QAction(tr("Set as second (diff)"),actionParent);


    if(curNode2)
        actions << actionReplaceFirst << actionReplaceSecond << actionSetAsOnly;
    else
        actions << actionReplace << actionNewSecond;

    QAction * res = QMenu::exec ( actions,mapToGlobal(pos));


    if     (res == actionReplaceFirst || res == actionReplace || res == actionSetAsOnly)
        curNode=node;
    else if(res == actionReplaceSecond || res == actionNewSecond)
        curNode2= node;

    if(res==actionSetAsOnly)
        curNode2=NULL;

    delete actionParent;

    updateModel();
}

void AsmView::contextMenuEvent( QContextMenuEvent *event )
{
    if(curNode == NULL) // don't show context-menu when view is empty
        return;

    QObject *actionParent = new QObject();
    QAction *actionSetDelegate = new QAction( tr( "Set Background Options" ), actionParent );

    QList<QAction *> actions;

    actions << actionSetDelegate;

    QAction * res = QMenu::exec( actions, mapToGlobal( event->pos() ) );

    if( res == actionSetDelegate )
    {
        QDialog dialog;
        Ui::ConfigureAsmViewDelegate dialogUi;

        dialogUi.setupUi( &dialog );

        MetricBgDelegate *leftDelegate(
                dynamic_cast<MetricBgDelegate *>( itemDelegateForColumn( 0 ) )
                );
        MetricBgDelegate *rightDelegate(
                dynamic_cast<MetricBgDelegate *>( itemDelegateForColumn( 5 ) )
                );

        if( leftDelegate )
        {
            leftDelegate->setupConfigWidget( dialogUi.leftDelegateBox );
        }
        if( !curNode )
            dialogUi.leftDelegateBox->hide();

        if( rightDelegate )
        {
            rightDelegate->setupConfigWidget( dialogUi.rightDelegateBox );
        }
        if( !curNode2 )
            dialogUi.rightDelegateBox->hide();

        if( dialog.exec() != QDialog::Accepted )
            return;

        if( leftDelegate )
        {
            leftDelegate->applyConfigWidget( dialogUi.leftDelegateBox );
            setItemDelegateForColumn( 1, leftDelegate->copy() );
            setItemDelegateForColumn( 2, leftDelegate->copy() );
            setItemDelegateForColumn( 3, leftDelegate->copy() );
        }

        if( rightDelegate )
        {
            rightDelegate->applyConfigWidget( dialogUi.rightDelegateBox );
            setItemDelegateForColumn( 6, rightDelegate->copy() );
            setItemDelegateForColumn( 7, rightDelegate->copy() );
            setItemDelegateForColumn( 8, rightDelegate->copy() );
        }
    }

    delete actionParent;
}


void AsmView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    removeAllArrows();
    if(selected.size() ==1 && selected.front().height() ==1)
    {
        QModelIndex ind = selected.front().bottomRight();
        addArrows(3,0, ind);
        // TODO check if it works in diffview
        if(model->columnCount() > 5)
            addArrows(8,5,ind);
    }

    RoseTreeView::selectionChanged(selected,deselected);
}


void AsmView::addArrows(int drawColumn,int addressColumn, const QModelIndex & startInd)
{
    QModelIndex ind = model->index(startInd.row(),drawColumn,startInd.parent());

    SgNode * sg = qvariant_cast<SgNode*>(model->data(ind,SgNodeRole));

    rose_addr_t address;
    bool success = findNextInstruction(sg,address);
    if(success)
    {
        QString strAddress = QString("%1").arg(address,0,16);
        QVariant varAddress(strAddress);
        QModelIndex targetInd = findVariantInModel(varAddress,*model,Qt::DisplayRole,QModelIndex(),addressColumn);
        if(targetInd.isValid())
        {
            QModelIndex from = model->index(ind.row(),drawColumn,ind.parent());
            QModelIndex to = model->index(targetInd.row(),drawColumn,targetInd.parent());
            addArrow(from,to);
        }
    }

}

