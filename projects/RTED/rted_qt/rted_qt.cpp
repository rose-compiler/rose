#include "rted_qt.h"
#include "RtedDebug.h"

#include <cstdarg>

#include <QString>

/*
void showDebugDialog(struct RuntimeVariablesType * stack, int stackSize,
                     struct RuntimeVariablesType * heap, int heapSize,
                     struct MemoryType * mem, int memSize,
                     char* filename, int row)
{
    static QApplication * app = new QApplication (0,NULL);
    static DebugDialog * dlg = new DebugDialog();

    QCoreApplication::setOrganizationName("LLNL");
    QCoreApplication::setOrganizationDomain("ROSE");
    QCoreApplication::setApplicationName("rted_qt");


    dlg->setHeapVars(heap,heapSize);
    dlg->setStackVars(stack,stackSize);
    dlg->setMemoryLocations(mem,memSize);

    dlg->setEditorMark(filename,row);

    dlg->showMaximized();
    app->exec();
}*/

void showDebugDialog(struct RuntimeVariablesType * stack, int stackSize,
                     struct RuntimeVariablesType * heap, int heapSize,
                     struct MemoryType * mem, int memSize,
                     char* filename, int row)
{
    RtedDebug * d = RtedDebug::instance();
    d->stack = stack;
    d->stackSize=stackSize;

    d->vars= heap;
    d->varSize = heapSize;

    d->mem = mem;
    d->memSize = memSize;

    d->file1= QString(filename);
    d->file1Line = row;
    d->startGui();
}



void printGui(const char * msg)
{
    RtedDebug * d = RtedDebug::instance();
    QString strMsg ( msg);
    QStringList list = strMsg.split('\n',QString::SkipEmptyParts);
    foreach(const QString & s, list )
        d->messages.push_back(s);
}


void printfGui(const char * format, ...)
{
    va_list ap;
    va_start(ap,format);
    vprintfGui(format,ap);
}

void vprintfGui(const char * format, va_list ap)
{
    static const size_t BUFFER_SIZE=100;
    static char buffer[BUFFER_SIZE];

    vsnprintf(buffer,BUFFER_SIZE,format,ap);
    printGui(buffer);

}
