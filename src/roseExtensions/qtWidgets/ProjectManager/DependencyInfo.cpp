#include "DependencyInfo.h"
#include "TaskList.h"

#include <QDebug>




void DependencyInfo::addDependency(DependencyInfo * dep)
{
    dependsOn.append(dep);
    dep->dependendNodes.append(this);
}


void DependencyInfo::buildFinished(bool successful)
{
    // a rebuild should only be scheduled if this node is invalid
    // if valid, no rebuild is needed,
    // if error a previous rebuild has failed (when user changes the input it's set to INVALID)
    Q_ASSERT(state == INVALID);

    state = successful ? VALID : ERROR;
}

void DependencyInfo::invalidate()
{
    state = INVALID;

    // all nodes which depend on this node are now invalid too
    foreach(DependencyInfo * n, dependendNodes)
        n->invalidate();
}


void  DependencyInfo::checkGraphForRebuilds(const QList<DependencyInfo*> & graph)
{
    foreach( DependencyInfo * curDepInfo, graph )
    {
        if(curDepInfo->state != INVALID) //only invalid should be rebuild
            continue;

        bool canBeRebuild=true;
        //Try rebuild
        foreach(DependencyInfo * dep, curDepInfo->dependsOn )
        {
           if(dep->state != VALID )
           {
               canBeRebuild=false;
               break;
           }
        }

        if(canBeRebuild)
            curDepInfo->rebuild();
    }

}

