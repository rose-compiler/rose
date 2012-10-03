template<class IndexType,class HashFun>
class Maintainer : public HSet<IndexType,HashFun> {
public:
  typedef pair<bool,const IndexType*> ProcessingResult;
  bool exists(IndexType& s) { 
	typename Maintainer<IndexType,HashFun>::iterator i;
	i=Maintainer<IndexType,HashFun>::find(s);
	return i!=Maintainer<IndexType,HashFun>::end();
  }
  ProcessingResult process(IndexType newIndexType) {
	std::pair<typename Maintainer::iterator, bool> res;
	res=insert(newIndexType);
	return make_pair(res.second,&(*res.first));
  }
  const IndexType* processNew(IndexType& s) {
	ProcessingResult res=process(s);
	assert(res.first==false);
	return res.second;
  }
  const IndexType* processNewOrExisting(IndexType& s) {
	ProcessingResult res=process(s);
	return res.second;
  }
  long numberOf() { return Maintainer<IndexType,HashFun>::size(); }
  long memorySize() const { return 0; 
	// TODO: compute memory
  }
 private:
  const IndexType* ptr(IndexType& s) {}
};
