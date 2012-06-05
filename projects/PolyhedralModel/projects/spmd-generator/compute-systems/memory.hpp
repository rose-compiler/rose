
#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <vector>

class Memory {
  public:
    Memory();
    Memory(const Memory & arg);
    virtual ~Memory();

    virtual Memory * copy() const = 0;
};

class BasicMemory : public Memory {
  protected:
    unsigned long size;
    unsigned blocking;
    unsigned distance;

  public:
    BasicMemory(unsigned long size_, unsigned blocking_, unsigned distance_);
    BasicMemory(const BasicMemory & arg);
    virtual ~BasicMemory();
 
    virtual BasicMemory * copy() const;
};

class MemoryHierarchy : public Memory {
  protected:
    std::vector<Memory *> levels;

  public:
    MemoryHierarchy();
    MemoryHierarchy(const MemoryHierarchy & arg);
    virtual ~MemoryHierarchy();

    void add(Memory * mem);

    unsigned size() const;
    Memory * get(unsigned i) const;

    virtual MemoryHierarchy * copy() const;
};

class Cache : public BasicMemory {
  public:
    enum CachePolicy {
      direct, LRU, LFU, Rand, pLRU, pLFU, pRand
    };

  protected:
    unsigned way;
    CachePolicy policy;

  public:
    Cache(unsigned long size_, unsigned block_, unsigned distance_, unsigned way_ = 1, CachePolicy policy_ = direct);
    Cache(const Cache & arg);
    virtual ~Cache();

    virtual Cache * copy() const;
};

#endif /* __MEMORY_HPP__ */

