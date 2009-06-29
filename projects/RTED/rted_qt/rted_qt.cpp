#include "rted_qt.h"
#include "RtedDebug.h"

#include <cstdarg>

#include <QString>
#include <QFileInfo>



void showDebugDialog(struct RuntimeVariablesType * stack, int stackSize,
                     struct RuntimeVariablesType * heap, int heapSize,
                     struct MemoryType * mem, int memSize,
                     char* filename, int lineFileA,int lineFileB )
{
    RtedDebug * d = RtedDebug::instance();
    d->stack = stack;
    d->stackSize=stackSize;

    d->vars= heap;
    d->varSize = heapSize;

    d->mem = mem;
    d->memSize = memSize;

    d->file1= QString(filename);
    d->file1Line = lineFileA;

    // build second filename, (filename 1 with prefix rose_
    QFileInfo fi (d->file1);
    QString filename2 = fi.absolutePath();
    filename2 += "rose_";
    filename2 += fi.fileName();

    d->file2= filename2;
    d->file2Line = lineFileB;
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
