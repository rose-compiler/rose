
#ifndef RTEDDEBUG_H
#define RTEDDEBUG_H


#include <QApplication>
#include <QObject>


#include "DebugDialog.h"
#include "RuntimeSystem.h"


#include <pthread.h>

#include <QDebug>

class RtedDebug
{
    struct ThreadData
    {
        ThreadData();

        pthread_t id;
        pthread_mutex_t mutex;
        pthread_cond_t  signal;
        bool running;
    };

    public:
        static RtedDebug * instance();

        //Variables for exchanging data between to threads

        enum MessageType
        {
        	MESSAGE, WARNING,ERROR
        };

        QList<QPair<MessageType,QString> > messages;



        /// File which is displayed in first codeEditor
        QString file1;
        /// line which is highlighted in first codeEditor
        int file1Line;

        /// File which is displayed in second codeEditor
        QString file2;
        /// line which is highlighted in second codeEditor
        int file2Line;


        RuntimeVariablesType * stack;
        int stackSize;

        RuntimeVariablesType * vars;
        int varSize;

        MemoryType * mem;
        int memSize;

        /// Starts debug dialog
        /// runtimeSystem is blocked until singleStep or resume was clicked
        /// this function is called only from runtimeSystem thread
        void startGui();

        void on_singleStep();
        void on_resume();

    protected:
        static void * guiMain(void* rtedQt);

        void printDbg(const QString & s);

        void updateDialogData();

        static RtedDebug * single;
        RtedDebug();

        QApplication * app;
        DebugDialog * dlg;


        ThreadData gui;
        ThreadData rtsi;

        void enterGui()  { enterThread(gui,rtsi); printDbg("After Entering Gui"); }
        void leaveGui()  { printDbg("Before Leaving Gui");   leaveThread(gui,rtsi); }
        void enterRtsi() { enterThread(rtsi,gui);  printDbg("After Entering RTSI"); }
        void leaveRtsi() { printDbg("Before Leaving RTSI");  leaveThread(rtsi,gui); }

        void enterThread (ThreadData & cur, ThreadData & other);
        void leaveThread (ThreadData & cur, ThreadData & other );
};


#endif
