#ifndef reverse_rctypes_h
#define reverse_rctypes_h

#include <stdlib.h>
#include <memory.h>

typedef struct flag_stack
{
    int* data;
    int index, offset;
} FlagStack;

int Pop(FlagStack* fs)
{
    /* 
    static offset = 0;
    int ret = (*data) & 1;
    (*data) >>= 1;
    ++offset;
    if (offset == sizeof(int))
	--data;
    return ret;
    */

    int ret = fs->data[fs->index] & fs->offset;
    fs->offset >>= 1;
    if (fs->offset == 0)
    {
	fs->offset = 1 << (sizeof(int) - 1);
	--fs->index;
    }
    return ret;
}

void Push(FlagStack* fs, int val)
{
    fs->offset <<= 1;
    if (fs->offset == 0)
    {
	fs->offset = 1;
	++fs->index;
    }	
    if (val)
	fs->data[fs->index] |= fs->offset;
}

void FlagTop(FlagStack* fs)
{
    fs->data[fs->index] |= fs->offset;
}
 
FlagStack* BuildFlagStack()
{
    const int SIZE = 128;
    int size = sizeof(FlagStack) + SIZE * sizeof(int);
    FlagStack* fs = (FlagStack*)malloc(size);
    memset(fs, size, 0);
    fs->data = (int*)(fs + 1);
    return fs;
}

void DestroyFlagStack(FlagStack* fs)
{
    free(fs);
}

#endif
