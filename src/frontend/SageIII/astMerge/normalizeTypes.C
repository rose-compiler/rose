// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"

// #include "colorTraversal.h"

using namespace std;


// This traversal might become a part of the AST post-processing.
// the alternative would be to introduce a mechanism to support
// the sharing of SgNamedTypes as they are built, this would be 
// complicated, because the state would have to be preserved 
// across all AST file I/O.  In this fixup we also have to worry 
// about primative types (e.g. SgIntType) that could be multiply 
// declared (from builtin's associated with multiple files read 
// using AST file I/O).
class NormalizeTypesTraversal : public ROSE_VisitTraversal
   {
  // Fixup all types to be shared or have equivalent typedef sequences.
  // Note that multiple types could exist and be different becuase they 
  // have different typedef in there SgTypedefSeq object.
     public:
       // For all SgDeclarations we need the set of all SgTypes
          typedef multimap<SgNode*,SgNode*> DeclarationTypeMultiMapType;

          DeclarationTypeMultiMapType declarationTypeMultiMap;

          void visit ( SgNode* node);

          void displayDeclarationTypeMultiMap();

       // This avoids a warning by g++
          virtual ~NormalizeTypesTraversal(){};
   };

void
NormalizeTypesTraversal::displayDeclarationTypeMultiMap()
   {
     printf ("In declarationTypeMultiMap(): declarationTypeMultiMap.size() = %" PRIuPTR " \n",declarationTypeMultiMap.size());
     DeclarationTypeMultiMapType::iterator i = declarationTypeMultiMap.begin();
     while (i != declarationTypeMultiMap.end())
        {
          SgNode* declaration = i->first;
          SgNode* namedType   = i->second;
          ROSE_ASSERT(declaration != NULL);
          ROSE_ASSERT(namedType != NULL);

          printf ("In declarationTypeMultiMap(): declaration = %p = %s namedType = %p = %s \n",
               declaration,declaration->class_name().c_str(),namedType,namedType->class_name().c_str());

          i++;
        }
   }

void
NormalizeTypesTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
  // printf ("NormalizeTypesTraversal::visit: node = %p = %s \n",node,node->class_name().c_str());

     SgType* type = isSgType(node);
     if (type != NULL)
        {
          SgNamedType* namedType = isSgNamedType(type);
          if (namedType != NULL)
             {
               SgDeclarationStatement* declaration = isSgDeclarationStatement(namedType->get_declaration());
               declarationTypeMultiMap.insert(pair<SgNode*,SgNode*>(declaration,type));
             }
        }
   }

class FixupTypesTraversal : public ROSE_VisitTraversal
   {
  // Fixup all types to be shared or have equivalent typedef sequences.
  // Note that multiple types could exist and be different becuase they 
  // have different typedef in there SgTypedefSeq object.
     public:
          NormalizeTypesTraversal::DeclarationTypeMultiMapType & declarationTypeMultiMap;

          void visit ( SgNode* node);

          FixupTypesTraversal(NormalizeTypesTraversal::DeclarationTypeMultiMapType & m) : declarationTypeMultiMap(m) {};

       // This avoids a warning by g++
          virtual ~FixupTypesTraversal(){};
   };

void
FixupTypesTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
  // printf ("FixupTypesTraversal::visit: node = %p = %s \n",node,node->class_name().c_str());

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
#if 0
          printf ("FixupTypesTraversal::visit: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
          NormalizeTypesTraversal::DeclarationTypeMultiMapType::iterator lowerBound = declarationTypeMultiMap.lower_bound(declaration);
          NormalizeTypesTraversal::DeclarationTypeMultiMapType::iterator upperBound = declarationTypeMultiMap.upper_bound(declaration);

       // Increment the upperBound just past the end
       // if (upperBound != declarationTypeMultiMap.end())
       //      upperBound++;
       // if ( lowerBound != declarationTypeMultiMap.end() )
#if 0
          if ( upperBound != declarationTypeMultiMap.end() && (lowerBound != upperBound) )
             {
               while (lowerBound != upperBound)
                  {
                    printf ("loop: lowerBound = %p = %s \n",lowerBound->second,lowerBound->second->class_name().c_str());
                    lowerBound++;
                  }

               if (upperBound != declarationTypeMultiMap.end())
                    printf ("end: upperBound = %p = %s \n",upperBound->second,upperBound->second->class_name().c_str());
             }
#endif
       // if ( upperBound != declarationTypeMultiMap.end() && (lowerBound != upperBound) )
       // if ( upperBound != declarationTypeMultiMap.end() )
          NormalizeTypesTraversal::DeclarationTypeMultiMapType::iterator i = lowerBound;
          if ( i != declarationTypeMultiMap.end() )
             {
            // There must be at least two reference to what we want to be a shared type
            // printf ("end: upperBound = %p = %s \n",upperBound->second,upperBound->second->class_name().c_str());
            // printf ("loop: lowerBound = %p = %s \n",lowerBound->second,lowerBound->second->class_name().c_str());
               while (i != upperBound)
                  {
#if 0
                    printf ("loop: i = %p = %s \n",i->second,i->second->class_name().c_str());
#endif
                    i++;
                  }
             }
        }
   }

void
normalizeTypesTraversal()
   {
     NormalizeTypesTraversal t1;
     t1.traverseMemoryPool();

  // t1.displayDeclarationTypeMultiMap();

     FixupTypesTraversal t2(t1.declarationTypeMultiMap);
     t2.traverseMemoryPool();
   }



