#include "rose.h"
#include "CallGraphWidget.h"

#include "Project.h"

#include "DisplayGraphNode.h"
#include "SgNodeUtil.h"


#include "ui_LayoutControl.h"

#include <QToolButton>
#include <QMouseEvent>
#include <QMenu>

CallGraphWidget::CallGraphWidget(QWidget * par)
    : RoseGraphicsView(par)
{
    scene     = new QGraphicsScene();
    callGraph = new DisplayCallGraph(scene);

    callGraph->setOptimalDistance(80);


    QWidget * ctrlWdg =  callGraph->controlWidget();
    ctrlWdg->setParent(this);
    ctrlWdg->setVisible(false);


    QToolButton * but = new QToolButton(this);
    but->setIcon(QIcon(":/icons/start-here.png"));
    but->setCheckable(true);
    but->setIconSize(QSize(32,32));

    ctrlWdg->move(16,16);

    connect(but, SIGNAL(toggled(bool)),
            ctrlWdg, SLOT(setVisible(bool)) );

    setScene(scene);
}


CallGraphWidget::~CallGraphWidget()
{
    delete callGraph;
    delete scene;
}


void CallGraphWidget::updateGraph(SgNode * node, AstFilterInterface *)
{
    SgProject * proj = getProjectOf(node);
    if(!proj)
        return;

    ProjectManager * pm =ProjectManager::instance();
    int projId = pm->getIdFromSgProject(proj);

    callGraph->setCg( pm->getProject(projId)->getCallGraph());


    callGraph->addFunction(node,3);

    callGraph->on_cmdStartTimer_clicked();
    /*
    qreal update=1e10;
    int iter=0;
    while( update>0.7 && iter < 1000)
    {
        update = callGraph->springBasedLayoutIteration(1e-4);
        //qDebug() << "Iteration" << iter << "Update" << update;
        iter++;
    }
    //qDebug() << "Layout done in" << iter << "iterations";
     */
}





void CallGraphWidget::mouseReleaseEvent(QMouseEvent * ev)
{
    DisplayGraphNode * node = dynamic_cast<DisplayGraphNode*>(itemAt(ev->pos()));

    if(node && ev->button()==Qt::RightButton)
    {
        QList<QAction*> actionList;
        actionList << new QAction(tr("Remove"),NULL);
        if( QMenu::exec(actionList,mapToGlobal(ev->pos())) != 0)
            callGraph->deleteNode(node);

        qDeleteAll(actionList);
    }

    RoseGraphicsView::mouseReleaseEvent(ev);
}




