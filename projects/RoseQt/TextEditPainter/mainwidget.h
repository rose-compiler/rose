#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include "ui_DemoWidget.h"

class MainWidget : public QWidget
{
    Q_OBJECT

    public:
        MainWidget(QWidget * par=0);
        virtual ~MainWidget() {}


    protected:
        Ui::DemoWidget ui;

    protected slots:
        void on_cmdSetPos1_clicked();
        void on_cmdSetPos2_clicked();

};


#endif
