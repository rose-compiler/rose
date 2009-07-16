#ifndef PROJECT_H
#define PROJECT_H

#include "ItemTreeNode.h"
#include "QDebug"

class SgNode;
class SgProject;
class SgSourceFile;
class SgBinaryFile;


class ProjectNode;
class ProjectManagerRootNode;

class MetricsConfig;

class TaskList;
class QWidget;

class ProjectManager : public ItemTreeNode
{
    public:
        virtual ~ProjectManager();

        static ProjectManager * instance();

        int addProject(const QString & projectName);

        ProjectNode * getProject(int id);
        int getProjectCount() const;
        SgProject * getSgProject(int id);
        MetricsConfig * getMetricsConfig(int id);

        MetricsConfig *getMetricsConfig( SgNode *node );

        void setTaskListWidget     (TaskList * l)   { taskListWdg = l; }
        void setTaskMsgOutputWidget(QWidget * wdg)  { taskOutputWdg = wdg; }

        TaskList * taskListWidget()   { Q_ASSERT(taskListWdg);   return taskListWdg; }
        QWidget  * taskOutputWidget() { Q_ASSERT(taskOutputWdg); return taskOutputWdg; }

        ProjectManagerRootNode * getModelRootNode();

        int getIdFromSgProject(SgProject * proj);

        void storeProjectState();
        void loadProjectState();

    private:
        //singleton
        static ProjectManager * single;
        ProjectManager();

        ProjectManagerRootNode * rootNode;

        TaskList * taskListWdg;
        QWidget * taskOutputWdg;
};


class ProjectManagerRootNode : public ItemTreeNode
{
    public:
        ProjectManagerRootNode()
            : ItemTreeNode()
        {}

        virtual QStringList sectionHeader() const
        {
            static QStringList l = QStringList() << "Element";
            return l;
        }
};

class SgFile;
class SourceFileNode;
class BinaryFileNode;
class SourceFileHeaderNode;
class BinaryFileHeaderNode;
class SgIncidenceDirectedGraph;

class ProjectNode : public ItemTreeNode
{
    public:
        /// Creates a ProjectNode with an empty SgProject
        ProjectNode(const QString & name);
        virtual ~ProjectNode();

        /// Creates a ProjectNode with an existing SgProject
        /// the project is freed by this class
        //ProjectNode(const QString & name, SgProject * initProj);

        /// Adds a source or binary file to the project
        /// @return false if failure (file doesn't exist / unknown filetype)
        bool addFile(const QString & path);

        /// Adds a binary file by compiling an existing source-file
        void addBinaryFile(const QList<SourceFileNode *> & sourceFileNode);


        /** Expensive operation because conversion
         * from std-container to QStringList,
         *  use getSgProject()->get_originalCommandLineArgumentList() if possible  */
        QStringList getCommandLine() const;
        void addToCommandLine(const QString & string);
        void setCommandLine(const QStringList & l);


        SgProject * getSgProject()   { return sgProject; }
        MetricsConfig * getMetricsConfig() { return metricsConfig; }


        int getSourceFileCount() const;
        SourceFileNode * getSourceFile(int id) const;

        int getBinaryFileCount() const;
        BinaryFileNode * getBinaryFile(int id) const;

        //  ----  Implementation of ItemTreeNode Interface  ---------
        virtual QVariant data(int role, int column=0) const;


        const QString & getName() const { return name;}

        /// returns the callgraph associated with this Project
        /// TODO update graph when files are added
        SgIncidenceDirectedGraph * getCallGraph();

    protected:
        QString name;
        SgProject * sgProject;
        MetricsConfig *metricsConfig;

        SgIncidenceDirectedGraph * callGraph;

        /// The *fileHeaderNode are created when needed
        /// if you want to add something call these functions before
        /// they check if the node exists, if not create them
        inline void createSrcFileHeaderNode();
        inline void createBinFileHeaderNode();

        SourceFileHeaderNode * srcFileHeaderNode;
        BinaryFileHeaderNode * binFileHeaderNode;
};

class SourceFileHeaderNode : public ItemTreeNode
{
    public:
        virtual QVariant data(int role, int column=0) const;
};


class ProjectNode;
class RoseFrontendTask;

class SourceFileNode : public QObject, public ItemTreeNode
{
    Q_OBJECT
    friend class ProjectNode;

    public:
        virtual ~SourceFileNode();


        const QString & getPath() const          { return path;}
        const QString & getFileName() const      { return filename; }
        SgSourceFile * getSgSourceFile() const   { return sgSourceFile; }

        virtual QVariant data(int role, int column=0) const;

        virtual void rebuild();


    protected slots:
        void buildTaskFinished();

    protected:

        // only the projectNode can create new SourceFileNodes
        SourceFileNode(const QString & path);


        QString path;
        QString filename;
        SgSourceFile * sgSourceFile;

        RoseFrontendTask * task;

        /// Binaries which were build using this file are added here
        QList<BinaryFileNode*> binaries;
};




class BinaryFileHeaderNode :   public ItemTreeNode
{
    public:
        virtual QVariant data(int role, int column=0) const;

};

class ProjectNode;
class GccCompileTask;

class BinaryFileNode : public QObject, public ItemTreeNode
{
    Q_OBJECT

    friend class ProjectNode;
    public:
        virtual ~BinaryFileNode();

        virtual QVariant data(int role, int column=0) const;

        SgBinaryFile *getSgBinaryFile() const { return sgBinaryFile; }

        virtual void rebuild();

    protected slots:
        void frontendTaskFinished();
        void buildTaskFinished();

    protected:
        void submitCompileTask();
        void submitFrontendTask();

        // only the projectNode can create new SourceFileNodes
        // add an existing binary file
        BinaryFileNode(const QString & path);

        BinaryFileNode(const QList<SourceFileNode*> & sources, const QString & outputPath);

        QString path;
        QString filename;

        QList<SourceFileNode*> sourceFiles;

        GccCompileTask *   compileTask;
        RoseFrontendTask * frontendTask;

        /// Node in the Sage-AST
        SgBinaryFile * sgBinaryFile;
};


#endif
