

#ifndef ROSEFRONTENDTASK_H
#define ROSEFRONTENDTASK_H

#include "TaskInterface.h"

#include <QString>

class SgProject;
class SgFile;

class RoseFrontendTask : public Task
{
    public:
        RoseFrontendTask(SgProject * proj,const QString & file);

        /// Currently no output information
        /// TODO find elegant way to redirect STDOUT
        virtual TaskOutputInfo * getOutputInfo()  { return 0; }


        virtual void start();

        virtual State getState() const { return state; }

        virtual QString getDescription() const;

        SgFile * getResult() const { return sgFile; }

    protected:
        SgFile    * sgFile;    // resulting sgFile or NULL if error
        SgProject * sgProject; // project to add the sgFile to
        QString     file;      // file which gets compiled

        State state;
};

#endif
