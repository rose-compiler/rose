
#include "rose.h"

#include "RoseFrontendTask.h"
#include "CompilerOutputWidget.h"

#include <iostream>
#include <sstream>
#include <unistd.h>

#include <QVBoxLayout>
#include <QFileInfo>
#include <QDebug>


RoseFrontendTask::RoseFrontendTask(SgProject * proj,const QString & filename)
    : sgFile(NULL),sgProject(proj),file(filename),state(BEFORE_START)
{
}

void RoseFrontendTask::start()
{
    state = RUNNING; //not really needed,because executed in same thread

    // ------------------------------------------------------------------------
    // begin platform specific, only works on posix systems
    int out_pipe[2];
    int saved_stdout;
    int saved_stderr;

    // saving stdout file descriptor
    saved_stdout = dup( STDOUT_FILENO );
    saved_stderr = dup( STDERR_FILENO );

    try
    {
        // creating pipe
        if( pipe( out_pipe ) != 0 )
        {
            ROSE_ABORT( "unable to create pipe" );
        }
    }
    catch( ... )
    {
        state = FINISHED_ERROR;
        emit finished();
    }

    // redirecting stdout to pipe
    dup2( out_pipe[1], STDOUT_FILENO );
    close( out_pipe[1] );
    // redirecting stderr to stdout
    dup2( STDOUT_FILENO, STDERR_FILENO );

    try
    {
        // creating file ... (platform independent, hopefully)
        sgFile = SageBuilder::buildFile( file.toStdString(), std::string(), sgProject );
    }
    catch( ... )
    {
        state = FINISHED_ERROR;
        emit finished();
    }

    fflush( stdout );

    /*typedef io::file_descriptor_source source;
    source src( out_pipe[0] );*/

    QFile output;
    output.open( out_pipe[0], QIODevice::ReadOnly );

    // restoring stdout
    dup2( saved_stdout, STDOUT_FILENO );
    // restoring stderr
    dup2( saved_stderr, STDERR_FILENO );
    // end platform specific
    // ------------------------------------------------------------------------


    out.readData( &output );

    state = FINISHED_SUCCESS;
    emit finished();
}

QString RoseFrontendTask::getDescription() const
{
    return "Rose Frontend Call for " + file;
}

RoseFrontendOutput::RoseFrontendOutput( QWidget *par )
    : TaskOutputInfo( par ),
      outputWidget( new CompilerOutputWidget )
{
}

RoseFrontendOutput::~RoseFrontendOutput()
{}

void RoseFrontendOutput::showInWidget( QWidget *w, TaskList *l )
{
    QVBoxLayout *lay( new QVBoxLayout( w ) );

    outputWidget->setParent( w );
    lay->addWidget( outputWidget );

    outputWidget->show();
}

void RoseFrontendOutput::hide( QWidget *w )
{
    outputWidget->hide();
    outputWidget->setParent(0);
    
    qDeleteAll(w->children());
}

void RoseFrontendOutput::readData( QIODevice *io )
{
    QTextStream stream(io);

    QString line;

    int i( 0 );
    while( ++i )
    {
        line=stream.readLine();
        if(line.isNull())
            return;
    
        outputWidget->addItem( CompilerOutputWidget::Unknown,
                               line, "...", i );
    }

}
