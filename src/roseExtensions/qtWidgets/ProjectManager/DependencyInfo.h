#ifndef DEPENDENCYINFO_H
#define DEPENDENCYINFO_H


#include <QList>
#include <QObject>


class DependencyInfo : public QObject
{
    Q_OBJECT

    public:
        enum State
        {
            VALID,   ///< node is up to date
            INVALID, ///< note is invalidated, waiting for dependecies to update
            ERROR    ///< while building the node an error occured
        };

        /// Adds a node this node depends on
        void addDependency(DependencyInfo * dep);

        /// Updates the current state of this node
        /// when build successful to VALID, and to ERROR otherwise
        virtual void buildFinished(bool successful);



        virtual State getState() { return state; }

        static void checkGraphForRebuilds(const QList<DependencyInfo*> & graph);

    public slots:
        /// This node an all nodes which depend on it, are set to invalid
        /// usually called if object changes,
        //// this function schedules the object for rebuild, don't call rebuild yourself!
        virtual void invalidate();

    protected:
        /// Starts a rebuild of this node
        virtual void rebuild() =0 ;

        /// incoming edges in dependency graph
        /// i.e. nodes this node depends on
        QList<DependencyInfo *> dependsOn;

        /// outgoing edges in dependency graph
        /// i.e. list of nodes which depend on this node
        QList<DependencyInfo *> dependendNodes;

        State state;
};


#endif
