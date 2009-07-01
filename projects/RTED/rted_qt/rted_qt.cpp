#include "rted_qt.h"
#include "RtedDebug.h"

#include <cstdarg>

#include <QString>
#include <QFileInfo>

#include <QDebug>

/// generates filename of transformed file ( from name.c to name_rose.c)
QString buildSecondFileName(const char * filename1)
{
	QString strFilename(filename1);
    QFileInfo fi (strFilename );
    QString res = fi.absolutePath();
    res += "/" + fi.baseName();
    res += "_rose.";
    res += fi.completeSuffix();
    qDebug() << "Transformed filename from " << filename1 <<"to" << res;
    return res;
}


void gui_updateDataStructures(struct RuntimeVariablesType * stack,int stackSize,
		                      struct RuntimeVariablesType * vars, int varSize,
							  struct MemoryType * mem,            int memSize)
{
	//update gui direcly, assuming that gui thread is not running
    RtedDebug * d = RtedDebug::instance();
    d->stackSize = stackSize;
    d->stack=stack;

    d->varSize = varSize;
    d->vars = vars;

    d->memSize = memSize;
    d->mem = mem;
}

void gui_checkpoint(const char* filename, int lineFile1, int lineFile2)
{
    RtedDebug * d = RtedDebug::instance();
    d->file1 = QString(filename);
    d->file2 = buildSecondFileName(filename);

    d->file1Line = lineFile1;
    d->file2Line = lineFile2;
    d->startGui();

}

void gui_showDialog(const char* filename, int lineFile1, int lineFile2)
{
    RtedDebug * d = RtedDebug::instance();
    d->file1 = QString(filename);
    d->file2 = buildSecondFileName(filename);

    d->file1Line = lineFile1;
    d->file2Line = lineFile2;
    d->startGui();
}


/// ----------------------- Output --------------------------------------------



QString printfToQString(const char * format, va_list ap )
{
    static const size_t BUFFER_SIZE=200;
    static char buffer[BUFFER_SIZE];

    vsnprintf(buffer,BUFFER_SIZE,format,ap);
    QString res(buffer);
    if(res.endsWith('\n'))
    	res.remove(res.size()-1,1);

    return res;
}


void gui_printMessage(const char * format, va_list ap )
{
    RtedDebug * d = RtedDebug::instance();
    d->messages.push_back(qMakePair(RtedDebug::MESSAGE,printfToQString(format,ap)));
}

void gui_printWarning(const char * format, va_list ap)
{
    RtedDebug * d = RtedDebug::instance();
    d->messages.push_back(qMakePair(RtedDebug::WARNING,printfToQString(format,ap)));
}

void gui_printError(const char * format, va_list ap)
{
    RtedDebug * d = RtedDebug::instance();
    d->messages.push_back(qMakePair(RtedDebug::ERROR,printfToQString(format,ap)));
}


