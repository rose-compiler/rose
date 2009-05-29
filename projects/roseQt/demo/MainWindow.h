#ifndef ASTTREEVIEW_H
#define ASTTREEVIEW_H


#include "ui_MainWindow.h"

#include <QMainWindow>

class QFormatScheme;
class QLanguageFactory;

class SgNode;
class AstFilterInterface;


class QCodeEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(SgProject * project, QWidget * parent= 0);
        virtual ~MainWindow();

    public slots:
        void setFilter1( AstFilterInterface *filter );
        void setFilter2( AstFilterInterface *filter );


    protected:
        Ui::MainWindow ui;
        AstFilterInterface *f1;
        AstFilterInterface *f2;

    private:
        void emitFilterChanged();
        QCodeEdit * editorWrapper;

};

#endif
