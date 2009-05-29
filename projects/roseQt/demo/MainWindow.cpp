#include "rose.h"

#include "MainWindow.h"

#include <QHeaderView>
#include <QApplication>
#include <QDir>

#include <QDebug>

#include "util/AstFilters.h"
#include "MetricsConfig/MetricsConfig.h"


#include "qcodeedit.h"

MainWindow::MainWindow(SgProject * project, QWidget * p)
	: QMainWindow(p),
          f1( new AstFilterAll() ),
          f2( new AstFilterAll() )
{
        MetricsConfig *global = new MetricsConfig( "", project );

	ui.setupUi(this);
	ui.roseFileCmbBox->setProject(project);
	ui.astBrowserWidget->setNode(project);
	ui.bAstView->setNode(project);
        //ui.asmInstructionsBar->setNode( project );

	//Menu
	connect( ui.actionQuit             , SIGNAL( triggered() ), qApp      , SLOT( quit() ) );
	connect( ui.actionCascade          , SIGNAL( triggered() ), ui.mdiArea, SLOT(cascadeSubWindows() ) );
	connect( ui.actionTile             , SIGNAL( triggered() ), ui.mdiArea, SLOT(tileSubWindows() ) );
        connect( ui.actionMetric_Attributes, SIGNAL( triggered() ), global    , SLOT( configureSingle() ) );
	ui.mdiArea->tileSubWindows();

	//Kiviat Metrics
	/*connect( ui.astBrowserWidget, SIGNAL( clicked( SgNode* ) )      , ui.kvtMetrics, SLOT( updateView( SgNode* ) ) );
	connect( ui.bAstView        , SIGNAL( clicked( SgNode* ) )      , ui.kvtMetrics, SLOT( updateView( SgNode* ) ) );
	connect( ui.astBrowserWidget, SIGNAL( doubleClicked( SgNode* ) ), ui.kvtMetrics, SLOT( addNode( SgNode* ) ) );
	connect( ui.bAstView        , SIGNAL( coubleClicked( SgNode* ) ), ui.kvtMetrics, SLOT( addNode( SgNode* ) ) );*/
	ui.kvtMetrics->init( project );

    // And Filter
    connect( ui.roseFileCmbBox, SIGNAL( selectedFileChanged( AstFilterInterface * ) ), this, SLOT( setFilter1( AstFilterInterface * ) ) );
    connect( ui.metricFilter  , SIGNAL( filterChanged( AstFilterInterface * ) )    , this, SLOT( setFilter2( AstFilterInterface * ) ) );

    // Kiviat Info
    ui.kvtInfo->setKiviat( ui.kvtMetrics );
    connect( ui.kvtInfo, SIGNAL( clicked( SgNode * ) ), ui.kvtMetrics, SLOT( updateView( SgNode * ) ) );

    editorWrapper = new QCodeEdit(ui.roseCodeEdit,this);


    editorWrapper
        ->addPanel("Line Mark Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F6"));

    editorWrapper
        ->addPanel("Line Number Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F11"));

    editorWrapper
        ->addPanel("Fold Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F9"));

    editorWrapper
        ->addPanel("Line Change Panel", QCodeEdit::West, true);

    editorWrapper
        ->addPanel("Status Panel", QCodeEdit::South, true);

    editorWrapper
        ->addPanel("Search Replace Panel", QCodeEdit::South);

}

MainWindow::~MainWindow()
{
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
    //ui.bAstView->setFilter( &filter );
}
