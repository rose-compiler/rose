
#include "rose.h"

#include "RoseFrontendTask.h"
#include "CompilerOutputWidget.h"

// annotators
#include "FlopCounter.h"
#include "AsmToSourceMapper.h"
#include "InstructionCountAnnotator.h"
// ----

#include <iostream>
#include <sstream>
#include <unistd.h>

#include <QVBoxLayout>
#include <QFileInfo>
#include <QDebug>

// ---------------------- begin annotations ------------------------------

// TODO: proper, general mechanism

void annotateFiles( SgProject *project, SgFile *file )
{
    ///////////////////////////////////////////////////////////////////////////
    // annotators which work for all kind of SgFiles
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    // annotators which only work for SgSourceFile
    ///////////////////////////////////////////////////////////////////////////
    
    SgSourceFile *srcFile( isSgSourceFile( file ) );
    if( srcFile )
    {
        FlopCounterProcessor flops;
        flops.traverse( file );

        for( int i( 0 ); i < project->numberOfFiles(); ++i )
        {
            SgBinaryFile *otherBinFile( isSgBinaryFile( (*project)[i] ) );

            if( otherBinFile == NULL ) continue;

            AsmToSourceMapper mapper( otherBinFile );
            mapper.annotate( srcFile );
        }
    }
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    // annotators which only work for SgBinaryFile
    ///////////////////////////////////////////////////////////////////////////

    SgBinaryFile *binFile( isSgBinaryFile( file ) );
    if( binFile )
    {
        std::string pinFileName( binFile->getFileName() );
        pinFileName += ".pin";
        InstructionCountAnnotator::annotate( binFile, pinFileName );

        AsmToSourceMapper mapper( binFile );
        for( int i( 0 ); i < project->numberOfFiles(); ++i )
        {
            SgSourceFile *otherSrcFile( isSgSourceFile( (*project)[i] ) );

            if( otherSrcFile == NULL ) continue;
            
            mapper.annotate( otherSrcFile );
        }
        
    }
    ///////////////////////////////////////////////////////////////////////////
}

// ---------------------- end annotations --------------------------------

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
        // creating SgFile ... (platform independent, hopefully)
        sgFile = SageBuilder::buildFile( file.toStdString(), std::string(), sgProject );
    }
    catch( ... )
    {
        state = FINISHED_ERROR;
        emit finished();
        return;
    }

    fflush( stdout );
    fflush( stderr );

    /*typedef io::file_descriptor_source source;
    source src( out_pipe[0] );*/

    // restoring stdout
    dup2( saved_stdout, STDOUT_FILENO );
    // restoring stderr
    dup2( saved_stderr, STDERR_FILENO );
    // end platform specific
    // ------------------------------------------------------------------------

    QFile output;
    output.open( out_pipe[0], QIODevice::ReadOnly | QIODevice::Text );

    out.readData( &output );

    annotateFiles( sgProject, sgFile );

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
    connect( outputWidget, SIGNAL( taskClicked( const QString &, int ) ),
             this,         SLOT  ( itemClicked( const QString &, int ) ) );
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

    QRegExp rx( "\"(.+)\", line (\\d+): (warning|error):.*" );

    int i( 0 );
    //while( !stream.atEnd() )
    while( ++i )
    {
        //++i;
        QString line = stream.readLine();

        if( line.isNull() ) return;

        int pos( 0 );
        if( rx.exactMatch( line ) )
        {
            //qDebug() << "found match ...";

            //qDebug() << "file" << rx.cap( 1 );
            //qDebug() << "line" << rx.cap( 2 );
            //qDebug() << "type" << rx.cap( 3 );

            const QString &file( rx.cap( 1 ) );
            const QString &lineNo( rx.cap( 2 ) );
            const QString &type( rx.cap( 3 ) );
            const QString &message( stream.readLine() );

            if( message.isNull() ) return;

            if( type == "warning" )
            {
                outputWidget->addItem( CompilerOutputWidget::Warning,
                                       message, file, lineNo.toInt() );
            }
            if( type == "error" )
            {
                outputWidget->addItem( CompilerOutputWidget::Error,
                                       message, file, lineNo.toInt() );
            }
        }

        //outputWidget->addItem( CompilerOutputWidget::Unknown,
        //line, "...", i );
    }

}
