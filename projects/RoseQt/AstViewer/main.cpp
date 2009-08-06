#include "rose.h"
#include "ui_MyWidget.h"


#include <QApplication>


int main(int argc, char**argv)
{
	SgProject * proj = frontend(argc,argv);
	QApplication app (argc,argv);

	QWidget * wdg = new QWidget();
	
	Ui::MyWidget ui;
	ui.setupUi(wdg);
	
	wdg->show();
	ui.bAstView->setNode(proj);

	app.exec();	
	
	return 0;
};
