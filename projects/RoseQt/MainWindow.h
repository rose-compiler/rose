#ifndef ASTTREEVIEW_H
#define ASTTREEVIEW_H


#include "ui_MainWindow.h"

#include <QMainWindow>


class QFormatScheme;
class QLanguageFactory;

class SgNode;
class AstFilterInterface;


class QCodeEdit;

class ProjectManager;

//#include <QtScript>
//#include <QtScriptTools/QScriptEngineDebugger>

namespace Ui { class DebugDialog; }


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow( QWidget * parent= 0);
        virtual ~MainWindow();

    public slots:
        void setFilter1( AstFilterInterface *filter );
        void setFilter2( AstFilterInterface *filter );

        void on_actionNewAssemblerView_triggered();
        void on_actionNewCodeEditor_triggered();
        void on_actionNewGraphWidget_triggered();
        void on_actionNewMetricsKiviat_triggered();
        void on_actionNewAstBrowser_triggered();
        void on_actionNewPropertyView_triggered();
        void on_actionNewSrcBinView_triggered();

        void on_actionSaveAs_triggered();
        void on_actionOpen_triggered();

        //void on_cmdExecScript_clicked();
        void on_cmdSubmitTest_clicked();

        void on_mdiArea_subWindowActivated(QMdiSubWindow * wnd);
        void on_actionEditorSettings_triggered();


    protected:

        Ui::MainWindow ui;
        AstFilterInterface *f1;
        AstFilterInterface *f2;

        virtual void closeEvent( QCloseEvent * e);

    private:
        void restoreMdiState();
        void saveMdiState();

        void emitFilterChanged();

        QMdiSubWindow* addMdiSubWidget(QWidget * widget, const QString & caption);
        void buildupEditorToolbar(QWidget * w);
        ProjectManager *pm;

        QCodeEdit * editorWrapper;
        //QScriptEngine * scriptEngine;
        //QScriptEngineDebugger * scriptDebugger;
};

#endif
