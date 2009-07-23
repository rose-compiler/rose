#include "rose.h"

#include "MainWindow.h"

#include <QHeaderView>
#include <QApplication>
#include <QDir>
#include <QCloseEvent>
#include <QSettings>
#include <QFileDialog>

#include <QDebug>

#include "AstFilters.h"
#include "MetricsConfig.h"
#include "MetricBgDelegate.h"

#include "Project.h"

#include "QCodeEdit/qcodeedit.h"
#include "RoseCodeEdit.h"


#include <CallGraph.h>
#include <GraphUpdate.h>

#include <QMdiSubWindow>

#include "AsmViewCreator.h"
#include "AstBrowserWidgetCreator.h"
#include "NodeInfoWidgetCreator.h"
#include "AstGraphWidgetCreator.h"
#include "MetricsKiviatCreator.h"
#include "RoseCodeEditCreator.h"
#include "CallGraphWidgetCreator.h"
#include "SrcBinViewCreator.h"

MainWindow::MainWindow( int argc, char **argv, QWidget * p )
	: QMainWindow(p),
          f1( new AstFilterAll() ),
          f2( new AstFilterAll() ),
          pm( ProjectManager::instance() )
{
    //dbgFunc();

    ui.setupUi(this);
    showMaximized();

    pm->setTaskListWidget(ui.taskList);
    pm->setTaskMsgOutputWidget(ui.wdgTaskOutput);

    pm->loadProjectState( argc, argv );

    if(pm->getProjectCount() <1)
        pm->addProject("Project1");

    //Menu
    connect( ui.actionQuit             , SIGNAL( triggered() ),
             qApp                      , SLOT  ( quit() ) );
    connect( ui.actionCascade          , SIGNAL( triggered() ),
             ui.subWindowArea          , SLOT  (cascadeSubWindows() ) );
    connect( ui.actionTile             , SIGNAL( triggered() ),
             ui.subWindowArea          , SLOT  (tileSubWindows() ) );

    ui.subWindowArea->tileSubWindows();

    //ui.kvtMetrics->init( project );

    // And Filter
    //connect( ui.roseFileCmbBox, SIGNAL( selectedFileChanged( AstFilterInterface * ) ),
             //this             , SLOT  ( setFilter1( AstFilterInterface * ) ) );
    //connect( ui.metricFilter  , SIGNAL( filterChanged( AstFilterInterface * ) )      ,
             //this             , SLOT  ( setFilter2( AstFilterInterface * ) ) );

    // Kiviat Info
    //ui.kvtInfo->setKiviat( ui.kvtMetrics );
    //connect( ui.kvtInfo   , SIGNAL( clicked( SgNode * ) ),
    //         ui.kvtMetrics, SLOT  ( updateView( SgNode * ) ) );

    // setup SubWindowFactory
    ui.subWindowArea->registerSubWindow( new AsmViewCreator() );
    ui.subWindowArea->registerSubWindow( new RoseCodeEditCreator() );
    ui.subWindowArea->registerSubWindow( new AstGraphWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new MetricsKiviatCreator() );
    ui.subWindowArea->registerSubWindow( new AstBrowserWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new NodeInfoWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new CallGraphWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new SrcBinViewCreator() );
    

    foreach( QAction *action, ui.subWindowArea->getActions() )
    {
        ui.menuNew_SubWindow->addAction( action );
        ui.toolBarNewSubWidget->addAction( action );
    }

    //Restore State (Positions of dock-windows etc)
    QSettings settings;
    settings.beginGroup("WindowState");

    QByteArray d = settings.value("mainwindow").toByteArray();
    if(d.size() > 0)
      restoreState(d);
    settings.endGroup();

    restoreMdiState();

    buildupEditorToolbar(NULL);
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
  //settings.setValue("mdiview",ui.mdiArea->saveGeometry());
  settings.endGroup();

  saveMdiState();

  QMainWindow::closeEvent(e);
}



void MainWindow::saveMdiState()
{
    QSettings settings;

    /*settings.remove("MdiArea");

    settings.beginGroup("MdiArea");
    QList<QMdiSubWindow *> subWdgs = ui.mdiArea->subWindowList();
    settings.beginWriteArray("SubWidgets");
    for(int i=0; i<subWdgs.size(); i++)
    {
        settings.setArrayIndex(i);

        settings.setValue("geometry", subWdgs[i]->saveGeometry());

        AsmView *          asmView    = subWdgs[i]->findChild<AsmView*> ();
        RoseCodeEdit *     codeEdit   = subWdgs[i]->findChild<RoseCodeEdit*> ();
        AstGraphWidget *   graphWdg   = subWdgs[i]->findChild<AstGraphWidget*> ();
        MetricsKiviat *    metKiviat  = subWdgs[i]->findChild<MetricsKiviat*> ();
        AstBrowserWidget * astBrowser = subWdgs[i]->findChild<AstBrowserWidget*> ();
        NodeInfoWidget *   nodeInfo   = subWdgs[i]->findChild<NodeInfoWidget*> ();
        CallGraphWidget*   cgWidget   = subWdgs[i]->findChild<CallGraphWidget*>();
        SrcBinView *       srcBinView = subWdgs[i]->findChild<SrcBinView*> ();

        if     (asmView)    settings.setValue("type", "AsmView");
        else if(codeEdit)   settings.setValue("type", "RoseCodeEdit");
        else if(graphWdg)   settings.setValue("type", "AstGraphWidget");
        else if(metKiviat)  settings.setValue("type", "MetricsKiviat");
        else if(astBrowser) settings.setValue("type", "AstBrowser" );
        else if(nodeInfo)   settings.setValue("type", "NodeInfo" );
        else if(cgWidget)   settings.setValue("type", "CallGraphWidget");
        else if(srcBinView) settings.setValue("type", "SrcBinView" );
        else qDebug() << "Unknown WidgetType in MdiArea, cannot be stored";
    }
    settings.endArray();
    settings.endGroup();*/
}

void MainWindow::restoreMdiState()
{
    QSettings s;
    /*s.beginGroup("MdiArea");

    int size = s.beginReadArray("SubWidgets");
    for(int i=0; i<size; i++)
    {
        s.setArrayIndex(i);
        QString type = s.value("type").toString();
        QMdiSubWindow * w = NULL;

        if     (type == "AsmView")
            w = addMdiSubWidget(new AsmView(),           "Assembler View");
        else if(type == "RoseCodeEdit")
            w = addMdiSubWidget(new RoseCodeEdit(),      "Code Editor");
        else if(type == "AstGraphWidget")
            w = addMdiSubWidget(new AstGraphWidget(),    "Graph Widget");
        else if(type == "MetricsKiviat")
            w = addMdiSubWidget(new MetricsKiviat(),     "MetricsKiviat");
        else if( type == "AstBrowser" )
            w = addMdiSubWidget( new AstBrowserWidget(), "Ast Browser" );
        else if( type == "NodeInfo" )
            w = addMdiSubWidget( new NodeInfoWidget(),   "SgNode Properties" );
        else if( type == "CallGraphWidget")
            w = addMdiSubWidget( new CallGraphWidget(), "CallGraphWidget");
        else if( type == "SrcBinView" )
            w = addMdiSubWidget( new SrcBinView(),       "Source to Assembly Demo" );
        else
            qDebug() << "QMainWindow::restoreMdiState - unknown type" << type;

        if( w )
        {
            QByteArray geom = s.value("geometry").toByteArray();
            if(geom.size() > 0)
                w->restoreGeometry(geom);
        }
    }
    s.endArray();

    s.endGroup();*/
}

void MainWindow::on_actionEditorSettings_triggered()
{
    RoseCodeEdit::showEditorSettingsDialog();
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
    //AstFilterAnd filter( f1, f2 );

    //ui.astBrowserWidget->setFilter( &filter );
    //ui.bAstView->setFilter( &filter );
}


/*void MainWindow::on_actionNewAssemblerView_triggered()
{
    addMdiSubWidget(new AsmView(), "Assembler View");
}

void MainWindow::on_actionNewCodeEditor_triggered()
{
    RoseCodeEdit * e = new RoseCodeEdit();
    e->enableBreakPointEdit();
    addMdiSubWidget(e, "Code Editor");
}

void MainWindow::on_actionNewGraphWidget_triggered()
{
    addMdiSubWidget(new AstGraphWidget(),"Graph View");
}

void MainWindow::on_actionNewMetricsKiviat_triggered()
{
    addMdiSubWidget(new MetricsKiviat(), "Metrics Kiviat");
}

void MainWindow::on_actionNewAstBrowser_triggered()
{
    addMdiSubWidget(new AstBrowserWidget(), "Ast Browser");
}

void MainWindow::on_actionNewPropertyView_triggered()
{
    addMdiSubWidget(new NodeInfoWidget(), "SgNode Properties");
}

void MainWindow::on_actionNewCallGraphWidget_triggered()
{
    addMdiSubWidget(new CallGraphWidget(), "CallGraphWidget");
}

void MainWindow::on_actionNewSrcBinView_triggered()
{
    addMdiSubWidget(new SrcBinView(), "Source to Assembly Demo");
}*/

void MainWindow::on_actionSaveAs_triggered()
{
    /*
    RoseCodeEdit * codeEdit = ui.mdiArea->activeSubWindow()->findChild<RoseCodeEdit*>();
    Q_ASSERT(codeEdit);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "", tr("C++ files (*.cpp *.C *.h)"));

    codeEdit->save(fileName);
    */
}

void MainWindow::on_actionOpen_triggered()
{
    /*QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                               "", tr("C++ files (*.cpp *.C *.h)"));

    RoseCodeEdit * w = new RoseCodeEdit();
    addMdiSubWidget(w, "Code Editor");

    w->loadCppFile(fileName);*/
}

/*QMdiSubWindow * MainWindow::addMdiSubWidget(QWidget * widget,const QString & caption)
{
    widget->setWindowTitle(caption);
    QWidget * subWidget = new QWidget();
    widget->setParent(subWidget);
    QVBoxLayout * verticalLayout = new QVBoxLayout(subWidget);
    verticalLayout->addWidget(widget);

    subWidget->setWindowTitle(widget->windowTitle());

    QMdiSubWindow * res = ui.mdiArea->addSubWindow(subWidget);

    subWidget->show();
    widget->show();

    return res;
}*/



void MainWindow::buildupEditorToolbar(QWidget * wnd)
{

    RoseCodeEdit * edit = 0;
    if(wnd)
        edit = wnd->findChild<RoseCodeEdit*>();

    ui.editorToolbar->clear();
    ui.menuEdit->clear();
    if(!edit)
    {
        ui.editorToolbar->setEnabled(false);
        ui.menuEdit->setEnabled(false);
        ui.actionSaveAs->setEnabled(false);
        ui.actionSave->setEnabled(false);

        ui.editorToolbar->addAction(RoseCodeEdit::getDisabledActions("undo"));
        ui.editorToolbar->addAction(RoseCodeEdit::getDisabledActions("redo"));
        ui.editorToolbar->addSeparator();
        ui.editorToolbar->addAction(RoseCodeEdit::getDisabledActions("cut"));
        ui.editorToolbar->addAction(RoseCodeEdit::getDisabledActions("copy"));
        ui.editorToolbar->addAction(RoseCodeEdit::getDisabledActions("paste"));

        ui.menuEdit->addAction(RoseCodeEdit::getDisabledActions("undo"));
        ui.menuEdit->addAction(RoseCodeEdit::getDisabledActions("redo"));
        ui.menuEdit->addSeparator();
        ui.menuEdit->addAction(RoseCodeEdit::getDisabledActions("cut"));
        ui.menuEdit->addAction(RoseCodeEdit::getDisabledActions("copy"));
        ui.menuEdit->addAction(RoseCodeEdit::getDisabledActions("paste"));

        return;
    }

    disconnect(ui.actionSave);
    connect(ui.actionSave,SIGNAL(triggered()),edit,SLOT(save()));

    ui.editorToolbar->setEnabled(true);
    ui.menuEdit->setEnabled(true);
    ui.actionSaveAs->setEnabled(true);
    ui.actionSave->setEnabled(true);


    ui.editorToolbar->addAction(edit->action("undo"));
    ui.editorToolbar->addAction(edit->action("redo"));
    ui.editorToolbar->addSeparator();
    ui.editorToolbar->addAction(edit->action("cut"));
    ui.editorToolbar->addAction(edit->action("copy"));
    ui.editorToolbar->addAction(edit->action("paste"));

    ui.menuEdit->addAction(edit->action("undo"));
    ui.menuEdit->addAction(edit->action("redo"));
    ui.menuEdit->addSeparator();
    ui.menuEdit->addAction(edit->action("cut"));
    ui.menuEdit->addAction(edit->action("copy"));
    ui.menuEdit->addAction(edit->action("paste"));
}

//void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow * wnd)
//{
    //buildupEditorToolbar(wnd);
//}


//#include "GccTask.h"

//void MainWindow::on_cmdSubmitTest_clicked()
//{
    //QStringList args;
    //args << "-Wall" <<"inputTestErr.cpp";
    //ui.taskList->submitTask( new GccCompileTask("inputTestErr.cpp","inputTest.out"));
//}

/*
void MainWindow::on_cmdExecScript_clicked()
{
    scriptEngine->evaluate(ui.txtScriptInput->toPlainText());

}*/



