
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseTemplateArgumentList 0

void Unparse_ExprStmt::unparseTemplateArgumentList(const SgTemplateArgumentPtrList & tpl_args_in, SgUnparse_Info& info) {
#if DEBUG__unparseTemplateArgumentList
     printf("Enter Unparse_ExprStmt::unparseTemplateArgumentList():\n");
     printf("  tpl_args_in.size() = %" PRIuPTR " \n", tpl_args_in.size());
#endif

     SgUnparse_Info ninfo(info);

     if (ninfo.isTypeFirstPart())  ninfo.unset_isTypeFirstPart();
     if (ninfo.isTypeSecondPart()) ninfo.unset_isTypeSecondPart();

     SgTemplateArgumentPtrList templateArgListPtr;
     SgTemplateArgumentPtrList::const_iterator copy_iter = tpl_args_in.begin();
     bool isEmptyTemplateArgumentList = true;
     while (copy_iter != tpl_args_in.end()) {
       isEmptyTemplateArgumentList = false;

       SgTemplateArgument * tplarg = *(copy_iter++);
       ASSERT_not_null(tplarg);

       bool skipTemplateArgument = false;
       bool stopTemplateArgument = false;
       tplarg->outputTemplateArgument(skipTemplateArgument, stopTemplateArgument);

#if DEBUG__unparseTemplateArgumentList
       printf("  - tplarg = %s\n", tplarg->unparseToString().c_str());
       printf("    skipTemplateArgument = %d\n", skipTemplateArgument);
       printf("    stopTemplateArgument = %d\n", stopTemplateArgument);
#endif

       if (stopTemplateArgument)
         break;
       else if (!skipTemplateArgument)
         templateArgListPtr.push_back(tplarg);
     }

     if (isEmptyTemplateArgumentList == false) {
       unp->u_exprStmt->curprint ( "< ");
       SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr.begin();
       while (i != templateArgListPtr.end()) {
         ROSE_ASSERT((*i)->get_argumentType() != SgTemplateArgument::start_of_pack_expansion_argument);
         unparseTemplateArgument(*i,ninfo);
         i++;
         if (i != templateArgListPtr.end()) {
           if ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument) break;
           unp->u_exprStmt->curprint(" , ");
         }
       }
       unp->u_exprStmt->curprint(" > ");
     } else {
       ROSE_ASSERT(templateArgListPtr.empty() == true);
       unp->u_exprStmt->curprint("<>");
     }
#if DEBUG__unparseTemplateArgumentList
     printf("Leave Unparse_ExprStmt::unparseTemplateArgumentList():\n");
#endif
   }