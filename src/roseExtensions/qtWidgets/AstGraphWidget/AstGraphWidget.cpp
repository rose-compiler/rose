#include "sage3basic.h"
#include "AstGraphWidget.h"

#include "SageMimeData.h"
#include "Project.h"


#include <cmath>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDropEvent>


#include <QDebug>
#include <QSet>

#include "TreeLayoutGenerator.h"
#include "AstFilters.h"
#include "DisplayGraphNode.h"

#include "SgNodeUtil.h"

#include "AstDisplayInfo.h"


AstGraphWidget::AstGraphWidget(QWidget * par)
        :       RoseGraphicsView(par),
                scene(NULL),
                root(NULL)
{
        scene=new QGraphicsScene();
        setScene(scene);
}

AstGraphWidget::~AstGraphWidget()
{
    delete scene;
}


void AstGraphWidget::updateGraph(SgNode * node, AstFilterInterface * filter)
{
    if(root)
        delete root;

        root=NULL;

        if(node == NULL)
            return;


        DisplayTreeGenerator gen;
        root = gen.generateTree(node,filter);

        qDebug() << "Simplifying Tree";
        DisplayTreeNode::simplifyTree(root);
        qDebug() << "Done";

        TreeLayoutGenerator layouter;
        layouter.layoutTree(root);

        root->setScene(scene);
}
