
#ifndef ROSEFRONTENDTASK_H
#define ROSEFRONTENDTASK_H

#include "TaskInterface.h"

#include <QString>

class SgProject;
class SgFile;

class CompilerOutputWidget;

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

    signals:
        virtual void itemSelected( const QString &file, int line, int column );

    private:
        CompilerOutputWidget *outputWidget;
};

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
