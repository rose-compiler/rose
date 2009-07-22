#include "GccTask.h"

#include "CompilerOutputWidget.h"

#include <QRegExp>
#include <QMap>
#include <QDir>

#include <QDebug>

QString     GccCompileTask::gcc_path    =  QString("g++") ;
QStringList GccCompileTask::default_args=  QStringList() << "-Wall";


GccCompileTask::GccCompileTask(const QString & sourcefile, const QString & outFile)
    : started(false)
{
    arguments << sourcefile;
    init(outFile);
}

GccCompileTask::GccCompileTask(const QStringList & sourcefiles,const QString & outfile)
    : started(false)
{
    foreach(const QString & s, sourcefiles)
        arguments << s;

    init(outfile);
}


void GccCompileTask::init(const QString & outfile)
{
    foreach(const QString & s, default_args)
        arguments << s;

    arguments << "-o" << outfile;

    process.setProcessChannelMode(QProcess::MergedChannels);
    outParser = new GccOutputParser(this);

}

Task::State GccCompileTask::getState() const
{
   QProcess::ProcessState procState = process.state();

   if(procState==QProcess::Running || procState==QProcess::Starting)
       return RUNNING;

   if(!started)
       return BEFORE_START;

   if(process.exitCode() ==0)
       return FINISHED_SUCCESS;
   else
       return FINISHED_ERROR;
}


void GccCompileTask::start()
{
    Q_ASSERT(!started);

    started=true;
    process.start(gcc_path,arguments);

    connect(&process,SIGNAL(readyRead()), SLOT(dataAvailable()) );
}

void GccCompileTask::dataAvailable()
{
    outParser->readData(&process);
}


QString GccCompileTask::getDescription() const
{
    QString ret = tr("Compiling");
    foreach(const QString & s, arguments)
    {
        ret.append(" ");
        ret.append(s);
    }

    return ret;
}


// ---------------------------------- Output Parser ------------------------------

#include "CompilerOutputWidget.h"

#include <QTextStream>
#include <QVBoxLayout>

#include "TaskList.h"

GccOutputParser::GccOutputParser(QObject * par)
    : TaskOutputInfo(par)
{
    outWidget = new CompilerOutputWidget();
}


void GccOutputParser::showInWidget (QWidget * w, TaskList * taskList)
{
    QVBoxLayout * l = new QVBoxLayout(w);
    outWidget->setParent(w);
    l->addWidget(outWidget);

    connect(outWidget, SIGNAL(taskClicked(const QString&,int)),
            taskList,  SIGNAL(clicked(const QString&, int))    );

    outWidget->show();
}

void GccOutputParser::hide( QWidget * w)
{
    outWidget->hide();
    outWidget->setParent(0);

    disconnect(outWidget, SIGNAL(taskClicked(const QString&,int)));


    //to delete layout
    qDeleteAll(w->children());
}

void GccOutputParser::readData(QIODevice * dev)
{
    QTextStream stream(dev);

    QString line;

    ParsedLine parsedLine;
    while(1)
    {
        line=stream.readLine();
        if(line.isNull())
            return;

        parseLine(line, parsedLine);
        addLineToWidget(parsedLine);
    }
}

void GccOutputParser::addLineToWidget(const ParsedLine & parsed)
{
    if(!parsed.isValid())
        return;

    QRegExp error("\\s*error: (.*)");
    QRegExp warning("\\s*warning: (.*)");

    bool isError   = error.exactMatch(parsed.message);
    bool isWarning = warning.exactMatch(parsed.message);

    QString msg = parsed.message;
    if(isError)   msg = error.cap(1);
    if(isWarning) msg = warning.cap(1);

    CompilerOutputWidget::PatternType t;
    if(isError)         t = CompilerOutputWidget::Error;
    else if(isWarning ) t = CompilerOutputWidget::Warning;
    else                t = CompilerOutputWidget::Unknown;

    outWidget->addItem(t,msg,parsed.file,parsed.line);
}





// this function is taken from Edyuk
QStringList GccOutputParser::parseLine(const QString& outputLine, ParsedLine& line)
{
       QStringList modifiedFiles;

       line.line = 0;
       line.column = 0;
       line.file.clear();
       line.message.clear();

       static QString path;
       static QRegExp
           special("\\[([^]:]+)(?::([^]:]+))*\\]"),
           enterleave("((?:\\w:)?(?:[/\\\\][^/\\\\]+)+[/\\\\]?).$");

       QMap<QString, QRegExp> pattern;

       pattern[QObject::tr("compiling %1[%1]")]
           = QRegExp("^g(?:cc|\\+\\+) -c .+ -o .+\\.o (.+\\.(?:c|cxx|cc|cpp))");

       pattern[QObject::tr("linking %1[]")]
           = QRegExp("^g(?:cc|\\+\\+) .+ -o (\\S+)(?: .+\\.o)+");

       pattern[QObject::tr("moc'ing %1[%1]")]
           = QRegExp("^(?:(?:\\w:)?/(?:[^/]+/)*)?moc (?:-\\S+ )*(\\S+)");

       pattern[QObject::tr("processing form %1[%1]")]
           = QRegExp("^(?:(?:\\w:)?/(?:[^/]+/)*)?uic (\\S+)( -\\w+(?: (?:\\S+)?))*");

       pattern[QObject::tr("processing resource %1[%1]")]
           = QRegExp("^(?:(?:\\w:)?/(?:[^/]+/)*)?rcc (?:-name \\S+ )(\\S+)");

       pattern["$enterleave"]
           = QRegExp(QString("^%1\\[\\d+\\]:").arg("make"));

       pattern["%3[%1:%2]"]
           = QRegExp("^([^\n:\\[\\]]+):(\\d+):(.+)");

       QMap<QString, QRegExp>::iterator it = pattern.begin();

       QString data = outputLine.trimmed();

       while ( it != pattern.end() )
       {
           if ( data.contains(*it) )
           {
               line.message = it.key();
               QStringList caps = it->capturedTexts();

               if ( (line.message == "$enterleave") && data.contains(enterleave) )
               {
                   QString oldpath = path;
                   path = enterleave.cap(1).trimmed();

                   //qDebug("path : %s", qPrintable(path));

                   if ( QDir(path).exists() && (path != oldpath) )
                   {
                       line.message = QObject::tr("Moving to %1").arg(path);
                   } else {
                       line.message.clear();
                       path = oldpath;
                   }
               } else if ( line.message.startsWith('$') ) {
                   line.message.clear();
               } else {
                   for ( int i = 1; i < caps.count(); ++i )
                   {
                       line.message.replace(QString::number(i).prepend("%"), caps.at(i));

                       if ( caps.at(i).startsWith(" -o ") )
                       {
                           QString out = caps.at(i).mid(4);

                           if ( out.count() && path.count() && !QFileInfo(out).isAbsolute() )
                               out = QDir(path).absoluteFilePath(out);

                           if ( out.count() )
                               modifiedFiles << out;
                       }
                   }

                   if ( line.message.contains(special) )
                   {
                       caps = special.capturedTexts();

                       line.file = caps.at(1);

                       // beware gcc line numbers are starting from 1 not 0 as QCodeEdit expects
                       if ( caps.count() > 2 )
                           line.line = caps.at(2).toInt() - 1;
                       else
                           line.line = -1;

                       if ( caps.count() > 3 )
                           line.column = caps.at(3).toInt();
                       else
                           line.column = -1;

                       line.message.remove(caps.at(0));
                   }

                   if ( line.file.count() && path.count() && !QFileInfo(line.file).isAbsolute() )
                       line.file = QDir(path).absoluteFilePath(line.file);

                   modifiedFiles << line.file;

                   //qDebug("{%s,%s}=>%s", qPrintable(line.message), qPrintable(path), qPrintable(line.file));
               }

               break;
           }

           ++it;
       }

       return modifiedFiles;
}

