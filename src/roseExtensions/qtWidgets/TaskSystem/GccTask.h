
#ifndef GCCTASK_H_
#define GCCTASK_H_



#include <QProcess>
#include <QString>
#include <QStringList>

#include "TaskInterface.h"

class GccOutputParser;

class CompilerOutputWidget;
class QVBoxLayout;


class TaskList;
class GccOutputParser : public TaskOutputInfo
{
    Q_OBJECT

    public:
        GccOutputParser(QObject * par = 0);


        /// Shows a table with errors/warnings
        /// by using this widget as parent for a custom ListWidget
        virtual void showInWidget  (QWidget * w, TaskList *l);

        /// Hides the window, such that another output can be displayed
        virtual void hide(QWidget * w);



    public slots:
        /// Called if new data are available on IO-device
        virtual void readData(QIODevice * dev);

    signals:
        virtual void itemSelected(const QString & file, int line, int column);


    protected:

        struct ParsedLine
        {
            ParsedLine() : line(0), column(0) {}

            bool isValid() const { return message.count(); }

            int line, column;
            QString file, message;
        };

        /**
         * Parses a line from g++ output and returns information about the error msg and location
         * function was taken out of edyuk, see http://edyuk.org
         *
         * @param outputLine the output from g++
         * @param line  result of parsing (struct with error msg, line etc), call ParserLine::isValid before use
         *              some lines from g++ do not contain errors
         * @return list of modified files
         * */
        QStringList parseLine(const QString& outputLine, ParsedLine& line);

        void addLineToWidget(const ParsedLine & line);

        CompilerOutputWidget * outWidget;
};



class GccCompileTask : public Task
{
    Q_OBJECT

    public:
        GccCompileTask(const QString & sourcefile, const QString & outFile);
        GccCompileTask(const QStringList & sourcefiles,const QString & outFile);


        virtual void start();

        virtual Task::State getState() const;

        virtual TaskOutputInfo * getOutputInfo()  { return outParser; }

        virtual QString getDescription() const;

        static QString     gcc_path;
        static QStringList default_args;

    protected slots:
        void dataAvailable();

    protected:
        void init(const QString & outfile);

        QProcess process;
        QStringList arguments;

        bool started;

        GccOutputParser * outParser;
};





#endif
