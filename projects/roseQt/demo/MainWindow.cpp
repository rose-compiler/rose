#include "rose.h"

#include "MainWindow.h"

#include <QHeaderView>
#include <QApplication>
#include <QDir>
#include <QCloseEvent>
#include <QSettings>

#include <QDebug>

#include "AstFilters.h"
#include "MetricsConfig.h"
#include "MetricBgDelegate.h"

#include "Project.h"

#include "QCodeEdit/qcodeedit.h"


#include <CallGraph.h>
#include <GraphUpdate.h>







MainWindow::MainWindow( QWidget * p )
	: QMainWindow(p),
          f1( new AstFilterAll() ),
          f2( new AstFilterAll() ),
          pm( ProjectManager::instance() )
{
    //dbgFunc();

    ui.setupUi(this);
    showMaximized();



    editorWrapper = new QCodeEdit(ui.roseCodeEdit ,this);

    editorWrapper
        ->addPanel("Line Mark Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F6"));

    editorWrapper
        ->addPanel("Line Number Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F11"));

    editorWrapper
        ->addPanel("Fold Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F9"));

    //editorWrapper
    //    ->addPanel("Line Change Panel", QCodeEdit::West, true);

    editorWrapper
        ->addPanel("Status Panel", QCodeEdit::South, true);

    editorWrapper
        ->addPanel("Search Replace Panel", QCodeEdit::South);

    /*
    ui.editorToolbar->addAction(ui.codeEdit->action("undo"));
    ui.editorToolbar->addAction(ui.codeEdit->action("redo"));
    ui.editorToolbar->addSeparator();
    ui.editorToolbar->addAction(ui.codeEdit->action("cut"));
    ui.editorToolbar->addAction(ui.codeEdit->action("copy"));
    ui.editorToolbar->addAction(ui.codeEdit->action("paste"));

    ui.menuEdit->addAction(ui.codeEdit->action("undo"));
    ui.menuEdit->addAction(ui.codeEdit->action("redo"));
    ui.menuEdit->addSeparator();
    ui.menuEdit->addAction(ui.codeEdit->action("cut"));
    ui.menuEdit->addAction(ui.codeEdit->action("copy"));
    ui.menuEdit->addAction(ui.codeEdit->action("paste"));
    */





    pm->setTaskListWidget(ui.taskList);
    pm->setTaskMsgOutputWidget(ui.wdgTaskOutput);
    pm->loadProjectState();

    if(pm->getProjectCount() <1)
        pm->addProject("Project1");

    SgProject *project = pm->getSgProject(0);

    /*ui.roseFileCmbBox->setProject( project );
    ui.astBrowserWidget->setNode( project );
    ui.bAstView->setNode( project );*/

    ui.toolBar->addWidget(ui.asmInstructionsBar);

    //Menu
    connect( ui.actionQuit             , SIGNAL( triggered() ),
             qApp                      , SLOT  ( quit() ) );
    connect( ui.actionCascade          , SIGNAL( triggered() ),
             ui.mdiArea                , SLOT  (cascadeSubWindows() ) );
    connect( ui.actionTile             , SIGNAL( triggered() ),
             ui.mdiArea                , SLOT  (tileSubWindows() ) );
    /*connect( ui.actionMetric_Attributes, SIGNAL( triggered() ),
             global                    , SLOT  ( configureSingle() ) );*/
    ui.mdiArea->tileSubWindows();

    ui.kvtMetrics->init( project );

    // And Filter
    connect( ui.roseFileCmbBox, SIGNAL( selectedFileChanged( AstFilterInterface * ) ),
             this             , SLOT  ( setFilter1( AstFilterInterface * ) ) );
    connect( ui.metricFilter  , SIGNAL( filterChanged( AstFilterInterface * ) )      ,
             this             , SLOT  ( setFilter2( AstFilterInterface * ) ) );

    // Kiviat Info
    ui.kvtInfo->setKiviat( ui.kvtMetrics );
    connect( ui.kvtInfo   , SIGNAL( clicked( SgNode * ) ),
             ui.kvtMetrics, SLOT  ( updateView( SgNode * ) ) );


    ui.asmView->setNode(project);
    ui.asmView->setItemDelegateForColumn(0, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(1, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(2, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(3, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(5, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(6, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(7, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));
    ui.asmView->setItemDelegateForColumn(8, new MetricBgDelegate( ui.asmView, "MetricsBgDelegate", pm->getMetricsConfig( project ) ));



    //Restore State (Positions of dock-windows etc)
    QSettings settings;
    settings.beginGroup("WindowState");

    QByteArray d = settings.value("mainwindow").toByteArray();
    if(d.size() > 0)
      restoreState(d);

    QByteArray mdi = settings.value("mdiview").toByteArray();
    if(mdi.size() > 0)
        ui.mdiArea->restoreGeometry(mdi);

    settings.endGroup();



}

MainWindow::~MainWindow()
{
    pm->storeProjectState();
}

void MainWindow::closeEvent ( QCloseEvent * e)
{
  QSettings settings;
  settings.beginGroup("WindowState");
  settings.setValue("mainwindow",saveState());
  settings.setValue("mdiview",ui.mdiArea->saveGeometry());
  settings.endGroup();

  QMainWindow::closeEvent(e);
}


void MainWindow::setFilter1( AstFilterInterface *filter )
{
    if(f1)delete f1;

    if( filter )
        f1 = filter->copy();
    else
        f1 = new AstFilterAll();

    emitFilterChanged();
}
void MainWindow::setFilter2( AstFilterInterface *filter )
{
    if(f2) delete f2;

    if( filter )
        f2 = filter->copy();
    else
        f2 = new AstFilterAll();

    emitFilterChanged();
}

void MainWindow::emitFilterChanged()
{
    AstFilterAnd filter( f1, f2 );

    ui.astBrowserWidget->setFilter( &filter );
    ui.bAstView->setFilter( &filter );
}


void MainWindow::on_actionNewAssemblerView_triggered()
{
    AsmView * w = new AsmView();
    w->setWindowTitle("Assembler View");
    addMdiSubWidget(w);
}

void MainWindow::on_actionNewCodeEditor_triggered()
{
    RoseCodeEdit * w = new RoseCodeEdit();
    w->setWindowTitle("Code Editor");
    addMdiSubWidget(w);
}

void MainWindow::addMdiSubWidget(QWidget * widget)
{
    QWidget * subWidget = new QWidget();
    widget->setParent(subWidget);
    QVBoxLayout * verticalLayout = new QVBoxLayout(subWidget);
    verticalLayout->addWidget(widget);

    subWidget->setWindowTitle(widget->windowTitle());

    ui.mdiArea->addSubWindow(subWidget);

    subWidget->show();
    widget->show();
}


#include "GccTask.h"

void MainWindow::on_cmdSubmitTest_clicked()
{
    QStringList args;
    args << "-Wall" <<"inputTestErr.cpp";
    ui.taskList->submitTask( new GccCompileTask("inputTestErr.cpp","inputTest.out"));
}

/*
void MainWindow::on_cmdExecScript_clicked()
{
    scriptEngine->evaluate(ui.txtScriptInput->toPlainText());

}*/



