#include "FastNumericsRoseSupport.h"

#include "rose.h"

#include "TypeAttribute.h"
#include "sageBuilder.h"

#include "utility/utils.h"

namespace FastNumericsRoseSupport
{
    /*
    Collects all the function declarations(defining) from the current matlab file.
    The function declarations created will have its file_info node to have fileName "transformation"
   */

  Rose_STL_Container<SgFunctionDeclaration*> getMatlabFunctions(SgNode *scope)
  {
    Rose_STL_Container<SgFunctionDeclaration*> matlabFunctions;

    Rose_STL_Container<SgNode*> allFunctions = NodeQuery::querySubTree(scope, V_SgFunctionDeclaration);

    for(size_t i = 0; i < allFunctions.size(); ++i)
      {
        SgFunctionDeclaration *functionDeclaration = isSgFunctionDeclaration(allFunctions[i]);

        std::string fileName = functionDeclaration->get_file_info()->get_filename(); //declarations filename

        if(fileName == "transformation") //Only insert those declarations in the main file
          {
            matlabFunctions.push_back(isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration()));
          }
      }

    return matlabFunctions;

  }

  NameToDeclarationMap
  collectNameToDeclarationPair(Rose_STL_Container<SgFunctionDeclaration*> matlabFunctions)
  {
    NameToDeclarationMap                       nameToFuncDeclarations;
    Rose_STL_Container<SgFunctionDeclaration*> allFunctions = matlabFunctions;

    for(size_t i = 0; i < allFunctions.size(); ++i)
    {
      SgFunctionDeclaration* functionDeclaration = allFunctions[i];
      std::string            name = functionDeclaration->get_name().getString();
      std::string            fileName = functionDeclaration->get_file_info()->get_filename(); //declarations filename
      SgFunctionDeclaration* definingDecl = isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration());

      ROSE_ASSERT(definingDecl);
      nameToFuncDeclarations[name].push_back(MatlabFunctionRec(definingDecl, false));
    }

    return nameToFuncDeclarations;
  }


  void updateVarRefType(SgVarRefExp *var_ref, SgType *newType)
  {
    SgVariableSymbol *variableSymbol = var_ref->get_symbol();
    SgInitializedName *initializedName = variableSymbol->get_declaration();

    initializedName->set_type(newType);
  }

  /*
    Update the type of the variable as well as create a new variable declaration
   */
  void updateVarRefTypeWithDeclaration(SgVarRefExp *var_ref, SgType *newType)
  {
    updateVarRefType(var_ref, newType);
  }


  void updateVarRefTypeWithDominantType(SgVarRefExp *var_ref, TypeAttribute  *inferredType)
  {
    if(SgType *existingType = getVarType(var_ref))
      {
        //This variable has some other type.
        //Check if the new type dominates this type. If yes, update it with new type, else leave it as is

        TypeAttribute *existingTypeAttribute = TypeAttribute::buildAttribute(existingType);

        Rose_STL_Container<TypeAttribute*> attributes;
        attributes.push_back(existingTypeAttribute);
        attributes.push_back(inferredType);

        SgType *dominantType = get_dominant_type(attributes);

        updateVarRefType(var_ref, dominantType);

        //TypeAttribute *existingTypeAttribute = TypeAttribute::buildAttribute(existingType);

        TypeAttribute *dominantTypeAttribute = TypeAttribute::buildAttribute(dominantType);

        dominantTypeAttribute->attach_to(var_ref);
      }
    else
      {
        updateVarRefType(var_ref, inferredType->get_inferred_type());
        inferredType->attach_to(var_ref);
      }
  }

  SgType* getVarType(SgVarRefExp *var_ref)
  {
    SgType *type = var_ref->get_type();

    if(type->variantT() == V_SgTypeUnknown)
      return SageBuilder::buildIntType(); // \todo \pp QUICK FIX, was null
    else
      return type;
  }

  bool isParameter(SgVarRefExp *var_ref, SgFunctionDeclaration *functionDeclaration)
  {
    SgInitializedName *varInitName = var_ref->get_symbol()->get_declaration();

    //If the initializedName's parent = parameterlist, then the var is a parameter
    return  (varInitName->get_parent() == functionDeclaration->get_parameterList());
  }

  /// Extracts the type from the attached TypeAttribute
  SgType* getInferredType(SgNode* node)
  {
    ROSE_ASSERT(node);
    
    TypeAttribute* attachedTypeAttribute = TypeAttribute::get_typeAttribute(node);
    ROSE_ASSERT(attachedTypeAttribute != NULL);

    return attachedTypeAttribute->get_inferred_type();
  }

   SgType *getInferredType(SgVarRefExp *var_ref)
  {
    SgType* res = getVarType(var_ref);

    if (!res) res = getInferredType(static_cast<SgNode*>(var_ref));
    return res;
  }

}
