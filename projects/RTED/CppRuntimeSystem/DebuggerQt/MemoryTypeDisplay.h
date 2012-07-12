
#ifndef MEMORYTYPEDISPLAY_H
#define MEMORYTYPEDISPLAY_H

#include "ItemTreeNode.h"

class MemoryType;
class MemoryManager;

class MemoryTypeDisplay : public ItemTreeNode
{
    public:
        MemoryTypeDisplay(const MemoryType& mt_, bool displayPointer=true);

        virtual QVariant data(int role, int column=0) const;
        virtual QStringList sectionHeader() const;

        static MemoryTypeDisplay* build(MemoryManager * mm, bool showHeap=true, bool showStack=true);

    protected:
        MemoryTypeDisplay() : mt(NULL) {}

        const MemoryType * mt;
};




#endif
