#ifndef BITVECTOR_DATA_REPR_H
#define BITVECTOR_DATA_REPR_H

#include <CountRefHandle.h>
#include <FunctionObject.h>
#include <DoublyLinkedList.h>
#include <assert.h>
#include <sstream>

class BitVectorReprImpl {
  unsigned* impl;
  unsigned  num;
  
  void operator = ( const BitVectorReprImpl& that)
  {}
 public:
  BitVectorReprImpl( unsigned size)
    { 
      unsigned intsize = sizeof(unsigned);
      num = (size + intsize-1) / intsize ;
      impl = new unsigned[num];
      for (unsigned i = 0; i < num; ++i) { 
	impl[i] = 0;
      }
    }
  BitVectorReprImpl( const BitVectorReprImpl& that)
    : num(that.num)
    {
      impl = new unsigned[that.num];
      for (unsigned i = 0; i < num; ++i) {
	impl[i] = that.impl[i];
      }
    }
  ~BitVectorReprImpl() 
    { delete [] impl; }
  
  BitVectorReprImpl* Clone() const { return new BitVectorReprImpl(*this); }
  
  void operator |=( const BitVectorReprImpl& that)
  {
    assert(num == that.num);
    for (unsigned i = 0; i < num; ++i) {
      impl[i] |= that.impl[i];
    }
  }

  std::string toString() const
  {
    std::stringstream r;
    r <<  ":";
    for (unsigned i = 0; i < num; ++i) {
       r << impl[i];
    }
    r << ":"; 
    return r.str();
  }
  void operator &=( const BitVectorReprImpl& that)
  {
    assert(num == that.num);
    for (unsigned i = 0; i < num; ++i) {
      impl[i] &= that.impl[i];
    }
  }
  
  void complement() 
    {
      for (unsigned i = 0; i < num; ++i) {
	impl[i] = ~impl[i];
      }
    }
  bool operator ==( const BitVectorReprImpl& that) const
  {
    assert(num == that.num);
    for (unsigned i = 0; i < num; ++i) {
      if (impl[i] != that.impl[i])
	return false;
    }
    return true;
  }
  
  bool has_member( unsigned index)  const
    {
      unsigned intsize = sizeof(unsigned);
      int i1 = index / intsize;
      int i2 = index % intsize;
      unsigned mask = 1 << i2;
      return impl[i1] & mask;
    }
  void add_member( unsigned index)  
    {
      unsigned intsize = sizeof(unsigned);
      int i1 = index / intsize;
      int i2 = index % intsize;
      unsigned mask = 1 << i2;
      impl[i1] |= mask;
    }
  void delete_member( unsigned index)
    {
      unsigned intsize = sizeof(unsigned);
      int i1 = index / intsize;
      int i2 = index % intsize;
      unsigned mask = ~(1 << i2);
      impl[i1] &= mask;
    }
};

class BitVectorRepr : public CountRefHandle<BitVectorReprImpl>
{
 public:
  BitVectorRepr( BitVectorReprImpl* _impl)
    : CountRefHandle <BitVectorReprImpl>(_impl) {}
  BitVectorRepr( const BitVectorReprImpl& _impl)
    : CountRefHandle <BitVectorReprImpl>(_impl) {}
  
  BitVectorRepr() {}
  BitVectorRepr (unsigned size) 
    : CountRefHandle <BitVectorReprImpl>( new BitVectorReprImpl(size) ) {}
  BitVectorRepr( const BitVectorRepr& that)
    : CountRefHandle <BitVectorReprImpl>(that) {}
  BitVectorRepr& operator = (const BitVectorRepr& that)
  { CountRefHandle <BitVectorReprImpl>:: operator = (that); return *this; }
  
  bool operator ==( const BitVectorRepr& that)
  { return (ConstPtr() == that.ConstPtr()) || 
      (ConstPtr() && that.ConstPtr() && ConstRef() == that.ConstRef()); }
  
  bool IsNIL() const { return ConstPtr()==0; }
  bool operator !=( const BitVectorRepr& that)
  { return !operator==(that); }
  bool has_member( unsigned index)  const
    { return ConstPtr() != 0 && ConstRef().has_member(index); }
  void add_member( unsigned index)  
    { UpdateRef().add_member(index); }
  void delete_member( unsigned index)  
    { UpdateRef().delete_member(index); }
  void operator |= ( const BitVectorRepr& that)
  { UpdateRef() |= that.ConstRef(); }
  void operator &=( const BitVectorRepr& that)
  { UpdateRef() &= that.ConstRef(); }
  void complement() 
    { UpdateRef().complement(); }
  std::string toString() const
    { return ConstRef().toString();  }
};

template <class Name, class Data>
class BitVectorReprBase 
{
  struct DataEntry {
    Data data;
    Name name;
    int index;
    DataEntry( const Name& n, const Data& d, int i = 0) 
      : data(d), name(n), index(i) {}
  };
  typedef DoublyLinkedListWrap<DataEntry> DataList;
  typedef std::map<Name, DoublyLinkedEntryWrap<DataEntry>* > DataMap;
  
  DataList datalist;
  DataMap datamap; 
  
 public:
  typedef typename DoublyLinkedListWrap<DataEntry>::iterator iterator;
  iterator begin() const { return datalist.begin(); }
  iterator end() const { return datalist.end(); }

  BitVectorReprBase( const BitVectorReprBase<Name,Data>& that)
    {
      std::string name = "";
      for ( iterator p = that.datalist.begin(); !p.ReachEnd(); ++p) {
	DataEntry& cur = *p;
        DoublyLinkedEntryWrap<DataEntry>* e = datalist.AppendLast(cur);
        if (name != cur.name) {
           name = cur.name;
           datamap[name] = e;
        } 
      }
    }
  BitVectorReprBase() {} 
  unsigned size() const { return datalist.size(); }
  void add_data( const Name& n, const Data& d)
    {
      typename DataMap::iterator mp = datamap.find(n);
      if (mp == datamap.end()) {
	DoublyLinkedEntryWrap<DataEntry>* cur = datalist.AppendLast(DataEntry(n,d));
	datamap[n] = cur;
      }
      else {
	DoublyLinkedEntryWrap<DataEntry>* lp = (*mp).second;
	assert( lp->GetEntry().name == n);
	datalist.InsertAfter( DataEntry(n,d), lp); 
      }
    }
  void finalize() 
    {
      unsigned index = 0;
      for ( iterator p = begin(); !p.ReachEnd(); ++p, ++index) {
	DataEntry& cur = *p;
	cur.index = index;
      }
    }
  iterator find( const std::string& name) const
    {
      typename DataMap::const_iterator mp = datamap.find(name);
      if (mp == datamap.end()) {
	return end();
      }
      
      iterator lp( datalist, (*mp).second);  
      return lp;
    }
  std::string get_name ( iterator p) const
    { return (*p).name; }
  Data get_data ( iterator p) const
    { return (*p).data; }
  int get_index( iterator p) const
    { return (*p).index; }
};

template <class Name, class Data>
class BitVectorReprGenerator
{
  BitVectorReprBase<Name, Data> base;
 public:
  BitVectorReprGenerator( const BitVectorReprBase<Name,Data>& b)
    : base(b) {}
  BitVectorRepr get_empty_set() const
    {  return BitVectorRepr(base.size()); }
  BitVectorRepr get_data_set( const Name& name) const
    {
      BitVectorRepr result(base.size());
      for (typename BitVectorReprBase<Name,Data>::iterator p 
	     = base.find(name);
	   p != base.end() && base.get_name(p) == name; ++p) {
	int i = base.get_index( p );
	result.add_member(i);
      }
      return result;
    }
  
  void add_member( BitVectorRepr& repr, const Name& name, const Data& d) const
    {
      typename BitVectorReprBase<Name,Data>::iterator p = base.find(name);
      if (p == base.end()) {
           std::cerr << "Error: cannot find variable " << name << "\n";
           assert(false);
      }
      for ( ;  p != base.end() && base.get_data(p) != d; ++p); 
      if (p == base.end()) {
           std::cerr << "Error: cannot find data " << " for variable " << name << "\n";
           assert(false);
      }
      repr.add_member( base.get_index(p));
    }
      
  void delete_member( BitVectorRepr& repr, const Name& name, 
		      const Data& d) const
    {
      typename BitVectorReprBase<Name,Data>::iterator p = base.find(name);
      for ( ; p != base.end() && base.get_data(p) != d; ++p) ;
      assert( p != base.end());
      repr.delete_member(base.get_index(p));
    }
  
  void collect_member( const BitVectorRepr& repr, 
		       CollectObject<Data>& collect) const
    {
      for (typename BitVectorReprBase<Name,Data>::iterator lp = base.begin();  
	   lp != base.end(); ++lp) {
	if (repr.has_member( base.get_index(lp)))
	  collect( base.get_data(lp) );
      }
    }
  const BitVectorReprBase<Name,Data>& get_base() const { return base; }
};

#endif
