#ifndef FIXUP_TYPEDEF_SEQUENCE_LISTS_H
#define FIXUP_TYPEDEF_SEQUENCE_LISTS_H

/*! \brief This traversal uses the Memory Pool traversal to fixup the lists of typedef using this base type.
 */
class NormalizeTypedefSequenceLists : public ROSE_VisitTraversal
   {
  // Build a traversal for use on just the SgTypedefSeq memory pool so that we can 
  // normalize all the lists for each kind of type derived type for each semantically 
  // equivalent type.
     public:
          class key_t
             {
               public:
                    std::vector<size_t> IRnodeList;
                    std::string declarationString;

                 // Required functions to use STL interface.
                    bool empty() const;
                    size_t hash(const std::string & s) const;
                    size_t size() const;
                    size_t operator[](size_t i) const;
                    bool operator<(const key_t & x) const;
             };

          typedef std::map<key_t, std::vector<SgType*> > typedefSeqMap_t;
          typedefSeqMap_t typedefSeqMap;

          static key_t generateKey(SgTypedefSeq* typedefSeq);

      //! Required traversal function
          void visit (SgNode* node);
   };

class NormalizeTypedefSequenceLists_CopyList : public ROSE_VisitTraversal
   {
  // Build a traversal for use on just the SgTypedefSeq memory pool so that we can 
  // normalize all the lists for each kind of type derived type for each semantically 
  // equivalent type.
     public:
          NormalizeTypedefSequenceLists::typedefSeqMap_t & typedefSeqMap;
          NormalizeTypedefSequenceLists_CopyList (NormalizeTypedefSequenceLists::typedefSeqMap_t & l);

      //! Required traversal function
          void visit (SgNode* node);
   };

void normalizeTypedefSequenceLists();

// endif for FIXUP_TYPEDEF_SEQUENCE_LISTS_H
#endif
