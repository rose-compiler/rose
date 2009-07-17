/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>

#include <QRAstInfo.h>
#include <QRPixmap.h>
#include <QRIcons.h>

using namespace std;

namespace qrs {

string QRAstInfo::abstract(string str) {
    string ret;
    if (str[0] == 'e' && str[1] == 'x') return str;
    int n = str.find_first_of("{;");
    ret.assign(str, 0, n);
    return ret;
}


bool QRAstInfo::isRelevant(SgNode *node) {

    switch (node->variantT()) {
 case V_SgProject:
 case V_SgGlobal:
 case V_SgFile:
 case V_SgFunctionDefinition:
 case V_SgIfStmt:
 case V_SgForStatement:
 case V_SgBasicBlock:
    return true;
 default:
    return false;
     }
}

string QRAstInfo::getInfo(SgNode *node) {
    if (isUnparseBlackList(node)) {
       return "";
    } else if (isSgProject(node)) {
       return "Rose Project";
    } else if (isSgGlobal(node)) {
       return "Global";
    } else if (isSgFile(node)) {

    	string result;
    	result = ((SgFile *) node)->getFileName();
    	size_t found = result.find_last_of("/\\");
    	result = result.substr(found + 1);
       return result;
   } else if (isSgTypedefDeclaration(node)) {
       string result;
       result = string("typedef ") +((SgTypedefDeclaration *) node)->get_name().str();
       return result;
   } else if (SgFunctionDeclaration *fdnode = isSgFunctionDeclaration(node)) {
        string result;
       result += fdnode->get_orig_return_type()->unparseToString();
       result += " ";
       result += fdnode->get_name().str();
       SgInitializedNamePtrList lst = fdnode->get_parameterList()->get_args();
       string params;
       for (vector<SgInitializedName *>::iterator iter = lst.begin(); iter != lst.end(); iter++) {
    	   if (!params.empty()) params += ", ";
    	   SgInitializedName *var = *iter;
    	   params += var->get_type()->unparseToString() + " " + var->unparseToString();

       }
       result += "(" + params + ")";
       return result;
   }  else if (isSgMemberFunctionDeclaration(node) ||
		   isSgVariableDeclaration(node) ||
		   isSgExprStatement(node)) {
      return abstract(node->unparseToString());
   } else {
     return abstract(node->unparseToString());
   }
}




QPixmap* QRAstInfo::getPixmap(SgNode *node) {
   switch (node->variantT()) {
      case V_SgProject: return QRPixmap::getPixmap(RoseIcons::project);
      case V_SgFile: return QRPixmap::getPixmap(RoseIcons::source_c);
      case V_SgFunctionDeclaration:
         return QRPixmap::getPixmap( ((SgFunctionDeclaration *) node)->get_definition()?
                        RoseIcons::func_defn : RoseIcons::func_decl);
      case V_SgClassDeclaration:
	 return QRPixmap::getPixmap(RoseIcons::class_decl);
      case V_SgTypedefDeclaration:
	 return QRPixmap::getPixmap(RoseIcons::typedf);
      case V_SgMemberFunctionDeclaration:
	 {

	     SgMemberFunctionDeclaration *fn = (SgMemberFunctionDeclaration *) node;
	     /*if (fn->get_definition()) {
		 // SgMemberFunctionDeclaration with a body does not provide information about access modifier, so we need
		 // to find out from the class definition
		 SgDeclarationStatementPtrList &lstdecl = fn->get_scope()->get_members();
		 SgDeclarationStatementPtrList::iterator iter = lstdecl.begin();
		 while ( (iter != lstdecl.end()) &&
  		        (((*iter)->variantT() != V_SgMemberFunctionDeclaration) ||
			(((SgMemberFunctionDeclaration *) *iter)->get_mangled_name() != fn->get_mangled_name())))
		     iter++;
		 ROSE_ASSERT(iter != lstdecl.end());
		 SgDeclarationModifier &dfm = ((SgMemberFunctionDeclaration *) *iter)->get_declarationModifier();
		 bool not_public = dfm.get_accessModifier().isPrivate() || dfm.get_accessModifier().isProtected();
		 return QRPixmap::getPixmap(not_public? RoseIcons::method_npublic_defn : RoseIcons::method_public_defn);
	     } else */ {
  	         SgDeclarationModifier &dfm = fn->get_declarationModifier();
		 bool not_public = dfm.get_accessModifier().isPrivate() || dfm.get_accessModifier().isProtected();
		 return QRPixmap::getPixmap(not_public? RoseIcons::method_npublic_decl : RoseIcons::method_public_decl);
	     }
	 }
      case V_SgVariableDeclaration:
	 return QRPixmap::getPixmap(RoseIcons::var);
      default: return NULL;
   }
}

bool QRAstInfo::isUnparseBlackList(SgNode *node) {
   switch (node->variantT()) {
      case V_SgTypedefSeq:
      case V_SgClassDefinition: // somehow doesn't unparse correctly


   //   case V_SgPointerType:
   //   case V_SgTypedefDeclaration:
      case V_SgPragma:
      case V_SgNamespaceSymbol:
         return true;
      default:
	 return false;
   }
}

}
