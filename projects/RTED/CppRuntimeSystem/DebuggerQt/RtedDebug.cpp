#include "RtedDebug.h"

#include <QDebug>

#include <pthread.h>


RtedDebug * RtedDebug::single = NULL;

RtedDebug * RtedDebug::instance()
{
    if(!single)
       single = new RtedDebug();

    return single;
}


RtedDebug::RtedDebug()
{
    showAlways=false;

    //here we are in rtsi thread
    rtsi.id = pthread_self();
    rtsi.running=false;
    gui.running=true;


    // creates a new thread for gui, which starts at guiMain
    pthread_create( & gui.id, NULL, &RtedDebug::guiMain, this );
    enterRtsi();
}


// this is the entry-point of gui-thread
void * RtedDebug::guiMain(void * rtedVoid)
{
    RtedDebug * r = static_cast<RtedDebug*>(rtedVoid);

    r->enterGui();
    r->gui.id=pthread_self();


    //qDebug() << "In Gui Thread - starting" << pthread_self();

    r->app = new QApplication(0); /* tps changed, removed second param NULL */
    r->dlg = new DbgMainWindow(r);

    // Initialization finished, go back to rtsi thread
    r->leaveGui();

    r->enterGui();
    r->updateDialogData();
    r->dlg->showMaximized();
    r->app->exec();
    qDebug() << "Exiting because Debugger was closed";
    exit(0);
}


void RtedDebug::startRtsi()
{
    leaveGui();
    enterGui();
    cout << "StartRtsi." << endl;
    updateDialogData();
}


void RtedDebug::startGui(bool sa)
{
    showAlways = sa;
    leaveRtsi();
    enterRtsi();
}

void RtedDebug::updateDialogData()
{
    Q_ASSERT(gui.running);

    for(int i=0; i<messages.size(); i++)
    {
        if(messages[i].first == ERROR)
            dlg->addErrorMessage(messages[i].second);
        else
            dlg->addMessage(messages[i].second);
    }

    dlg->updateAllRsData(showAlways);
    showAlways=false;

    messages.clear();
}


void RtedDebug::addMessage(std::string &  msg, MessageType type)
{
  QString message = QString(msg.c_str());
  addMessage(message,type);
}

void RtedDebug::addMessage(std::string &  msg)
{
  QString message = QString(msg.c_str());
  addMessage(message,RtedDebug::MESSAGE);
}

void RtedDebug::addMessage(const QString & msg, MessageType type)
{
    QString msgCopy=msg;
    while(msgCopy.endsWith('\n'))
        msgCopy.remove(msgCopy.size()-1,1);

    messages.push_back(qMakePair(type,msgCopy));
}


// --------------------- Thread Stuff ------------------------


RtedDebug::ThreadData::ThreadData()
{
    id = -1;
    pthread_mutex_init(& mutex ,NULL);
    pthread_cond_init(& signal,NULL);
    running=false;
}

void RtedDebug::enterThread (ThreadData & cur, ThreadData & other)
{
    pthread_mutex_lock ( & cur.mutex );

    // the signal from other thread signals "finished"
    while(other.running)
        pthread_cond_wait(& cur.signal, & cur.mutex);


    pthread_mutex_lock( & other.mutex);
    cur.running=true;
    other.running=false;
    pthread_mutex_unlock(& other.mutex);
}

void RtedDebug::leaveThread (ThreadData & cur, ThreadData & other )
{
    pthread_mutex_lock(& other.mutex);
    cur.running=false;
    other.running=true;
    pthread_cond_signal(& other.signal);
    pthread_mutex_unlock(& other.mutex);

    pthread_mutex_unlock( & cur.mutex);
}


void RtedDebug::printDbg(const QString & s)
{
    return;

    if(gui.id < 0 || rtsi.id < 0)
    {
        qDebug() << "Not yet initialized" << gui.id << rtsi.id ;
        return;
    }

    Q_ASSERT(! (gui.running && rtsi.running) );

    if(gui.running)
    {
        Q_ASSERT(pthread_self() == gui.id);
        qDebug() << "GUI running" << s;
        return;
    }

    if(rtsi.running)
    {
        Q_ASSERT(pthread_self() == rtsi.id);
        qDebug() << "RTSI running" << s;
        return;
    }

    qDebug() << "Nothing running" << s;
}


