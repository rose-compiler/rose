#include <rose.h>
#include <fstream>
#include <iostream>
#include <time.h>

#define DEBUG(tf) if (tf) 

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
namespace CanonicalLoopChecker { // must put into a namespace to avoid collision with other enum values!!
enum canonical_violation_code_enum {
	e_none = 0, // no violation is detected
	e_not_a_for_loop ,
	e_empty_init_stmt,
	e_init_stmt_multiple_stmt, // multiple initialization statements
	e_init_expr_multiple_expression , // not a recognized form
	e_init_expr_unexpected_form, // other
	e_init_expr_illegal_data_type, // not integer, pointer, or iterator type
	e_empty_test_exp, // no test expression
	e_test_exp_illigal_relational_operator,
	e_test_exp_empty_operand,
	e_test_exp_no_loop_index,
	e_empty_incr_exp,
	e_incr_exp_illegal_operation,
	e_incr_exp_empty_incr_var,
	e_incr_exp_incr_var_not_match,
	e_last   // last unused one
};
}

using namespace CanonicalLoopChecker;

std::string toString(canonical_violation_code_enum v_code)
{
	std::string rt = "unrecognized violation code";
	switch (v_code)
	{
	case e_none:
		rt = "e_none";
		break;
	case e_not_a_for_loop:
		rt = "e_not_a_for_loop";
		break;
	case e_empty_init_stmt:
		rt = "e_empty_init_stmt";
		break;
	case e_init_stmt_multiple_stmt:
		rt = "e_init_stmt_multiple_stmt";
		break;
	case e_init_expr_multiple_expression:
		rt ="e_init_expr_multiple_expression";
		break;
	case e_init_expr_unexpected_form:
		rt ="e_init_expr_unexpected_form";
		break;
	case e_init_expr_illegal_data_type:
		rt = "e_init_expr_illegal_data_type";
		break;
	case e_empty_test_exp:
		rt = "e_empty_test_exp";
		break;
	case e_test_exp_illigal_relational_operator:
		rt = "e_test_exp_illigal_relational_operator";
		break;
	case e_test_exp_empty_operand:
		rt = "e_test_exp_empty_operand";
		break;
	case e_test_exp_no_loop_index:
		rt = "e_test_exp_no_loop_index";
		break;
	case e_empty_incr_exp:
		rt = "e_empty_incr_exp";
		break;
	case e_incr_exp_illegal_operation:
		rt = "e_incr_exp_illegal_operation";
		break;
	case e_incr_exp_empty_incr_var:
		rt = "e_incr_exp_empty_incr_var";
		break;
	case e_incr_exp_incr_var_not_match:
		rt = "e_incr_exp_incr_var_not_match";
		break;
	case e_last :
		rt = "e_last";
		break;
	default:
		cerr<<"v_code is unrecognized:"<< v_code <<endl;
		ROSE_ASSERT (false);
	}
	return rt;
}
//! Check if a for loop is a canonical for loop as defined by OpenMP 4.0 specification (in page 51 or (59/320): section 2.6)
// www.openmp.org/mp-documents/OpenMP4.0.0.pdf
bool isCanonicalOpenMPForLoop(SgNode* loop, canonical_violation_code_enum* = NULL, SgInitializedName** ivar=NULL, SgExpression** lb=NULL, SgExpression** ub=NULL, 
		SgExpression** step=NULL, SgStatement** body=NULL, bool *hasIncrementalIterationSpace= NULL,
		bool* isInclusiveUpperBound=NULL);

// default file name to store the report
static string report_filename = "loopChecker.report.txt";
static string report_option ="-report-file";
ofstream report_file;
long node_number;

class visitorTraversal : public AstSimpleProcessing
{
protected:
	void virtual visit (SgNode* n)
	{
		node_number++;

		if (isSgForStatement(n)!=NULL) {
			SgForStatement* loop = isSgForStatement(n);
			if (loop->get_file_info()->isCompilerGenerated())
				return;
			canonical_violation_code_enum v_code ;

			if (!isCanonicalOpenMPForLoop (loop, &v_code))
			{
				// // Screen output
				// cout<<"Found a non-canonical loop:"<<endl;
				// cout<<loop->get_file_info()->get_filename()<<":"<< loop->get_file_info()->get_line();
				// cout<<" violation:"<< toString(v_code) <<endl;
				// report_file << loop->get_file_info()->get_filename()<<":"<< loop->get_file_info()->get_line();
				// report_file << " violation:"<< toString(v_code) <<endl;
			}
			else  // for canonical loops, we further try to figure out if there are potential reduction variables
			{
				// Screen output
				DEBUG(false) cout<<"Found a canonical loop: trying to find possible reduction variables..."<<endl;
				cout<<loop->get_file_info()->get_filename()<<":"<< loop->get_file_info()->get_line()<<endl;

				report_file <<loop->get_file_info()->get_filename()<<":"<< loop->get_file_info()->get_line()<<endl; /* ! */

				std::set < std::pair <SgInitializedName*, VariantT> > reduction_pairs;
				ReductionRecognition (loop, reduction_pairs);
				// if (reduction_pairs.size() >0)
				// {
				//   // write to a file also
				//   ofstream report_file(report_filename.c_str(), ios::app);
				//   DEBUG(false) report_file <<"Canonical loop with possible reduction variable(s):"<<endl;
				//   report_file <<loop->get_file_info()->get_filename()<<":"<< loop->get_file_info()->get_line()<<endl;
				//   std::set < std::pair <SgInitializedName*, VariantT> > ::iterator iter;
				//   for (iter =  reduction_pairs.begin(); iter!= reduction_pairs.end(); iter ++)
				//   {
				//     report_file <<"\t"<< ((*iter).first)->get_name() << ":"<<getVariantName((*iter).second)<<endl;
				//   }
				// } // end if has reduction variables
			} // end canonical loop
		} // end if (forloop)
	} // end visit()
};



int main (int argc, char** argv)
{
	// Build the AST used by ROSE
	vector <string> argvList (argv, argv + argc);

	// clock_gettime() nanosecond resolution
	struct timespec ts_now;
	long mtime_0, mtime_1, mtime_ft;

	if (CommandlineProcessing::isOption(argvList,"--help","", false))
	{
		cout<<"---------------------Tool-Specific Help-----------------------------------"<<endl;
		cout<<"This is a source analysis to find non-canonical for loops in your C/C++ code."<<endl;
		cout<<"Usage: "<<argvList[0]<<" -c ["<<report_option<<" result.txt] "<< "input.c"<<endl;
		cout<<endl;
		cout<<"The optional "<<report_option<<" option is provided for users to specify where to save the results"<<endl;
		cout<<"By default, the results will be saved into a file named loopCheckerReport.txt"<<endl;
		cout<<"----------------------Generic Help for ROSE tools--------------------------"<<endl;
	}

	if (CommandlineProcessing::isOptionWithParameter (argvList, report_option,"", report_filename,true))
	{
		cout<<"Using user specified file: "<<report_filename<<" for storing results."<<endl;
	}
	//  else
	//    cout<<"Using the default file:"<<report_filename<<" for storing results."<<endl;
	report_file.open(report_filename.c_str(), ios::out);


	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_0 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	SgProject* project = frontend(argvList);

	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_1 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	mtime_ft = mtime_1 - mtime_0;

	// Insert your own manipulations of the AST here...
	SgFilePtrList file_ptr_list = project->get_fileList();

	// --timer anchor 0
	clock_gettime(CLOCK_MONOTONIC, &ts_now);
	mtime_0 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	node_number = 0;
	visitorTraversal exampleTraversal;
	for (size_t i = 0; i<file_ptr_list.size(); i++)
	{
		SgFile* cur_file = file_ptr_list[i];
		SgSourceFile* s_file = isSgSourceFile(cur_file);
		if (s_file != NULL)
		{
			//exampleTraversal.traverseInputFiles(project,preorder);
			exampleTraversal.traverseWithinFile(s_file, preorder);
		}
	}

	// --timer anchor 1
	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_1 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	cout << "--------------------------------------------------" << endl;
	cout << "time#" << mtime_ft << "#" << mtime_1 - mtime_0  << endl;
	cout << "--------------------------------------------------" << endl;

	report_file << "--------------------------------------------------" << endl;
	report_file << "time#" << mtime_ft << "#" << mtime_1 - mtime_0  << endl;
	report_file << "--------------------------------------------------" << endl;
	report_file << "#node number#" << node_number << endl;
	report_file << "--------------------------------------------------" << endl;
	// Generate source code from AST and invoke your
	// desired backend compiler
	// return backend(project);
	return 0;
}

//! A helper function to strip off possible type casting operations for an expression
// usually useful when compare two expressions to see if they actually refer to the same variable
static SgExpression* SkipCasting (SgExpression* exp)
{
	SgCastExp* cast_exp = isSgCastExp(exp);
	if (cast_exp != NULL)
	{
		SgExpression* operand = cast_exp->get_operand();
		assert(operand != 0);
		return SkipCasting(operand);
	}
	else
		return exp;
}

//! Based on AstInterface::IsFortranLoop() and ASTtools::getLoopIndexVar()
//TODO check the loop index is not being written in the loop body
bool isCanonicalOpenMPForLoop(SgNode* loop, canonical_violation_code_enum* v_code, SgInitializedName** ivar/*=NULL*/, SgExpression** lb/*=NULL*/, SgExpression** ub/*=NULL*/, SgExpression** step/*=NULL*/, SgStatement** body/*=NULL*/, bool *hasIncrementalIterationSpace/*= NULL*/, bool* isInclusiveUpperBound/*=NULL*/)
{
	ROSE_ASSERT(loop != NULL);
	SgForStatement* fs = isSgForStatement(loop);
	//SgFortranDo* fs2 = isSgFortranDo(loop);
	if (fs == NULL)
	{
		// if (fs2)
		//   return isCanonicalDoLoop (fs2, ivar, lb, ub, step, body, hasIncrementalIterationSpace, isInclusiveUpperBound);
		// else
		*v_code = e_not_a_for_loop;
		return false;
	}
	// 1. Check initialization statement is something like i=xx;
	//--------------------------------------------------------------------------------
	/*
	 *init-expr One of the following:
	 *
	 * var = lb
	 * integer-type var = lb
	 * random-access-iterator-type var = lb
	 * pointer-type var = lb
	 *
	 *
	 */
	SgStatementPtrList & init = fs->get_init_stmt();
	if (init.size() == 0)
	{
		*v_code = e_empty_init_stmt;
		return false;
	}

	if (init.size() >1)
	{
		*v_code = e_init_stmt_multiple_stmt;
		return false;
	}

	SgStatement* init1 = init.front();
	SgExpression* ivarast=NULL, *lbast=NULL, *ubast=NULL, *stepast=NULL;
	SgInitializedName* ivarname=NULL;

	bool isCase1=false, isCase2=false;
	//consider C99 style: for (int i=0;...)
	if (isSgVariableDeclaration(init1))
	{
		SgVariableDeclaration* decl = isSgVariableDeclaration(init1);
		ivarname = decl->get_variables().front();
		ROSE_ASSERT(ivarname != NULL);
		SgInitializer * initor = ivarname->get_initializer();
		if (isSgAssignInitializer(initor))
		{
			lbast = isSgAssignInitializer(initor)->get_operand();
			isCase1 = true;
		}
	}// other regular case: for (i=0;..)
	else if (isAssignmentStatement(init1, &ivarast, &lbast))
	{ // integer-type var = lb
		SgVarRefExp* var = isSgVarRefExp(SkipCasting(ivarast));
		if (var)
		{
			ivarname = var->get_symbol()->get_declaration();
			isCase2 = true;
		}
	}
	// Cannot be both true
	ROSE_ASSERT(!(isCase1&&isCase2));
	// if not either case is true
	if (!(isCase1||isCase2))
	{
		if (isSgExprStatement(init1))
			if (isSgCommaOpExp(isSgExprStatement(init1)->get_expression()))
				*v_code = e_init_expr_multiple_expression;
		if (*v_code != e_init_expr_multiple_expression )
			*v_code = e_init_expr_unexpected_form;
		return false;
	}

	//type check
	// must be either integer,  pointer , or random-access iterator type
	//Check loop index's type
	// TODO allow random access iterator type
	if (!isStrictIntegerType(ivarname->get_type()) && ! isSgPointerType(ivarname->get_type()))
	{
		*v_code = e_init_expr_illegal_data_type;
		return false;
	}

	//2. Check test expression i [<=, >=, <, > ,!=] bound
	//--------------------------------------------------------------------------------
	/*
	 * test-expr One of the following:
	 *
	 *  var relational-op b
	 *  b relational-op var
	 *
	 * relational-op One of the following:
	 *
	 *   <
	 *   <=
	 *   >
	 *   >=
	 *
	 */
	SgBinaryOp* test = isSgBinaryOp(fs->get_test_expr());
	if (test == NULL)
	{
		*v_code = e_empty_test_exp;
		return false;
	}
	switch (test->variantT()) {
	case V_SgLessOrEqualOp: // <=
		if (isInclusiveUpperBound != NULL)
			*isInclusiveUpperBound = true;
		if (hasIncrementalIterationSpace != NULL)
			*hasIncrementalIterationSpace = true;
		break;
	case V_SgLessThanOp: // <
		if (isInclusiveUpperBound != NULL)
			*isInclusiveUpperBound = false;
		if (hasIncrementalIterationSpace != NULL)
			*hasIncrementalIterationSpace = true;
		break;
	case V_SgGreaterOrEqualOp: // >=
		if (isInclusiveUpperBound != NULL)
			*isInclusiveUpperBound = true;
		if (hasIncrementalIterationSpace != NULL)
			*hasIncrementalIterationSpace = false;
		break;
	case V_SgGreaterThanOp: // >
		if (isInclusiveUpperBound != NULL)
			*isInclusiveUpperBound = false;
		if (hasIncrementalIterationSpace != NULL)
			*hasIncrementalIterationSpace = false;
		break;
	default:
		*v_code = e_test_exp_illigal_relational_operator;
		return false;
	}

	// check the tested variable is the same as the loop index
	// OpenMP allows two orders:  var-OP-b or b-OP-var
	SgVarRefExp* testvar_lhs = isSgVarRefExp(SkipCasting(test->get_lhs_operand()));
	SgVarRefExp* testvar_rhs = isSgVarRefExp(SkipCasting(test->get_rhs_operand()));
	if (testvar_lhs == NULL && testvar_rhs == NULL)
	{
		*v_code = e_test_exp_empty_operand;
		return false;
	}
	SgVariableSymbol * sym = isSgVariableSymbol(ivarname->get_symbol_from_symbol_table ());
	ROSE_ASSERT (sym != NULL);
	if ((testvar_lhs->get_symbol() !=  sym) && (testvar_rhs ->get_symbol() != sym))
	{
		*v_code  = e_test_exp_no_loop_index;
		return false;
	}
	//grab the upper bound
	if (testvar_lhs->get_symbol() ==  sym)
		ubast = test->get_rhs_operand();
	else
		ubast = test->get_lhs_operand();

	// TODO:if the order changes, incremental, lower, upper bounds will be switched!

	//3. Check the increment expression
	//--------------------------------------------------------------------------------
	/* incr-expr One of the following:
	 *
	 *  ++var
	 *  var++
	 *  --var
	 *  var--
	 *  var += incr
	 *  var -= incr
	 *  var = var + incr
	 *  var = incr + var
	 *  var = var - incr
	 *
	 */
	SgExpression* incr = fs->get_increment();
	SgVarRefExp* incr_var = NULL;
	if (incr == NULL)
	{
		*v_code = e_empty_incr_exp;
		return false;
	}
	switch (incr->variantT()) {
	case V_SgPlusAssignOp: //+=
	case V_SgMinusAssignOp://-=
		incr_var = isSgVarRefExp(SkipCasting(isSgBinaryOp(incr)->get_lhs_operand()));
		stepast = isSgBinaryOp(incr)->get_rhs_operand();
		break;
	case V_SgPlusPlusOp:   //++
	case V_SgMinusMinusOp:  //--
		incr_var = isSgVarRefExp(SkipCasting(isSgUnaryOp(incr)->get_operand()));
		stepast = buildIntVal(1); // will this dangling SgNode cause any problem?
		break;
	default:
	{
		*v_code = e_incr_exp_illegal_operation;
		return false;
	}
	}
	if (incr_var == NULL)
	{
		*v_code = e_incr_exp_empty_incr_var;
		return false;
	}
	if (incr_var->get_symbol() != ivarname->get_symbol_from_symbol_table ())
	{
		*v_code = e_incr_exp_incr_var_not_match;
		return false;
	}

	// return loop information if requested
	if (ivar != NULL)
		*ivar = ivarname;
	if (lb != NULL)
		*lb = lbast;
	if (ub != NULL)
		*ub = ubast;
	if (step != NULL)
		*step = stepast;
	if (body != NULL) {
		*body = fs->get_loop_body();
	}
	return true;
}




