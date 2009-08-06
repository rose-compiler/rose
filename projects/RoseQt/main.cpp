#include "rose.h"

#define USE_QROSE 0

#if USE_QROSE
    #include "qrose.h"
#endif


#include <QApplication>
#include <QDebug>
#include <QtUiTools/QUiLoader>


#include <rosehpct/rosehpct.hh>

#include <vector>
#include <string>

#include "MainWindow.h"
#include "FlopCounter.h"
#include "AsmInstructionsBar.h"
#include "InstructionCountAnnotator.h"


// Includes needed only for manual and runtime loading approach
#include "AstBrowserWidget.h"
#include "NodeInfoWidget.h"
#include "RoseCodeEdit.h"
#include "BAstView.h"
#include "MetricsKiviat.h"
#include "RoseFileComboBox.h"


using namespace std;

#if USE_QROSE
using namespace qrs;

// Approach 1: Setup Manually
QWidget * setupGuiManually(SgProject * project)
{
    QRWindow * window = new QRWindow ("MainWindow",QROSE::LeftRight,QROSE::UseSplitter);

    AstBrowserWidget * astBrowser = new AstBrowserWidget();
    astBrowser->setNode(project);
    (*window)["AstBrowser"] << astBrowser;

    BAstView * bAstView = new BAstView();
    bAstView->setNode(project);
    (*window)["Beautified Ast"] << bAstView;


    NodeInfoWidget * nodeInfoWdg = new NodeInfoWidget();
    (*window)["NodeInfoWidget"] << nodeInfoWdg;

    RoseCodeEdit * codeEdit = new RoseCodeEdit();
    (*window)["CodeEdit"] << codeEdit;

    MetricsKiviat * kiviat = new MetricsKiviat();
    kiviat->init(project);
    (*window)["Kiviat"] << kiviat;

    //Connections to NodeInfoWidget
    QObject::connect(astBrowser,SIGNAL(clicked(SgNode*)),nodeInfoWdg,SLOT(setNode(SgNode*)));
    QObject::connect(bAstView,SIGNAL(clicked(SgNode*)),nodeInfoWdg,SLOT(setNode(SgNode*)));

    //Connections to CodeEdit
    QObject::connect(astBrowser,SIGNAL(clicked(SgNode*)),codeEdit,SLOT(setNode(SgNode*)));
    QObject::connect(bAstView,SIGNAL(clicked(SgNode*)),codeEdit,SLOT(setNode(SgNode*)));


    QObject::connect(astBrowser,SIGNAL(doubleClicked(SgNode*)),kiviat,SLOT(addNode(SgNode*)));
    QObject::connect(astBrowser,SIGNAL(clicked(SgNode*)),kiviat,SLOT(updateView(SgNode*)));

    return window;
}

QWidget * setupGuiUiFile(SgProject * project)
{
    QWidget * window;
    std::string file( "MainWindow.ui" );

#if 0
    window = new QRWindow ("MainWindow",QROSE::LeftRight,QROSE::UseSplitter);

    // New feature: use the path of an ui-file to add a widget
    // still a problem with deleting the loaded object (double free inside qrose?)
    (*qrWindow)["customWdg"] << file;

    return window;
#else
    // Usage of the UiLoader without QRose
    QUiLoader loader;
    QFile fileStr(QString::fromStdString(file));
    fileStr.open(QFile::ReadOnly);

    QWidget * loadedWin = loader.load(&fileStr);
    window=loadedWin;
    window->show();
#endif


    RoseFileComboBox * fileSelect = window->findChild<RoseFileComboBox*>("roseFileCmbBox");
    AstBrowserWidget * browserWdg = window->findChild<AstBrowserWidget*>("astBrowserWidget");
    BAstView * bAstView = window->findChild<BAstView*>("bAstView");

    fileSelect->setProject(project);
    browserWdg->setNode(project);
    bAstView->setNode(project);

    return window;
}

#endif //USE_QROSE


QWidget * setupGuiWithCompiledUi( int argc, char **argv )
{
    // everything is done by the MainWindow-Class
    return new MainWindow( argc, argv );
}

int main(int argc, char** argv)
{

    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("LLNL");
    QCoreApplication::setOrganizationDomain("ROSE");
    QCoreApplication::setApplicationName("demo");

    QWidget * main=NULL;


    // Three methods for GUI loading implemented:
    qDebug() << "Setup Gui..";
    //-----------------------------------------
    //main = setupGuiManually(project);
    //main = setupGuiUiFile(project);
    main = setupGuiWithCompiledUi( argc, argv );
    //-----------------------------------------

    main->show();

    app.exec();

    delete main;
    return 0;
}
