#ifndef BinQT_LCS_H
#define BinQT_LCS_H

#include <boost/smart_ptr.hpp>
#include <vector>
#include <string>
#include <map>
namespace LCS
{
    template<typename T>
    class vector_start_at_one
    {
        public:
            vector_start_at_one() :  sa(data){}
            vector_start_at_one(const std::vector<T> & init) : sa(init) {}

            size_t size() const                 {  return sa.size(); }
            //T* get()      const                 {  return sa.get();  }
            //void push_back(T v)                 {  sa.push_back(v);  }
            //T& operator[](size_t i)       {  return sa[i - 1]; }

            const T& operator[](size_t i) const { return  sa[i-1];  }

        private:
            vector_start_at_one(const vector_start_at_one<T>&); // Not copyable

            std::vector<T> data;
            const std::vector<T> & sa;
    };

    bool isEqual(SgNode* A, SgNode* B);

    void LCSLength(boost::scoped_array<boost::scoped_array<size_t> >& C,
                   vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B);

    void printDiff(vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B,
                   std::vector<int>& addInstr, std::vector<int>& minusInst);


    void getDiff( const std::vector<SgNode*>& A,const std::vector<SgNode*>& B,
                  std::vector<std::pair<SgNode*,SgNode*> > & result);



    std::string unparseInstrFast(SgAsmInstruction* iA);

};

#endif
