#include "rose.h"

#include "RoseFrontendTask.h"

#include <iostream>

#include <QFileInfo>
#include <QDebug>


RoseFrontendTask::RoseFrontendTask(SgProject * proj,const QString & filename)
    : sgFile(NULL),sgProject(proj),file(filename),state(BEFORE_START)
{
}

void RoseFrontendTask::start()
{
    state = RUNNING; //not really needed,because executed in same thread

    QFileInfo fileInfo(file);
    if(! fileInfo.exists())
    {
        state= FINISHED_ERROR;
        emit finished();
        return;
    }

    // Adapted from src/frontend/SageIII/sageBuilder.C  function buildFile
    // with the change that it should also handle binary files
    sgProject->get_sourceFileNameList().push_back(file.toStdString());

    Rose_STL_Container<std::string> arglist;
    arglist = sgProject->get_originalCommandLineArgumentList();
    arglist.push_back(file.toStdString());

    std::copy( arglist.begin(), arglist.end(),
               std::ostream_iterator<std::string, char>( std::cout, ", " ) );
    std::cout << std::endl;

    int nextErrorCode = 0;
    sgFile = isSgFile(determineFileType(arglist, nextErrorCode, sgProject));
    if(!sgFile)
    {
        qWarning() << "ProjectNode::addFile - failed to create SgFile";
        state= FINISHED_ERROR;
        emit finished();
        return;
    }


    sgFile->set_parent(sgProject);

    sgProject->get_fileList().push_back(sgFile);
    sgProject->set_frontendErrorCode(qMax(sgProject->get_frontendErrorCode(), nextErrorCode));

    state = FINISHED_SUCCESS;
    emit finished();
}

QString RoseFrontendTask::getDescription() const
{
    return "Rose Frontend Call for " + file;
}
