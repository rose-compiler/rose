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

#include "CompilerOutputWidget.h"

#include <CallGraph.h>
#include <GraphUpdate.h>

#include <QMdiSubWindow>

#include "AsmViewCreator.h"
#include "AstBrowserWidgetCreator.h"
#include "BeautifiedAstCreator.h"
#include "NodeInfoWidgetCreator.h"
#include "AstGraphWidgetCreator.h"
#include "MetricsKiviatCreator.h"
#include "RoseCodeEditCreator.h"
#include "CallGraphWidgetCreator.h"
#include "SrcBinViewCreator.h"

#ifdef WITHOUT_QROSE
#else
    #include "QRSourceBoxCreator.h"
    #include "QRQueryBoxCreator.h"
    #include "QRTreeBoxCreator.h"
#endif


MainWindow::MainWindow( int argc, char **argv, QWidget * p )
	: QMainWindow(p),
          pm( ProjectManager::instance() ),
          compilerEditor( NULL )
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
    ui.subWindowArea->registerSubWindow( new BeautifiedAstCreator());
    ui.subWindowArea->registerSubWindow( new MetricsKiviatCreator() );
    ui.subWindowArea->registerSubWindow( new AstBrowserWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new NodeInfoWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new CallGraphWidgetCreator() );
    ui.subWindowArea->registerSubWindow( new SrcBinViewCreator() );

#ifndef WITHOUT_QROSE
    ui.subWindowArea->registerSubWindow( new QRSourceBoxCreator() );
    ui.subWindowArea->registerSubWindow( new QRQueryBoxCreator() );
    ui.subWindowArea->registerSubWindow( new QRTreeBoxCreator() );
#endif

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


    buildupEditorToolbar(NULL);

    connect( pm->taskListWidget(), SIGNAL( clicked( const QString &, int ) ),
             this,    SLOT  ( showCompilerOutput( const QString &, int ) ) );
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
  settings.endGroup();


  QMainWindow::closeEvent(e);
}


void MainWindow::on_actionEditorSettings_triggered()
{
    RoseCodeEdit::showEditorSettingsDialog();
}



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

void MainWindow::on_subWindowArea_subWindowActivated(QMdiSubWindow * wnd)
{
    buildupEditorToolbar(wnd);
}


void MainWindow::showCompilerOutput( const QString &file, int line )
{
    if( compilerEditor == NULL )
    {
        QWidget * subWidget = new QWidget( );
        compilerEditor = new RoseCodeEdit( subWidget );
        
        QVBoxLayout * verticalLayout = new QVBoxLayout(subWidget);
        verticalLayout->addWidget( compilerEditor );

        subWidget->setWindowTitle( QString( "Source Code: ") + file );
        subWidget->setWindowIcon( QIcon( ":/util/NodeIcons/sourcefile.gif" ) );

        ui.subWindowArea->addSubWindow( subWidget );

        subWidget->show();
        compilerEditor->show();
    }

    compilerEditor->loadCppFile( file );
    compilerEditor->gotoPosition( line, 0 );
    compilerEditor->markAsWarning( line );
}

