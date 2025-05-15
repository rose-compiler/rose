#include "sage3basic.h"

#include <CallGraph.h>


#include "Project.h"
#include "ItemModelHelper.h"
#include "RoseFrontendTask.h"

#include "TaskList.h"

#include "SgNodeUtil.h"
#include "MetricsConfig.h"

#include "CmdLineEditDialog.h"

#include <QFileInfo>
#include <QIcon>
#include <QDebug>

#include <QMenu>
#include <QFileDialog>

#include <QSettings>

#include "GccTask.h"
#include "BAstNode.h"


// ---------------------- ProjectManager ---------------------------------

ProjectManager * ProjectManager::single = NULL;


ProjectManager * ProjectManager::instance()
{
    if(!single)
       single = new ProjectManager();

    return single;
}

ProjectManager::ProjectManager()
    : rootNode( new ProjectManagerRootNode() ),
      taskListWdg(0),
      taskOutputWdg(0)
{
}

ProjectManager::~ProjectManager()
{
    storeProjectState();
    delete rootNode;
}

ProjectManagerRootNode *ProjectManager::getModelRootNode()
{
    return rootNode;
}

int ProjectManager::addProject(const QString & projectName)
{
    ProjectNode * proj = new ProjectNode(projectName);
    return rootNode->addChild(proj);
}

ProjectNode * ProjectManager::getProject(int id)
{
    if( !rootNode ) return NULL;

    return dynamic_cast<ProjectNode*> (rootNode->child(id));
}

MetricsConfig * ProjectManager::getMetricsConfig(int id)
{
    if( rootNode == NULL ) return NULL;

    if( rootNode->childrenCount() == 0 ) return NULL;

    ProjectNode *projNode( dynamic_cast<ProjectNode*>( rootNode->child( id ) ) );


    if( projNode == NULL ) return NULL;

    return projNode->getMetricsConfig();
}

MetricsConfig *ProjectManager::getMetricsConfig( SgNode *node )
{
    if( node == NULL ) return NULL;
    if( rootNode == NULL ) return NULL;

    // finding the project root ...
    SgNode *proj = node;
    while( proj->get_parent() )
    {
        proj = proj->get_parent();
    }
    assert( isSgProject( proj ) );

    for( int i = 0; i < getProjectCount(); ++i )
    {
        if( proj == getSgProject( i ) )
        {
            return getProject( i )->getMetricsConfig();
        }
    }

    return NULL;
}


void ProjectManager::storeProjectState()
{
    QSettings s;

    s.remove("LoadedProjects");


    s.beginGroup("LoadedProjects");

    s.beginWriteArray("Projects");

    for(int p=0; p<rootNode->childrenCount(); p++)
    {
        ProjectNode * projNode =  dynamic_cast<ProjectNode*>(rootNode->child(p));

        s.setArrayIndex(p);
        s.setValue("Name",projNode->getName());

        SgProject * sgProj =  projNode->getSgProject();

        s.beginWriteArray("files");
        for(int i=0; i < sgProj->numberOfFiles(); i++)
        {
            QString name = (*sgProj)[i]->getFileName().c_str();
            s.setArrayIndex(i);
            s.setValue("name",name);
        }
        s.endArray();
    }

    s.endArray(); //Projects


    s.endGroup();
}

void ProjectManager::loadProjectState( int argc, char **argv )
{
    QSettings s;

    s.beginGroup("LoadedProjects");

    int projCount = s.beginReadArray("Projects");
    for(int p =0; p < projCount; p++)
    {
        s.setArrayIndex(p);

        QString projName = s.value("Name").toString();
        ProjectNode * projNode = new ProjectNode(projName);
        rootNode->addChild(projNode);

        QStringList l;
        for( int i( 0 ); i < argc; ++i )
            l << argv[i];

        projNode->setCommandLine( l );

        int size = s.beginReadArray("files");
        for(int i=0; i < size; i++)
        {
            s.setArrayIndex(i);
            QString file( s.value("name").toString() );
            projNode->addFile( file );
        }
        s.endArray();
    }
    s.endArray();

    s.endGroup();
}


SgProject * ProjectManager::getSgProject(int id)
{
    return getProject(id)->getSgProject();
}

int ProjectManager::getProjectCount() const
{
    Q_ASSERT(rootNode);
    return rootNode->childrenCount();
}


int ProjectManager::getIdFromSgProject(SgProject * sgProj)
{
    for(int i=0; i < getProjectCount(); i++)
    {
        if(getProject(i)->getSgProject() == sgProj)
            return i;
    }
    qDebug() << "Warning: ProjectManager::getIdFromProject";
    qDebug() << "Tried to get a projectId with an unknown sgProject";
    return -1;
}

// ----------------------- ProjectNode -----------------------------------


ProjectNode::ProjectNode(const QString & n)
    : name(n),
      sgProject( new SgProject() ),
      metricsConfig( new MetricsConfig( "", NULL, sgProject ) ),
      callGraph(NULL),
      srcFileHeaderNode(NULL),
      binFileHeaderNode(NULL)
{
    // Create an empty SgProject

    //sgProject = new SgProject();
    sgProject->get_fileList().clear();

    // TODO why is that needed? what does "cc" do?
    // just starting rose with filename argument seems to work too
    // ->what is the commandline of an SgProject created with frontend()
    Rose_STL_Container<std::string> commandLine;
    commandLine.push_back("cc");
    commandLine.push_back("-c");
    sgProject->set_originalCommandLineArgumentList (commandLine);
}

/*
ProjectNode::ProjectNode(const QString & n, SgProject * initProj)
    : name(n), sgProject(initProj)
{
    Q_ASSERT(initProj);

    typedef Rose_STL_Container<SgNode*>::iterator SgFileIter;
    Rose_STL_Container<SgNode *> stmts = NodeQuery::querySubTree(initProj,V_SgFile);

    for(SgFileIter i= stmts.begin(); i != stmts.end(); ++i)
    {
        SgFile * f = isSgFile(*i);
        Q_ASSERT(f);
        addFileIntern(f);
    }
}*/

ProjectNode::~ProjectNode()
{
    delete sgProject;
    delete metricsConfig;
}


bool ProjectNode::addFile(const QString & file)
{
    QFileInfo fileInfo(file);
    if(! fileInfo.exists())
        return false;

    qDebug() << "adding file ..." << file;

    if(CommandlineProcessing::isSourceFilename(fileInfo.absoluteFilePath().toStdString()), false/*AdaMode*/)
    {
        createSrcFileHeaderNode();

        SourceFileNode * newFileNode = new SourceFileNode(file);
        srcFileHeaderNode->addChild(newFileNode);
        newFileNode->rebuild();
    }
    else if(CommandlineProcessing::isExecutableFilename(fileInfo.absoluteFilePath().toStdString()))
    {
        createBinFileHeaderNode();
        BinaryFileNode * binFileNode = new BinaryFileNode(file);
        binFileHeaderNode->addChild(binFileNode);
        binFileNode->rebuild();
    }
    else
    {
        qWarning() << "ProjectNode::addFile - unrecognized file type";
        return false;
    }
    metricsConfig->setRoot( sgProject );

    qDebug() << "done";

    return true;
}

void ProjectNode::createSrcFileHeaderNode()
{
    if(srcFileHeaderNode)
        return;

    srcFileHeaderNode = new SourceFileHeaderNode();
    //SourceFileNode should be first child:add at beginning
    addChildAt(srcFileHeaderNode,0);
}

void ProjectNode::createBinFileHeaderNode()
{
    if(binFileHeaderNode)
        return;
    binFileHeaderNode = new BinaryFileHeaderNode();
    // Should be at second position
    int id = qMin(1,childrenCount());
    addChildAt(binFileHeaderNode,id);
}


void ProjectNode::addBinaryFile(const QList<SourceFileNode*> & srcFiles)
{
    //TODO
}


QStringList ProjectNode::getCommandLine() const
{
    QStringList res;

    Rose_STL_Container<std::string> arglist;
    arglist = sgProject->get_originalCommandLineArgumentList();

    Rose_STL_Container<std::string>::iterator iter;
    for(iter = arglist.begin(); iter != arglist.end(); ++iter )
        res << iter->c_str();


    return res;
}

void ProjectNode::addToCommandLine(const QString & s)
{
    std::vector<std::string> cmd( sgProject->get_originalCommandLineArgumentList() );

    cmd.push_back(s.toStdString() );
    sgProject->processCommandLine( cmd );
}


void ProjectNode::setCommandLine(const QStringList & sl)
{
    std::vector<std::string> cmd;

    foreach(const QString curStr, sl)
        cmd.push_back( curStr.toStdString() );

    sgProject->processCommandLine( cmd );
}


void ProjectNode::showCmdLineDialog()
{
    CmdLineEditDialog dlg;
    dlg.setCurrentCmdLine(getCommandLine());

    if( dlg.exec() == QDialog::Accepted)
    {
        setCommandLine(dlg.getResult());
    }
}

int ProjectNode::getSourceFileCount() const
{
    if( !srcFileHeaderNode )
        return 0;

    return srcFileHeaderNode->childrenCount();
}

SourceFileNode *ProjectNode::getSourceFile( int id ) const
{
    return dynamic_cast<SourceFileNode *>( srcFileHeaderNode->child( id ) );
}

int ProjectNode::getBinaryFileCount() const
{
    if( !binFileHeaderNode )
        return 0;

    return binFileHeaderNode->childrenCount();
}

BinaryFileNode *ProjectNode::getBinaryFile( int id ) const
{
    return dynamic_cast<BinaryFileNode *>( binFileHeaderNode->child( id ) );
}

QVariant ProjectNode::data(int role, int column) const
{
    if(column>0)
        return QVariant();

    if      (role == Qt::DisplayRole)      return name;
    else if (role == Qt::DecorationRole )  return QIcon(":/util/NodeIcons/project.gif");
    else if (role == SgNodeRole)           return QVariant::fromValue((SgNode*)sgProject);

    return QVariant();
}


SgIncidenceDirectedGraph * ProjectNode::getCallGraph()
{
    if(callGraph==NULL)
    {
      // tps (09/10/2009) The old call did not compile for me
      //  CallGraphBuilder cgb (sgProject, false );
        CallGraphBuilder cgb (sgProject );
        cgb.buildCallGraph();

        callGraph = cgb.getGraph();
    }

    return callGraph;
}





// ----------------------- SourceFileHeaderNode -----------------------------------

QVariant SourceFileHeaderNode::data(int role, int column) const
{
    Q_ASSERT(column>=0);

    if(column>0)
        return QVariant();

    if      (role == Qt::DisplayRole)      return QObject::tr("Source Files");
    else if (role == Qt::DecorationRole )  return QIcon(":/util/NodeIcons/sourcefiles.gif");

    return QVariant();
}

// -------------------------- SourceFileNode --------------------------------------

#include "RoseFrontendTask.h"

SourceFileNode::SourceFileNode(const QString & _path)
    : path(_path), sgSourceFile(NULL), task(NULL)
{
    QFileInfo fileInfo(path);
    filename=fileInfo.fileName();
    path=fileInfo.absoluteFilePath();

    Q_ASSERT( CommandlineProcessing::isSourceFilename(filename.toStdString(), false/*AdaMode*/) );
}

SourceFileNode::~SourceFileNode()
{
    SgProject * proj = dynamic_cast<ProjectNode*>(getParent()->getParent() )->getSgProject();
    deleteFileFromProject(sgSourceFile,proj);
}

QVariant SourceFileNode::data(int role, int column) const
{
    Q_ASSERT(column>=0);

    if(column>0)
        return QVariant();

    if      (role == Qt::DisplayRole)      return filename;
    else if (role == Qt::DecorationRole )  return QIcon(":/util/NodeIcons/sourcefile.gif");
    else if (role == SgNodeRole)           return QVariant::fromValue((SgNode*)sgSourceFile);

    return QVariant();
}

void SourceFileNode::rebuild()
{
    ProjectNode * pn = dynamic_cast<ProjectNode*>(getParent()->getParent() );
    Q_ASSERT(pn);

    deleteFileFromProject(sgSourceFile,pn->getSgProject());
    sgSourceFile = NULL;

    task = new RoseFrontendTask(pn->getSgProject(),path);
    connect(task,SIGNAL(finished()), SLOT(buildTaskFinished()));

    ProjectManager::instance()->taskListWidget()->submitTask(task);
}

void SourceFileNode::buildTaskFinished()
{
    Q_ASSERT(!sgSourceFile);

    if( task->getState() == Task::FINISHED_SUCCESS )
    {
        sgSourceFile = dynamic_cast<SgSourceFile*>(task->getResult());
        Q_ASSERT(sgSourceFile);

        BAstNode::generate(sgSourceFile,NULL,this);
    }


    disconnect(this,SLOT(buildTaskFinished()));

    // task is deleted by TaskList
    task=NULL;
}


// ----------------------- BinaryFileHeaderNode -----------------------------------

QVariant BinaryFileHeaderNode::data(int role, int column) const
{
    Q_ASSERT(column>=0);

    if(column>0)
        return QVariant();

    if      (role == Qt::DisplayRole)      return QObject::tr("Binary Files");
    else if (role == Qt::DecorationRole )  return QIcon(":/util/NodeIcons/binaryfiles.gif");

    return QVariant();
}

BinaryFileNode::~BinaryFileNode()
{
    SgProject * proj = dynamic_cast<ProjectNode*>(getParent()->getParent() )->getSgProject();
    deleteFileFromProject(sgBinaryComposite,proj);
}

// -------------------------- BinaryFileNode--- -----------------------------------



BinaryFileNode::BinaryFileNode(const QString & _path)
    : path(_path),compileTask(NULL),frontendTask(NULL),sgBinaryComposite(NULL)
{
    QFileInfo fileInfo(path);
    path=fileInfo.absoluteFilePath();
    qDebug() << "BinaryFile Node constructor path=" << path;
    filename=fileInfo.fileName();
}

BinaryFileNode::BinaryFileNode(const QList<SourceFileNode*> & sources, const QString & outfileName)
    : path(outfileName), sourceFiles(sources)
{
    QFileInfo fileInfo(path);
    filename=fileInfo.fileName();

    QStringList sourceFiles;
    foreach(const SourceFileNode * n, sources)
        sourceFiles << n->getPath();

    submitCompileTask();
}

void BinaryFileNode::submitCompileTask()
{
    // no source files to compile!
    Q_ASSERT(! sourceFiles.isEmpty());

    QStringList fileNames;
    foreach(const SourceFileNode * n, sourceFiles)
    fileNames << n->getPath();

    Q_ASSERT(compileTask == NULL);

    compileTask = new GccCompileTask(fileNames,filename);

    ProjectManager::instance()->taskListWidget()->submitTask(compileTask);

}


void BinaryFileNode::rebuild()
{
    ProjectNode * pn = dynamic_cast<ProjectNode*>(getParent()->getParent() );
    deleteFileFromProject(sgBinaryComposite,pn->getSgProject());
    sgBinaryComposite=NULL;

    if(sourceFiles.isEmpty())
        submitFrontendTask();
    else
        submitCompileTask();
}

void BinaryFileNode::submitFrontendTask()
{
    ProjectNode * pn = dynamic_cast<ProjectNode*>(getParent()->getParent() );
    Q_ASSERT(pn);

    frontendTask = new RoseFrontendTask( pn->getSgProject(), path );
    connect( frontendTask, SIGNAL( finished() ), SLOT( frontendTaskFinished() ) );

    ProjectManager::instance()->taskListWidget()->submitTask(frontendTask);
}

void BinaryFileNode::frontendTaskFinished()
{
    Q_ASSERT(!sgBinaryComposite);
    if( frontendTask->getState() == Task::FINISHED_SUCCESS )
    {
        sgBinaryComposite = dynamic_cast<SgBinaryComposite*>(frontendTask->getResult());
        Q_ASSERT(sgBinaryComposite);
    }


    disconnect(this,SLOT(frontendTaskFinished()));

    // task is deleted by TaskList
    frontendTask=NULL;

    BAstNode::generate(sgBinaryComposite,NULL,this);
}

void BinaryFileNode::buildTaskFinished()
{
    if(compileTask->getState() == Task::FINISHED_SUCCESS )
        submitFrontendTask();
    else
        qDebug() << "Compilation had errors";

    disconnect(this,SLOT(buildTaskFinished()));

    compileTask=NULL;
}

QVariant BinaryFileNode::data(int role, int column) const
{
    Q_ASSERT(column>=0);

    if(column>0)
        return QVariant();

    if      (role == Qt::DisplayRole)      return filename;
    else if (role == Qt::DecorationRole )  return QIcon(":/util/NodeIcons/binaryfile.gif");
    else if (role == SgNodeRole)           return QVariant::fromValue((SgNode*)sgBinaryComposite);


    return QVariant();
}


