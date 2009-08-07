
#ifndef ROSEFRONTENDTASK_H
#define ROSEFRONTENDTASK_H

#include "TaskInterface.h"

#include <QString>

class SgProject;
class SgFile;

class CompilerOutputWidget;

/**
 * \brief Widget for displaying the warnings/errors of the ROSE frontend call
 */
class RoseFrontendOutput
    : public TaskOutputInfo
{
    Q_OBJECT

    public:
        RoseFrontendOutput( QWidget *par = 0 );
        virtual ~RoseFrontendOutput();

        virtual void showInWidget( QWidget *w, TaskList *l );
        virtual void hide( QWidget *w );

    public slots:
        virtual void readData( QIODevice *io );
        void itemClicked( const QString &file, int line )
        { emit itemSelected( file, line, 0 ); }

    signals:
        virtual void itemSelected( const QString &file, int line, int column );

    private:
        CompilerOutputWidget *outputWidget;
};

/**
 * \brief Task for calling the ROSE frontend
 * 
 * Currently the ROSE frontend is called in the same process. This has certain disadvantages:
 *  - \c stdout and  \c stderr have to be redirected to get the output of the EDG frontend, these redirection can only be done in an architecture dependend way (call to dup() )
 *  - the EDG frontend calls abort() on some compilation errors (for example when a include file is not found - so the whole gui terminates
 *  
 *  Better solution: 
 * - call another program, which parses the file and creates the AST, then serialize the AST and write it to a file ( this approach is currently not possible because AST of binary files cannot be serialized yet)
 * - another advantage of this solution would be, that on GUI startup only a serialized AST would have to be read - which is much faster
 * 
 */
class RoseFrontendTask
    : public Task
{
    public:
        RoseFrontendTask(SgProject * proj,const QString & file);

        /// Currently no output information
        /// TODO find elegant way to redirect STDOUT
        virtual TaskOutputInfo * getOutputInfo()  { return &out; }


        virtual void start();

        virtual State getState() const { return state; }

        virtual QString getDescription() const;

        SgFile * getResult() const { return sgFile; }

    protected:
        SgFile    * sgFile;    // resulting sgFile or NULL if error
        SgProject * sgProject; // project to add the sgFile to
        QString     file;      // file which gets compiled
        RoseFrontendOutput out;

        State state;
};

#endif
