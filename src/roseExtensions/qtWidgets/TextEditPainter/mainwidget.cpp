#include "mainwidget.h"

MainWidget::MainWidget(QWidget * par)
    : QWidget(par)
{
    ui.setupUi(this);
    connect(ui.spnCurvature,SIGNAL(valueChanged(double)),
            ui.textEdit,SLOT(setCurvature(double)));

/*
    connect(ui.cmdSetPos1,SIGNAL(clicked()),
    		this,SLOT(on_cmdSetPos1_clicked()));

    connect(ui.cmdSetPos2,SIGNAL(clicked()),
    		this,SLOT(on_cmdSetPos2_clicked()));
    		*/

    ui.textEdit->setCurvature(ui.spnCurvature->value());
}


void MainWidget::on_cmdSetPos1_clicked()
{
	ui.textEdit->setArrowBegin(ui.textEdit->textCursor());
	ui.spnPos1->setValue(ui.textEdit->textCursor().position());
}
void MainWidget::on_cmdSetPos2_clicked()
{
	ui.textEdit->setArrowEnd(ui.textEdit->textCursor());
	ui.spnPos2->setValue(ui.textEdit->textCursor().position());
}
