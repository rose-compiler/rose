#ifndef reverse_rctypes_h
#define reverse_rctypes_h

#ifdef RC_USING_C_ONLY

#include <stdlib.h>
#include <memory.h>

typedef struct FlagStack
{
    int* data;
    int index, offset;
} FlagStack;

int popFlag(FlagStack* fs)
{
    int ret = fs->data[fs->index] & fs->offset;
    fs->offset >>= 1;
    if (fs->offset == 0)
    {
	fs->offset = 1 << (sizeof(int) - 1);
	--fs->index;
    }
    return ret;
}

int pushFlag(FlagStack* fs, int val)
{
    fs->offset <<= 1;
    if (fs->offset == 0)
    {
	fs->offset = 1;
	++fs->index;
    }	
    if (val)
	fs->data[fs->index] |= fs->offset;
    return val;
}

void flagTop(FlagStack* fs)
{
    fs->data[fs->index] |= fs->offset;
}
 
FlagStack* buildFlagStack()
{
    const int SIZE = 128;
    int size = sizeof(FlagStack) + SIZE * sizeof(int);
    FlagStack* fs = (FlagStack*)malloc(size);
    memset(fs, size, 0);
    fs->data = (int*)(fs + 1);
    return fs;
}

void destroyFlagStack(FlagStack* fs)
{
    free(fs);
}

#else

#include <vector>

class FlagStack
{
    std::vector<int> flags_;
    std::vector<int> marks_;

public:
    int pop()
    {
	int flag = flags_.back();
	flags_.pop_back();
	return flag;
    }

    /*  
    int push(int flag)
    {
	flags_.push_back(flag);
	return flag;
    }
    */

    int push(int flag, int mark = -1, bool ins = false)
    {
	if (mark >= 0)
	{
	    if (marks_.size() < mark)
		marks_.resize(mark * 2);
	    if (!ins)
	    {
		flags_.push_back(flag);
		marks_[mark] = flags_.size() - 1;
	    }
	    else
	    {
		int parent = mark - 1;
		flags_.insert(flags_.begin() + marks_[parent], flag);
		updateMarks(marks_[parent]);
		marks_[mark] = marks_[parent];
	    }
	}
	else
	    flags_.push_back(flag);
	return flag;
    }

    void updateMarks(int mark)
    {
	for (int i = 0; i < marks_.size(); ++i)
	    if (marks_[i] >= mark)
		++marks_[i];
    }
};

inline int popFlag(FlagStack* fs)
{
    return fs->pop();
}

inline int pushFlag(FlagStack* fs, int val, int mark = -1, bool ins = false)
{
    return fs->push(val, mark, ins);
}

FlagStack* buildFlagStack()
{
    return new FlagStack;
}

#endif

#endif
