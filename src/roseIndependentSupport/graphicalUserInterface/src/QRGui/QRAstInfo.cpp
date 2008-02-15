/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "QRAstInfo.h"
#include "QRPixmap.h"

#include <stdio.h>

using namespace std;

string QRAstInfo::abstract(string str) {
    string ret;
    if (str[0] == 'e' && str[1] == 'x') return str;
    int n = str.find_first_of("{;");
    ret.assign(str, 0, n);
    return ret;
}


bool QRAstInfo::is_relevant(SgNode *node) {
    
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

string QRAstInfo::get_info(SgNode *node) {

    if (isSgExpression(node)) {
//	return node->unparseToString();
    }
    switch (node->variantT()) {
	case V_SgProject: return "Rose Project";
	case V_SgGlobal: return "Global";
	case V_SgFile: 
	   {
	      char *filename = ((SgFile *) node)->getFileName();
	      char *ptr = strrchr(filename, '/');
	      if (ptr) return (char *) (ptr + 1);
	      else return filename;	      
	  }	
	case V_SgTypedefDeclaration:
	{
	      
	      string result;
	      result = string("typedef ") +((SgTypedefDeclaration *) node)->get_name().str();
	      return result;
	}
	
	case V_SgFunctionDeclaration:   
/*	{ 
	      
	      string result; SgFunctionDeclaration *fdnode = (SgFunctionDeclaration *) node;	      
	      result += fdnode->get_orig_return_type()->unparseToString();
	      result += " ";
	      result += fdnode->get_name().str();
	      result += fdnode->get_parameterList()->unparseToString();
	      return result;
	} 
*/	  
case V_SgMemberFunctionDeclaration:			 
        case V_SgVariableDeclaration:	
	case V_SgExprStatement:			 
	   return abstract(node->unparseToString());	   	  
	default:  return node->sage_class_name();
    };
}
  


	 
QPixmap* QRAstInfo::get_pixmap(SgNode *node) {
   switch (node->variantT()) {
      case V_SgProject: return QRPixmap::get_pixmap(icons::project);
      case V_SgFile: return QRPixmap::get_pixmap(icons::source_c); 
      case V_SgFunctionDeclaration: 
         return QRPixmap::get_pixmap( ((SgFunctionDeclaration *) node)->get_definition()? icons::func_defn : icons::func_decl);
      case V_SgClassDeclaration: 
	 return QRPixmap::get_pixmap(icons::class_decl); 
      case V_SgTypedefDeclaration:
	 return QRPixmap::get_pixmap(icons::typedf); 
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
		 return QRPixmap::get_pixmap(not_public? icons::method_npublic_defn : icons::method_public_defn);		 
	     } else */ {
  	         SgDeclarationModifier &dfm = fn->get_declarationModifier();
		 bool not_public = dfm.get_accessModifier().isPrivate() || dfm.get_accessModifier().isProtected();
		 return QRPixmap::get_pixmap(not_public? icons::method_npublic_decl : icons::method_public_decl);
	     }  
	 }
      case V_SgVariableDeclaration:	 
	 return QRPixmap::get_pixmap(icons::var);
      default: return NULL;
   }
} 



