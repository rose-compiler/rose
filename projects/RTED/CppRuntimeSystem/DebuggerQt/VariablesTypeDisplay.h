#ifndef VARIABLESTYPEDISPLAY_H
#define VARIABLESTYPEDISPLAY_H

#include "ItemTreeNode.h"

class VariablesType;
class StackManager;

class VariablesTypeDisplay : public ItemTreeNode
{
    public:
        VariablesTypeDisplay(VariablesType * mt_, bool displayMem=true);

        virtual QVariant data(int role, int column=0) const;
        virtual QStringList sectionHeader() const;


        static VariablesTypeDisplay * build(StackManager  * sm);

    protected:
        VariablesTypeDisplay() : vt(NULL) {}

        VariablesType  * vt;

};







#endif
