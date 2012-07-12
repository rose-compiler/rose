
#ifndef POINTERDISPLAY_H
#define POINTERDISPLAY_H


#include "ItemTreeNode.h"

class PointerManager;
class PointerInfo;


class PointerDisplay : public ItemTreeNode
{
    public:
        PointerDisplay(const PointerInfo * pi_);

        virtual QVariant    data(int role, int column=0) const;
        virtual QStringList sectionHeader() const;


        static PointerDisplay* build(PointerManager * mm);

    protected:
        PointerDisplay() : pi(NULL) {}

        const PointerInfo* pi;

};



#endif
