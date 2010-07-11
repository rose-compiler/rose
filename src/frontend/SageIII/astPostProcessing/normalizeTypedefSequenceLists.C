#include "sage3basic.h"
#include "normalizeTypedefSequenceLists.h"

using namespace std;

bool
NormalizeTypedefSequenceLists::key_t::empty() const
   {
     return false;
   }

size_t
NormalizeTypedefSequenceLists::key_t::size() const
   {
     return IRnodeList.size() + declarationString.empty() ? 0 : 1;
   }

size_t
NormalizeTypedefSequenceLists::key_t::operator[](size_t i) const
   {
     if (i < IRnodeList.size())
          return IRnodeList[i];
       else
          return hash(declarationString);
   }

bool
NormalizeTypedefSequenceLists::key_t::operator<(const key_t & x) const
   {
  // The first entry that is less then x triggers true.
     for (size_t i=0; i < size(); i++)
        {
          if (x[i] < IRnodeList[i])
               return true;
        }

     return false;
   }

size_t
NormalizeTypedefSequenceLists::key_t::hash(const std::string & s) const
   {
  // Need a hash function to use here!
     size_t returnValue = 0;

  // Compute hash based on sum over all characters in the string. Not a good hash!
     for (size_t i=0; i < s.length(); i++)
        {
          size_t multiplier = 2;
          if (i < 10)
               multiplier = (1 << i);
          returnValue += size_t(s[i]);
        }

     return returnValue;
   }



void
normalizeTypedefSequenceLists()
   {
  // DQ (6/25/2010): This work has been added to support the AST merge mechanism.
  // This function builds up a master set of lists of typedefs for each type in the AST (phase 1).
  // And then (phase 2) copies the master list to the SgTypedefSeq for each type so that they are consistant.
  // This work then supports the AST merge mechanism which has to compute unique names, which the
  // SgTypedefSeq the same for each type the correct names will be computed and for the same
  // types we will have the same generated name and this allows the AST merge to work properly.
  // This also assurs that a minimal representation of the type are present in the AST.
  // This function is called as part of the AST post-processing, and also is called after 
  // reading of multiple binary AST files as preparation for the AST merge.

#if 0
     printf ("Inside of normalizeTypedefSequenceLists() \n");
#endif

  // Phase 1
     NormalizeTypedefSequenceLists t1;

#if 0
     printf ("\n\nBuild the global map of typedef sequences. \n");
#endif

  // To specify the traversal of a specific IR node we pass the traversal object as a parameter.
  // t.traverseMemoryPool();
     SgTypedefSeq::traverseMemoryPoolNodes(t1);

  // Phase 2
     NormalizeTypedefSequenceLists_CopyList t2(t1.typedefSeqMap);

#if 0
     printf ("\n\nCopy the lists from the map of master lists to the lists in each SgTypedefSeq. \n");
#endif

     SgTypedefSeq::traverseMemoryPoolNodes(t2);

#if 0
     printf ("Leaving normalizeTypedefSequenceLists() \n");
#endif

#if 0
     printf ("Exiting after test! \n");
     ROSE_ASSERT(false);
#endif
   }

// vector<size_t>
NormalizeTypedefSequenceLists::key_t
NormalizeTypedefSequenceLists::generateKey(SgTypedefSeq* typedefSeq)
   {
  // Build a key to use in a set of unique types
  // vector<size_t> key;
     key_t key;

     SgTypePtrList & list = typedefSeq->get_typedefs();
     size_t listSize = list.size();

     if (listSize > 0)
        {
       // Accumulate the list of types used.
          SgType* associatedType = isSgType(typedefSeq->get_parent());
          ROSE_ASSERT(associatedType != NULL);

       // Error checking: Build a counter to detect recursion that is likely an error (too deep).
          int counter = 0;

          SgType* baseType = associatedType->stripType(0);
       // bool keepGoing = (isSgPointerType(baseType) != NULL) || (isSgReferenceType(baseType) != NULL) || (isSgArrayType(baseType) != NULL) || (isSgModifierType(baseType) != NULL);
          bool keepGoing = true; // (isSgPointerType(baseType) != NULL) || (isSgReferenceType(baseType) != NULL) || (isSgArrayType(baseType) != NULL) || (isSgModifierType(baseType) != NULL);
          while (keepGoing == true)
             {
            // This loop should be a loop that always terminate

            // printf (" -- baseType = %p = %s \n",baseType,baseType->class_name().c_str());

               key.IRnodeList.push_back(baseType->variantT());

               SgType* previousBaseType = baseType;
               keepGoing = (isSgPointerType(baseType) != NULL) || (isSgReferenceType(baseType) != NULL) || (isSgArrayType(baseType) != NULL) || (isSgModifierType(baseType) != NULL);
               baseType = baseType->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_POINTER_TYPE | SgType::STRIP_ARRAY_TYPE);
               if (baseType == previousBaseType) baseType = NULL;

            // Error checking to make sure that this will terminate with an error if the type is too complex (unlikely in real life).
               counter++;
               if (counter > 10)
                  {
                    printf ("Error, endless recursion in evaluation of base type of type in NormalizeTypedefSequenceLists::visit() (killed at loop counter = %d) \n",counter);

                    printf ("Exiting after test! \n");
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT(key.empty() == false);
        }

     return key;
   }

void
NormalizeTypedefSequenceLists::visit (SgNode* node)
   {
  // printf ("Visiting SgTypedefSeq = %p = %s \n",node,node->class_name().c_str());

     SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
     ROSE_ASSERT(typedefSeq != NULL);

     SgTypePtrList & list = typedefSeq->get_typedefs();
     size_t listSize = list.size();

#if 0
     if (listSize > 0)
        {
          printf ("Visiting typedefSeq = %p list.size() = %zu \n",typedefSeq,listSize);
        }
#endif

     if (listSize > 0)
        {
       // Accumulate the list of types used.
          SgType* associatedType = isSgType(typedefSeq->get_parent());
          ROSE_ASSERT(associatedType != NULL);

       // Build a key to use in a set of unique types
       // vector<size_t> key = generateKey(typedefSeq);
          key_t key = generateKey(typedefSeq);

          ROSE_ASSERT(key.empty() == false);

       // std::map<std::vector<int>, std::vector<SgType*> > typedefSeqMap;
          if (typedefSeqMap.find(key) == typedefSeqMap.end())
             {
#if 0
               printf ("Insert masterList into typedefSeqMap using key \n");
               printf ("   key = ");
               for (size_t i = 0; i < key.IRnodeList.size(); i++)
                  printf ("%zu (%s) ",key[i],Cxx_GrammarTerminalNames[key[i]].name.c_str());
               printf ("\n");
#endif
            // Build an empty list to support the assignment to add a new SgTypePtrList element to the map.
               SgTypePtrList masterList;

            // Insert the empty list onto the map.
               typedefSeqMap[key] = masterList;

               ROSE_ASSERT(typedefSeqMap.find(key) != typedefSeqMap.end());
               ROSE_ASSERT(typedefSeqMap[key].size() == 0);
             }
#if 0
            else
             {
               printf ("key is already present in typedefSeqMap \n");
             }
#endif
       // Generate a master list (to assign to the SgTypePtrList of each element in the
       // equivalence class for all equivalence classes of types that are C/C++/Fotran equivalent.
          for (size_t i = 0; i < list.size(); i++)
             {
            // SgType* typedefTypeNode = isSgType(list[i]);
            // ROSE_ASSERT(typedefTypeNode != NULL);

               SgTypedefType* typedefType = isSgTypedefType(list[i]);
               ROSE_ASSERT(typedefType != NULL);
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(typedefType->get_declaration());
               ROSE_ASSERT(typedefDeclaration != NULL);
            // printf ("typedefDeclaration = %p = %s \n",typedefDeclaration,typedefDeclaration->class_name().c_str());
            // printf ("     typedefDeclaration->get_base_type() = %p = %s \n",typedefDeclaration->get_base_type(),typedefDeclaration->get_base_type()->class_name().c_str());

            // Make sure that the element will be unique in the list
            // ROSE_ASSERT(find(typedefSeqMap[key].begin(),typedefSeqMap[key].end(),typedefType) == typedefSeqMap[key].end());

               SgTypePtrList & localList = typedefSeqMap[key];
            // if ( find(typedefSeqMap[key].begin(),typedefSeqMap[key].end(),typedefType) == typedefSeqMap[key].end() )
               if ( find(localList.begin(),localList.end(),typedefType) == localList.end() )
                  {
                 // Accumulate the element into the list.
                 // masterList.push_back(typedefType);
#if 0
                 // Slower initial version
                    ROSE_ASSERT(typedefSeqMap.find(key) != typedefSeqMap.end());
                    typedefSeqMap[key].push_back(typedefType);
#else
                 // Faster version
                    localList.push_back(typedefType);
#endif
                 // printf ("pointer to master list in map = %p \n",&(typedefSeqMap[key]));
                 // printf ("size of master list in map = %zu size of map = %zu \n",typedefSeqMap[key].size(),typedefSeqMap.size());
                  }
             }
#if 0
       // Output the information accumulated in the typedefSeqMap
          printf ("   typedefSeqMap[key = ");
          for (size_t i = 0; i < key.IRnodeList.size(); i++)
               printf ("%zu (%s) ",key[i],Cxx_GrammarTerminalNames[key[i]].name.c_str());
          printf ("] = ");
          for (size_t i = 0; i < typedefSeqMap[key].size(); i++)
               printf ("%p ",typedefSeqMap[key][i]);
          printf ("\n");
#endif
#if 0
          printf ("Exiting after test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     printf ("Exiting after test! \n");
     ROSE_ASSERT(false);
#endif
   }



NormalizeTypedefSequenceLists_CopyList::NormalizeTypedefSequenceLists_CopyList (NormalizeTypedefSequenceLists::typedefSeqMap_t & l)
   : typedefSeqMap(l)
   {
   }


void
NormalizeTypedefSequenceLists_CopyList::visit (SgNode* node)
   {
  // std::map<std::vector<size_t>, std::vector<SgType*> >::iterator i = typedefSeqMap.begin();

     SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
     ROSE_ASSERT(typedefSeq != NULL);

  // Build a key to use in a set of unique types
  // vector<size_t> key = NormalizeTypedefSequenceLists::generateKey(typedefSeq);
     NormalizeTypedefSequenceLists::key_t key = NormalizeTypedefSequenceLists::generateKey(typedefSeq);

     typedefSeq->get_typedefs() = typedefSeqMap[key];
   }

