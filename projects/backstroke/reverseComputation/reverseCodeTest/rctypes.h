#ifndef reverse_rctypes_h
#define reverse_rctypes_h

#include <stdlib.h>

#if 0

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
#include <queue>

class IntStack
{
    std::vector<int> values_;
    std::vector<int> marks_;

public:
    int pop()
    {
	int flag = values_.back();
	values_.pop_back();
	return flag;
    }

    /*  
    int push(int flag)
    {
	values_.push_back(flag);
	return flag;
    }
    */

    int push(int flag, int mark = -1)
    {
	if (mark >= 0)
	{
	    if (marks_.size() < (mark + 1))
		marks_.resize((mark + 1) * 2);
	    if (mark == 0)
	    {
		values_.push_back(flag);
		marks_[mark] = values_.size() - 1;
	    }
	    else
	    {
		int parent = mark - 1;
		values_.insert(values_.begin() + marks_[parent], flag);
		updateMarks(marks_[parent]);
		marks_[mark] = marks_[parent];
	    }
	}
	else
	    values_.push_back(flag);
	return flag;
    }

    void updateMarks(int mark)
    {
	for (int i = 0; i < marks_.size(); ++i)
	    if (marks_[i] >= mark)
		++marks_[i];
    }
};

inline int pop(IntStack* fs)
{
    return fs->pop();
}

inline int push(IntStack* fs, int val, int mark = -1)
{
    return fs->push(val, mark);
}

IntStack* buildIntStack()
{
    return new IntStack;
}

/**********************************************************************************
 * The following random number generator functions are to make sure the event and forward event
 * functions received the same random numbers, in order to make test pass.
 **********************************************************************************/
static std::queue<int> random_numbers;

int rand_num()
{
    int num = rand();
    random_numbers.push(num);
    return num;
}

int rand_num_fwd()
{
    int num = random_numbers.front();
    random_numbers.pop();
    return num;
}

#endif

#endif
