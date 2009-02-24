/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef C2PROMELAUNPARSER_H
#define C2PROMELAUNPARSER_H


#include "unparser_opt.h"
#include "unparse_format.h"
#include "unparser.h"

class CToProMeLaUnparser: public Unparser
   {
     public:
      //! constructor
          CToProMeLaUnparser( std::ostream* localStream, std::string filename, Unparser_Opt info, int lineNumberToUnparse, UnparseFormatHelp *h = NULL, UnparseDelegate* repl = NULL);

      //! destructor
          virtual ~CToProMeLaUnparser();
      //! friend string globalUnparseToString ( SgNode* astNode );
//          void unparseProject ( SgProject* project, SgUnparse_Info& info );
//          void unparseFile    ( SgFile* file, SgUnparse_Info& info );
          void unparseWhileStmt			(SgStatement* stmt, SgUnparse_Info& info);
					void unparseIfStmt   			(SgStatement* stmt, SgUnparse_Info& info);
					void unparseBasicBlockStmt (SgStatement* stmt, SgUnparse_Info& info);
					void unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info);
					void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);
					void unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info);
					void unparseAttachedPreprocessingInfo(SgStatement* stmt, SgUnparse_Info& info,
								   PreprocessingInfo::RelativePositionType whereToUnparse);
			protected:
				std::ostream *promCur;
				bool toUnparse(SgStatement * node);
   //       void unparseDoWhileStmt      (SgStatement* stmt, SgUnparse_Info& info);
   };
void unparsePromelaFile ( SgFile* file, UnparseFormatHelp *unparseHelp, UnparseDelegate* unparseDelegate );
void unparsePromelaProject( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate);

#endif
