#include <QApplication>

#include <QGraphicsScene>
#include <QGraphicsView>

#include "DisplayNode.h"
#include "TreeLayoutGenerator.h"



DisplayNode * generateLeftBTree(int length, QGraphicsScene * scene)
{
	DisplayNode * root=new DisplayNode(scene);

	DisplayNode * curNode=root;
	for(int i=0; i<length; i++)
	{
		DisplayNode  * left=curNode->addChild("left");
		curNode->addChild("longEntry");
		curNode->addChild("longEntry");
		curNode->addChild("right");

		curNode=left;
	}
	return root;
}

DisplayNode * generateSmallInterleaving(QGraphicsScene * scene)
{
	DisplayNode * root=new DisplayNode(scene);

	DisplayNode * left= root->addChild("left");
	DisplayNode * right= root->addChild("right");

	left->addChild("leftleft");
	right->addChild("rightright");

	return root;
}


void generateFullTree(DisplayNode *node, int height)
{
    if(height==0)
        return;

    DisplayNode * child;

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

int main ( int argc, char** argv)
{

	QApplication app(argc,argv);

	QGraphicsScene * scene = new QGraphicsScene();

	DisplayNode * root=new DisplayNode(NULL);


	DisplayNode * left = root->addChild("Left");
	DisplayNode * mid =  root->addChild("Mid");
	DisplayNode * right =  root->addChild("Right");

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
