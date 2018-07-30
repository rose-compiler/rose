#ifndef FAST_NUMERICS_ROSE_SUPPORT_H
#define FAST_NUMERICS_ROSE_SUPPORT_H

#include <map>
#include <string>
#include <utility>

#include "rose.h"
#include "TypeAttribute.h"

namespace FastNumericsRoseSupport
{
  typedef std::pair<SgFunctionDeclaration*, bool>  MatlabFunctionRec;
  typedef std::vector<MatlabFunctionRec>           MatlabOverloadSet;
  typedef std::map<std::string, MatlabOverloadSet> NameToDeclarationMap;

  Rose_STL_Container<SgFunctionDeclaration*> getMatlabFunctions(SgNode *scope);

  NameToDeclarationMap
  collectNameToDeclarationPair(Rose_STL_Container<SgFunctionDeclaration*> matlabFunctions);

  void updateVarRefType(SgVarRefExp *var_ref, SgType *newType);
  void updateVarRefTypeWithDeclaration(SgVarRefExp *var_ref, SgType *newType);
  SgName extractSgName(const SgVarRefExp *var_ref);
  std::string extractStringName(const SgVarRefExp *var_ref);
  bool isParameter(SgVarRefExp *var_ref, SgFunctionDeclaration *functionDeclaration);
  SgType* getInferredType(SgNode *node);
  SgType *getInferredType(SgVarRefExp *var_ref);
  /* template <class TList> */
  /*   SgType* get_dominant_type(TList typeAttributeList); */

  template <class TList>
  SgType* get_dominant_type(TList typeAttributeList)
  {
    TypeFlag typeFlag = (TypeFlag) 0;

    //Go through all the children types which are not null and extract the dominant type

    SgType *dominantType = NULL;

    for(typename TList::iterator listIterator = typeAttributeList.begin();
        listIterator != typeAttributeList.end(); ++listIterator)
      {

        if(*listIterator != NULL)
          {
            TypeAttribute *typeAttribute = *listIterator;

            MatlabTypeInfo currentTypeInfo = typeAttribute->get_type_info();

            SgType *currentType = typeAttribute->get_inferred_type();

            //            std::cout << "Current: " << currentType->class_name() << std::endl;

            if(currentTypeInfo.flag > typeFlag)
              {
                typeFlag = (TypeFlag) currentTypeInfo.flag;
                dominantType = currentType;
              }

            // if(SgTypeMatrix *matrixType = isSgTypeMatrix(currentType))
            //   std::cout << matrixType->get_base_type()->class_name();

          }
      }

    return dominantType;
  }


  void updateVarRefTypeWithDominantType(SgVarRefExp *var_ref, TypeAttribute *inferredType);
  SgType* getVarType(SgVarRefExp *var_ref);
}

#endif
