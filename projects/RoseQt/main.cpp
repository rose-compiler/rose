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


SgFile * addFileToSgProject(const QString & file, SgProject * proj)
{
    // Adapted from src/frontend/SageIII/sageBuilder.C  function buildFile
    // with the change that it should also handle binary files

    proj->get_sourceFileNameList().push_back(file.toStdString());

    Rose_STL_Container<std::string> arglist;
    arglist = proj->get_originalCommandLineArgumentList();
    arglist.push_back(file.toStdString());


    int nextErrorCode = 0;
    SgFile* result = isSgFile(determineFileType(arglist, nextErrorCode, proj));

    result->set_parent(proj);

    proj->set_file(*result);

    proj->set_frontendErrorCode(max(proj->get_frontendErrorCode(), nextErrorCode));

    return result;
}


SgProject * buildOwnProject()
{
    SgProject * proj = new SgProject();
    proj->get_fileList().clear();

    //Set command line:
    //proj->set_originalCommandLineArgumentList();

    Rose_STL_Container<std::string> arglist;

    arglist.push_back("cc");
    arglist.push_back("-c");
    proj->set_originalCommandLineArgumentList (arglist);

    addFileToSgProject("inputTest.cpp",proj);
    addFileToSgProject("inputTest",proj);


    return proj;
}


int main(int argc, char** argv)
{
    //vector<string> argvList(argv, argv + argc);
    //qDebug() << "Loading HPCToolkit or Gprof profiling data..." << endl;
    //RoseHPCT::ProgramTreeList_t profiles =
    //RoseHPCT::loadProfilingFiles(argvList);

    //qDebug() << "Done";

    //qDebug() << "Calling ROSE frontend...";
    //SgProject * project = frontend(argvList);
    //SgProject * project = 0;

    //qDebug() << "Done";

    //qDebug() << "Attaching HPCToolkit metrics to Sage IR tree..." << endl;
    //RoseHPCT::attachMetrics(profiles, project, true);
    //qDebug() << "Done";

    //qDebug() << "Annotate AST with Flop Counts ...";
    //FlopCounterProcessor flops;
    //flops.traverse(project);
    //qDebug() << "Done";

    /*qDebug() << "Annotate AST with ASM-Instruction Exec-Counts ...";
    InstructionCountAnnotator::annotate(project,vector<string> ());
    qDebug() << "Done";*/


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
