
#include "rose.h"

#include "RoseFrontendTask.h"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <QFileInfo>
#include <QDebug>


RoseFrontendTask::RoseFrontendTask(SgProject * proj,const QString & filename)
    : sgFile(NULL),sgProject(proj),file(filename),state(BEFORE_START)
{
}

void RoseFrontendTask::start()
{
    namespace io = boost::iostreams;

    state = RUNNING; //not really needed,because executed in same thread

    std::stringstream output;
    
    // ------------------------------------------------------------------------
    // begin platform specific, only works on posix systems
    int out_pipe[2];
    int saved_stdout;

    // saving stdout file descriptor
    saved_stdout = dup( STDOUT_FILENO );

    try
    {
        // creating pipe
        if( pipe( out_pipe ) != 0 )
        {
            ROSE_ABORT( "unable to create pipe" );
        }

        // duplicating stdout
        dup2( out_pipe[1], STDOUT_FILENO );
        close( out_pipe[1] );

        // creating file ... (platform independent, hopefully)
        sgFile = SageBuilder::buildFile( file.toStdString(), std::string(), sgProject );

        fflush( stdout );

        typedef io::file_descriptor_source source;
        source src( out_pipe[0] );
    // end platform specific
    // ------------------------------------------------------------------------

        io::stream<source>  input( src );
        input >> std::noskipws;
        
        std::istream_iterator<char> begin(input), end ;
        std::copy( begin, end, std::ostream_iterator<char>(output) ) ;
    }
    catch( rose_exception& e )
    {
        output << e.what();
    }
    catch( ... )
    {
        output << "Unkown exception caught ...";
    }

    // restoring stdout file descriptor
    dup2( saved_stdout, STDOUT_FILENO );

    /*QFileInfo fileInfo(file);
    if(! fileInfo.exists())
    {
        qDebug() << "RoseFrontendTask: File does not exist" << file;
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
    sgProject->set_frontendErrorCode(qMax(sgProject->get_frontendErrorCode(), nextErrorCode));*/

    state = FINISHED_SUCCESS;
    emit finished();
}

QString RoseFrontendTask::getDescription() const
{
    return "Rose Frontend Call for " + file;
}
