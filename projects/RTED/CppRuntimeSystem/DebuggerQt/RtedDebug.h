
#ifndef RTEDDEBUG_H
#define RTEDDEBUG_H


#include <QApplication>
#include <QObject>


#include "MainWindow.h"
#include "../CppRuntimeSystem.h"


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


        void addMessage(const QString& msg, MessageType type);
        void addMessage(const std::string& msg, MessageType type);
        void addMessage(const std::string& msg);

        /// Starts debug dialog
        /// runtimeSystem is blocked until singleStep or resume was clicked
        /// this function is called only from runtimeSystem thread
        void startGui(bool showAlways=false);

        void startRtsi();

    protected:
        static void * guiMain(void* rtedQt);

        void printDbg(const QString & s);

        void updateDialogData();

        static RtedDebug * single;
        RtedDebug();

        QApplication * app;
        DbgMainWindow * dlg;


        ThreadData gui;
        ThreadData rtsi;

        bool showAlways;

        void enterGui()  { enterThread(gui,rtsi); printDbg("After Entering Gui"); }
        void leaveGui()  { printDbg("Before Leaving Gui");   leaveThread(gui,rtsi); }
        void enterRtsi() { enterThread(rtsi,gui);  printDbg("After Entering RTSI"); }
        void leaveRtsi() { printDbg("Before Leaving RTSI");  leaveThread(rtsi,gui); }

        void enterThread (ThreadData & cur, ThreadData & other);
        void leaveThread (ThreadData & cur, ThreadData & other );
};


#endif
