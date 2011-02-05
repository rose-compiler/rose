#include <QApplication>

#include <QGraphicsScene>
#include <QGraphicsView>

#include "DisplayNode.h"
#include "TreeLayoutGenerator.h"



DisplayTreeNode * generateLeftBTree(int length, QGraphicsScene * scene)
{
        DisplayTreeNode * root=new DisplayTreeNode(scene);

        DisplayTreeNode * curNode=root;
        for(int i=0; i<length; i++)
        {
                DisplayTreeNode  * left=curNode->addChild("left");
                curNode->addChild("longEntry");
                curNode->addChild("longEntry");
                curNode->addChild("right");

                curNode=left;
        }
        return root;
}

DisplayTreeNode * generateSmallInterleaving(QGraphicsScene * scene)
{
        DisplayTreeNode * root=new DisplayTreeNode(scene);

        DisplayTreeNode * left= root->addChild("left");
        DisplayTreeNode * right= root->addChild("right");

        left->addChild("leftleft");
        right->addChild("rightright");

        return root;
}


void generateFullTree(DisplayTreeNode *node, int height)
{
    if(height==0)
        return;

    DisplayTreeNode * child;

    child=node->addChild(QString("Short Entry"));
    generateFullTree(child,height-1);

    child=node->addChild(QString("Short Entry"));
    generateFullTree(child,height-1);

    child=node->addChild(QString("LongLongLongLongLong Entry"));
    generateFullTree(child,height-1);

    child=node->addChild(QString("a"));
    generateFullTree(child,height-1);

    /*
    for(int i=0; i<3; i++)
    {
        DisplayNode * child=node->addChild(QString("Entry %1").arg(i));
        generateFullTree(child,height-1);
    }*/
}

#if 0
int main ( int argc, char** argv)
{

        QApplication app(argc,argv);

        QGraphicsScene * scene = new QGraphicsScene();

        DisplayTreeNode * root=new DisplayTreeNode(NULL);


        DisplayTreeNode * left =   root->addChild("Left");
        /*DisplayNode * mid =  */  root->addChild("Mid");
        /*DisplayNode * right = */ root->addChild("Right");

        generateFullTree(left,2);
        //generateFullTree(right,2);

        /*
        DisplayNode * l = generateLeftBTree(1,scene);
        DisplayNode * r = generateLeftBTree(1,scene);
        DisplayNode * root= DisplayNode::mergeTrees(scene,"Merged",l,r);

        DisplayNode::simplifyTree(root);*/
        //DisplayNode * root= generateSmallInterleaving(&scene);

        TreeLayoutGenerator gen;
        gen.layoutTree(root);


        root->setScene(scene);

        QGraphicsView view;
        view.setScene(scene);
        view.setRenderHints(QPainter::Antialiasing);
        view.show();


        app.exec();
        delete scene;

        return 0;
}

#endif 
