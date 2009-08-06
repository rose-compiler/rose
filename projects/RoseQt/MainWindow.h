#ifndef ASTTREEVIEW_H
#define ASTTREEVIEW_H


#include "ui_MainWindow.h"

#include <QMainWindow>
#include <QPointer>


class QFormatScheme;
class QLanguageFactory;

class SgNode;
class AstFilterInterface;


class RoseCodeEdit;

class ProjectManager;

class QMdiSubWindow;

//#include <QtScript>
//#include <QtScriptTools/QScriptEngineDebugger>

namespace Ui { class DebugDialog; }


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow( int argc, char **argv, QWidget * parent= 0);
        virtual ~MainWindow();

    public slots:

        void on_subWindowArea_subWindowActivated(QMdiSubWindow * wnd);
        void on_actionEditorSettings_triggered();

        void showCompilerOutput( const QString &file, int line );

    protected:

        Ui::MainWindow ui;

        virtual void closeEvent( QCloseEvent * e);

    private:
        void buildupEditorToolbar(QWidget * w);
        ProjectManager *pm;

        QPointer<RoseCodeEdit> compilerEditor;
        //QScriptEngine * scriptEngine;
        //QScriptEngineDebugger * scriptDebugger;
};

#endif
