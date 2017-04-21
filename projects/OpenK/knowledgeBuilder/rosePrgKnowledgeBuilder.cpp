#include <rose.h>
#include <iostream>
#include <fstream>
#include <string>
//#include <map>
#include <time.h>


#include "OntoGeneratorInterface.hpp"
#include "rosePrgKnowledgeBuilder.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace KG;


OntoGeneratorInterface * onto_generator;

// control if turn off the decorated line
#define ON(tag) if(tag)
#define BUILDTAG true

#define N2S(x) rose::StringUtility::numberToString((x))

// compose a URI with prefix p and x
#define MAKE_IRI_W_PREFIX(p, x) (std::string(p) + ":" + (x))

//assert an individual for a concept
#define MClassAssertion(c, ind) onto_generator->ClassAssertion(MAKE_IRI_W_PREFIX("c", c), current_file_prefix +":"+ (ind))

// Make C object Property relation between two individual language constructs 
// The Object Property has 'c' as the C ontology prefix, both subject and object are language constructs with file location information
// predicate, subject, object: assert an object property (predicate) between subject and object.
#define MCObjectPropertyAssertionBetweenConstructs(p, s, o) onto_generator->ObjectPropertyAssertion(MAKE_IRI_W_PREFIX("c", p), current_file_prefix +":"+ (s), current_file_prefix +":"+ (o))

// Data property assertion: subject, data predicate, object
#define MDataPropertyAssertion(p, s, o) onto_generator->DataPropertyAssertion(MAKE_IRI_W_PREFIX("c", p), current_file_prefix +":"+ (s), (o))

// alternative macro to assert an object property (predicate) between subject and object. 
// Note Object is a C concept, not language construct
#define MHas(p, s, o) onto_generator->ObjectPropertyAssertion(MAKE_IRI_W_PREFIX("c", p), current_file_prefix +":"+ (s), MAKE_IRI_W_PREFIX("c", o))


// Old macro without file name in IRIs
//#define MClassAssertion(c, ind) onto_generator->ClassAssertion(MAKE_IRI_W_PREFIX("c", c), MAKE_IRI_W_PREFIX("", ind))
//#define MCObjectPropertyAssertionBetweenConstructs(p, s, o) onto_generator->ObjectPropertyAssertion(MAKE_IRI_W_PREFIX("c", p), MAKE_IRI_W_PREFIX("", s), MAKE_IRI_W_PREFIX("", o))
//#define MDataPropertyAssertion(p, s, o) onto_generator->DataPropertyAssertion(MAKE_IRI_W_PREFIX("c", p), MAKE_IRI_W_PREFIX("", s), (o))
//#define MHas(p, s, o) onto_generator->ObjectPropertyAssertion(MAKE_IRI_W_PREFIX("c", p), MAKE_IRI_W_PREFIX("", s), MAKE_IRI_W_PREFIX("c", o))

class visitorTraversal : public AstSimpleProcessing
{
protected:
    void visit (SgNode * n)
    {
        ON(BUILDTAG) buildKnowledgeBase(n);
    }
};

// a hack to use full file for the language construct IRI
static int current_file_id=0; 
static std::string current_file_prefix; 

//TODO: move to Rose namespace
static bool checkRoseVersionNumber(const std::string &need) 
{
  std::vector<std::string> needParts = rose::StringUtility::split('.',
      need);
  std::vector<std::string> haveParts = rose::StringUtility::split('.',
      ROSE_PACKAGE_VERSION);

  for (size_t i=0; i < needParts.size() && i < haveParts.size(); ++i) {
    if (needParts[i] != haveParts[i])
      return needParts[i] < haveParts[i];
  }
  // E.g., need = "1.2" and have = "1.2.x", or vice versa
  return true;
} 

int main(int argc, char ** argv)
{
    string output_file = "tmp.kb.txt", fn;

    // clock_gettime() nanosecond resolution
    struct timespec ts_now;
    long mtime_0, mtime_1, mtime_ft;

 //TODO:  use rose::checkVersionNumber()
    string ROSE_version_needed="0.9.8.50";
    if(! checkRoseVersionNumber (ROSE_version_needed))
    {
      cerr<<"Error: This translator needs ROSE v."<<ROSE_version_needed<<" to work."<<endl;
      cerr<<"Detected version is v."<<ROSE_PACKAGE_VERSION<<endl;
      exit(1);
    }

    /* Command line processing */
    vector <string> argvList (argv, argv + argc);

    if (CommandlineProcessing::isOption(argvList, "--help", "", false))
    {
        cout << "---------------------Tool-Specific Help-----------------------------------"<< endl;
        cout << "This is a source translator to build knowledge base from your C/C++ code." << endl;
        cout << "Usage: "<< argvList[0] << "[options]" << "[-o output]" << "-c input.c" << endl;
        cout << endl;
        cout << "Options:" << endl;
        cout << "\t--help              This help message." << endl;
        cout << "\t-emit-pl output.pl       ontologies in prolog file." << endl;
        cout << "\t-emit-owl output.owl     ontologies in OWL format." << endl;
        cout << "\t-emit-ttl output.ttl     ontologies in Turtle." << endl;
        cout << "\t-alive true/false        embedding Prolog engine." << endl;
        cout << "----------------------Generic Help for ROSE tools--------------------------" << endl;

        exit(1);
    }

    cout << endl;

    if ( CommandlineProcessing::isOptionWithParameter(argvList, "-emit-pl", "", output_file, true) ) {
        onto_generator = new OntoPl();
//        cout << " save knowledgebase in prolog predicate (obsolete) " << output_file << endl;
    }
    else if ( CommandlineProcessing::isOptionWithParameter(argvList, "-emit-owl", "", output_file, true) ) {
    	onto_generator = new OntoOwlFunctional();
//    	cout << " save knowledgebase in OWL/Functional Syntax " << output_file << endl;
    }
    else if ( CommandlineProcessing::isOptionWithParameter(argvList, "-emit-ttl", "", output_file, true) ) {
    	onto_generator = new OntoTurtle();
//    	cout << " save knowledgebase in Turtle " << output_file << endl;
    }
#ifdef SWIPL
    if ( CommandlineProcessing::isOptionWithParameter(argvList, "-alive", "", output_file, true) ) {
    	bool repl = false;
    	if (output_file == "true") {
    		repl = true;
    	}
    	onto_generator = new OntoEmPl(repl);
//    	cout << " online store with embedded prolog engine " << output_file << endl;
    }
#endif

    cout << endl;

    clock_gettime(CLOCK_MONOTONIC, &ts_now);
    mtime_0 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

    // Generate AST for input files ---------------------------------
    SgProject* project = frontend(argvList);

    clock_gettime(CLOCK_MONOTONIC, &ts_now);
    mtime_1 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

    mtime_ft = mtime_1 - mtime_0;

    SgFilePtrList file_ptr_list = project->get_fileList();
    
    
    // Phase I, general ontology
    string c_onto_url = "http://www.semanticweb.org/yzhao30/ontologies/2015/7/c";

//TODO: move common prefix registration into constructor of the generator class
    onto_generator->prefix_register("owl:", "http://www.w3.org/2002/07/owl#");
    onto_generator->prefix_register("rdf:", "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
//    onto_generator->prefix_register("xml:", "http://www.w3.org/XML/1998/namespace");
    onto_generator->prefix_register("xsd:", "http://www.w3.org/2001/XMLSchema#");
    onto_generator->prefix_register("rdfs:", "http://www.w3.org/2000/01/rdf-schema#");
    onto_generator->prefix_register("c:", c_onto_url + "#");

    onto_generator->import_register("file:///home/yzhao30/openk/ontology/c.owl"); // may not necessary for local file
    //onto_generator->import_register("http://www.semanticweb.org/yzhao30/ontologies/2015/7/c");

    
    // --timer anchor 0
    clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
    mtime_0 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

    // Phase II, build basic relations from source code
    visitorTraversal v;
    for (unsigned int i = 0; i < file_ptr_list.size(); i++)
    {
        SgFile * cur_file = file_ptr_list[i];
        SgSourceFile * s_file = isSgSourceFile(cur_file);
        if (s_file != NULL) {
            // input file name as default namespace
            fn = s_file->get_file_info()->get_filename();
            //fn = fn.substr(fn.find_last_of('/') + 1);

// TODO: These two lines do not support multiple files
// Not quite right, each file has an ontology name? No
            current_file_prefix = "file"+N2S(current_file_id); 
            // URI syntax: 
            // scheme:/path#fragment
            onto_generator->prefix_register( current_file_prefix +":", "file:"+fn + "#");
            current_file_id ++; // TODO ,better global file id generation
            onto_generator->ontoName_register( c_onto_url + "/" + fn);

            onto_generator->open_store(output_file);

            // visit AST
            v.traverseWithinFile(s_file, preorder);
        }
    }
    
    // --timer anchor 1
    clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
    mtime_1 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

    onto_generator->close_store();

    cout << "--------------------------------------------------" << endl;
    cout << "time#" << mtime_ft << "#" << mtime_1 - mtime_0 << endl;
    cout << "--------------------------------------------------" << endl;

    delete onto_generator;

    return 0;
}

namespace KG {

//TODO move declarations into a header
std::string generateConceptIRI(SgNode * n);

// generate short URI for a node, without path/file prefix info.
string generateShortIRI(SgNode * n)
{
    string IRI;
    SgLocatedNode * ln = isSgLocatedNode(n);
    if (ln) {
        Sg_File_Info * sc = n->get_startOfConstruct();
        Sg_File_Info * se = n->get_endOfConstruct();
        
        string  fn = sc->get_filename();
#if 0
        // TODO: why is this recursive call on parents?
        if (sc->isCompilerGenerated() || se->isCompilerGenerated()) {
            IRI = "_" + generateShortIRI(n->get_parent());
        }
        else 
#endif          
        {
            IRI = N2S(sc->get_line()) + "_" + N2S(sc->get_col()) + "_" + \
            N2S(se->get_line()) + "_" + N2S(se->get_col());
        }
    }
    else {
        IRI = "_";
    }
    return (IRI);
}


int buildKnowledgeBase(SgNode * n)
{

    /* First assert class of individual */
    string thisIRI = generateShortIRI(n);

    // Obtain node's type short IRI
    string thisClass = generateConceptIRI(n);

    // assert an individual of its type
    // TODO: store full IRI with file path and name
    MClassAssertion(thisClass, thisIRI);
    
    /* Create relations */
    if (isSgLocatedNode(n)) {

       // assert hasParent relation
        SgNode * p = n->get_parent();
        if (p) {
            MCObjectPropertyAssertionBetweenConstructs("hasParent", thisIRI, generateShortIRI(p));
        }

        SgStatement * stmt;
        if ( (stmt = isSgStatement(n)) ) {
            translateStatement(n, thisIRI);
        }

        SgExpression * expr;
        SgExpression * operand;
        if ( (expr = isSgExpression(n)) ) 
        {
            /* Operand for operator */
            // Binary operand relation
            if (SgBinaryOp * bo = isSgBinaryOp(n)) {
                operand = bo->get_lhs_operand();
                if (operand)
                    MCObjectPropertyAssertionBetweenConstructs("hasLeftOperand", thisIRI, generateShortIRI(operand));
                operand = bo->get_rhs_operand();
                if (operand)
                    MCObjectPropertyAssertionBetweenConstructs("hasRightOperand", thisIRI, generateShortIRI(operand));
            }
            // Unary operand relation
            else if (SgUnaryOp * uo = isSgUnaryOp(n)) {
                operand = uo->get_operand();
                if (operand)
                    MCObjectPropertyAssertionBetweenConstructs("hasOperand", thisIRI, generateShortIRI(operand));
            }

            translateExpression(n, thisIRI);

            SgType * ty = expr->get_type();
            if (ty) {
                string type_str = translateType(ty, thisIRI);
                MHas("hasType", thisIRI, type_str);
            }

            // Other relations (higher level, derived)
            if (expr->isLValue()) {
                MClassAssertion("Lvalue", thisIRI);
            }

            // does not include initializer
            if (expr->get_lvalue()) {
                MClassAssertion("isAssignedTo", thisIRI);
            }
        } // end if expression

        if (isSgLocatedNodeSupport(n)) {
            translateLocatedNodeSupport(n, thisIRI);
        }
    }
    else {
    //TODO: SgSourceFile is covered in the beginning, not really uncoverred.  
        MClassAssertion(("UncoverredClass"), (n->class_name()) );
    }

    return 0;
}        

// trivial, useless function
void exportKnowledgeBase()
{
    onto_generator->close_store();
}


int translateStatement(SgNode * n, string thisIRI)
{
    SgStatement * stmt = isSgStatement(n);
    if (stmt == NULL) {
        return 1;
    }

    switch(stmt->variantT())
    {
        // ## case V_SgStatement:
        // ======================
        case V_SgArithmeticIfStatement:
        case V_SgAssertStmt:
            break;
        case V_SgAssignStatement:
            break;    
        case V_SgBreakStmt:
        {
            // string do_label = isSgBreakStmt(stmt)->get_do_string_label();
            // if (!do_label.empty())
            //     MCObjectPropertyAssertionBetweenConstructs("hasDoStringLabel", thisIRI, do_label);
        } break;
        case V_SgCaseOptionStmt: // a single case key: body
        {
            if ( SgCaseOptionStmt * co = isSgCaseOptionStmt(stmt) ) {

                SgExpression * key = co->get_key();
                if (key) MCObjectPropertyAssertionBetweenConstructs("hasKey", thisIRI, generateShortIRI(key));
                SgStatement * body = co->get_body();
                MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(body));
                // get_key_range_end() ? get_case_construct_name() ? Fortran feature?
            }
        }
            break;
        case V_SgContinueStmt:
            break;

        // ### 
        case V_SgDeclarationStatement: 
            break;
        //case V_SgAsmStmt, SgAttributeSpecificationStatement, SgC_PreprocessorDirectiveStatement, 
            // SgClassDeclaration, SgClinkageDeclarationStatement, SgCommonBlock, SgContainsStatement, SgCtorInitializerList
        case V_SgEnumDeclaration:
            break;
        case V_SgEquivalenceStatement:
            break;
        case V_SgFunctionDeclaration:
        {
            SgFunctionDeclaration * fd = isSgFunctionDeclaration(stmt);
            if (fd == NULL) break;
            SgFunctionParameterList * pl = fd->get_parameterList();
            if (pl) MCObjectPropertyAssertionBetweenConstructs("hasParameterList", thisIRI, generateShortIRI(pl));
            // typedef Rose_STL_Container<SgInitializedName*> SgInitializedNamePtrList
            SgInitializedNamePtrList & init_vec = fd->get_args();
            for (size_t i = 0; i < init_vec.size(); ++i) {
                string para = generateShortIRI(init_vec[i]);
                MCObjectPropertyAssertionBetweenConstructs("hasParameter", thisIRI, para);
                MDataPropertyAssertion("NthPara", para, N2S(i));
            }

            SgFunctionDefinition * fdef = fd->get_definition();
            if (fdef) {
                MCObjectPropertyAssertionBetweenConstructs("hasDefinition", thisIRI, generateShortIRI(fdef));
            }
            SgScopeStatement * scp = fd->get_scope();
            if (scp) {
                MCObjectPropertyAssertionBetweenConstructs("hasScope", thisIRI, generateShortIRI(fdef)); // how is global scope represented?
            }
            SgFunctionType * ty = fd->get_type();
            if (ty) {
                string type_str = translateType(ty, thisIRI); 
                MHas("hasType", thisIRI, type_str);
            }

            //SgFunctionModifier &fm = fd->get_functionModifier(); 
            SgName nm = fd->get_name();
            if (!nm.is_null()) {
                MDataPropertyAssertion("hasName", thisIRI, nm.str() );
            }
        }
            break;
        case V_SgFunctionParameterList: 
            break;
        case V_SgTypedefDeclaration:
        {
            SgTypedefDeclaration * tdef = isSgTypedefDeclaration(stmt);

            SgDeclarationStatement * decl = tdef->get_declaration();
            if (decl) MCObjectPropertyAssertionBetweenConstructs("hasDefinition", thisIRI, generateShortIRI(decl));

            SgScopeStatement * scp = tdef->get_scope();
            if (scp) MCObjectPropertyAssertionBetweenConstructs("hasScope", thisIRI, generateShortIRI(scp));

            SgTypedefType * ty = tdef->get_type();
            if (ty) {
                string type_str = translateType(ty, thisIRI);
                MHas("hasType", thisIRI, type_str);
            }

            SgName nm = tdef->get_name();
            if (!nm.is_null()) {
                MDataPropertyAssertion("hasName", thisIRI, nm.str());
            }
        }
            break;
        case V_SgVariableDeclaration: 
            break;
        case V_SgVariableDefinition:
            break;
        // ### 

        case V_SgDefaultOptionStmt:
        {
            SgDefaultOptionStmt * defopt = isSgDefaultOptionStmt(stmt);
            if (defopt) {
                SgStatement * body = defopt->get_body();
                MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(body));
            }
        }
            break;
        case V_SgExprStatement:
        {
            SgExpression * expr = isSgExprStatement(stmt)->get_expression();
            if (expr) MCObjectPropertyAssertionBetweenConstructs("hasExpression", thisIRI, generateShortIRI(expr));
        }
            break;
        case V_SgForInitStatement:
        {
            SgStatementPtrList & s_vec = isSgForInitStatement(stmt)->get_init_stmt();
            if (!s_vec.empty()) {
                for (size_t i = 0; i < s_vec.size(); ++i) {
                    MCObjectPropertyAssertionBetweenConstructs("hasInitStatement", thisIRI, generateShortIRI(s_vec[i]) );
                }
            }
        }
            break;
        case V_SgFunctionTypeTable: // useful ?
            break;
        case V_SgGotoStatement:
        {
            SgGotoStatement * gs = isSgGotoStatement(stmt);

            SgLabelStatement * ls = gs->get_label();
            if (ls) MCObjectPropertyAssertionBetweenConstructs("hasTarget", thisIRI, generateShortIRI(ls));

            SgLabelRefExp * lr = gs->get_label_expression();
            if (lr) MCObjectPropertyAssertionBetweenConstructs("hasLabelRef", thisIRI, generateShortIRI(lr));
        }
            break;

        // ### 
        case V_SgIOStatement: // to-do
            break;
        // ###

        case V_SgLabelStatement:
        {
            SgLabelStatement * ls = isSgLabelStatement(stmt);
            SgStatement * stmt = ls->get_statement();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasStatement", thisIRI, generateShortIRI(stmt));

            SgName nm = ls->get_label();
            if (!nm.is_null()) MDataPropertyAssertion("hasName", thisIRI, nm.str());
        }
            break;
        case V_SgNullStatement:
            break;
        case V_SgReturnStmt:
            break;

        // ###SgScopeStatement
        case V_SgBasicBlock:
            break;
        case V_SgDoWhileStmt:
        {
            SgDoWhileStmt * dw = isSgDoWhileStmt(stmt);
            SgStatement * stmt = dw->get_body();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(stmt));
            stmt = dw->get_condition();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasCondition", thisIRI, generateShortIRI(stmt));
        }
            break;
        case V_SgForStatement:
        {
            SgForStatement * fs = isSgForStatement(stmt);

            // Init relation
            SgForInitStatement * init = fs->get_for_init_stmt();
            if (init) {
                MCObjectPropertyAssertionBetweenConstructs("hasForInit", thisIRI, generateShortIRI(init));
            }
            // Test relation
            // SgExpression * test = fs->get_test_expr();
            // if (test) {
            //     MCObjectPropertyAssertionBetweenConstructs("hasForTest", thisIRI, generateShortIRI(test));
            // }
            // get_test_expr() is deprecated, use get_test(), return the same thing
            SgStatement * tests = fs->get_test();
            if (tests)
                MCObjectPropertyAssertionBetweenConstructs("hasForTest", thisIRI, generateShortIRI(tests));
            // Incr relation
            SgExpression * incr = fs->get_increment();
            if (incr) {
                MCObjectPropertyAssertionBetweenConstructs("hasForIncr", thisIRI, generateShortIRI(incr));
            }
            // has body
            SgStatement * body = fs->get_loop_body();
            if (body) {
                MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(body));
            }

        } 
            break;
        case V_SgFunctionDefinition:
        {
            SgFunctionDefinition * fdef = isSgFunctionDefinition(stmt);

            SgFunctionDeclaration * fdecl = fdef->get_declaration();
            if (fdecl) MCObjectPropertyAssertionBetweenConstructs("hasDeclaration", thisIRI, generateShortIRI(fdecl));

            SgBasicBlock * bb = fdef->get_body();
            if (bb) MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(bb));

            // get_scope_number_list() ?
        }
            break;
        case V_SgGlobal:
            break;
        case V_SgIfStmt:
        {
            SgIfStmt * ifs = isSgIfStmt(stmt);

            SgStatement * stmt = ifs->get_conditional();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasCondition", thisIRI, generateShortIRI(stmt));

            stmt = ifs->get_true_body();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasTrueBody", thisIRI, generateShortIRI(stmt));

            stmt = ifs->get_false_body();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasFalseBody", thisIRI, generateShortIRI(stmt));
        }
            break;
        case V_SgSwitchStatement:
        {
            SgSwitchStatement * sw = isSgSwitchStatement(stmt);

            SgStatement * stmt = sw->get_item_selector();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasSelector", thisIRI, generateShortIRI(stmt));

            stmt = sw->get_body();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(stmt));
        }
            break;
        case V_SgWhileStmt:
        {
            SgWhileStmt * ws = isSgWhileStmt(stmt);

            SgStatement * stmt = ws->get_condition();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasCondition", thisIRI, generateShortIRI(stmt));

            stmt = ws->get_body();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasBody", thisIRI, generateShortIRI(stmt));
            // hasElseBody seems useless
            stmt = ws->get_else_body();
            if (stmt) MCObjectPropertyAssertionBetweenConstructs("hasElseBody", thisIRI, generateShortIRI(stmt));
        }
            break;
        case V_SgTemplateInstantiationFunctionDecl: // template function instantiation
        {
          SgTemplateInstantiationFunctionDecl * tifd = isSgTemplateInstantiationFunctionDecl(stmt);
          SgTemplateFunctionDeclaration* tfd = tifd->get_templateDeclaration();
          ROSE_ASSERT (tfd != NULL);
          MCObjectPropertyAssertionBetweenConstructs("instantiatedFrom", thisIRI, generateShortIRI(tfd));
          break;
        }
        case V_SgTemplateInstantiationDecl: // template class instantiation
        {
          SgTemplateInstantiationDecl * tifd = isSgTemplateInstantiationDecl(stmt);
          SgTemplateClassDeclaration* tfd = tifd->get_templateDeclaration();
          ROSE_ASSERT (tfd != NULL);
          MCObjectPropertyAssertionBetweenConstructs("instantiatedFrom", thisIRI, generateShortIRI(tfd));
          break;
        }
 
        // ###
        // ## End of SgStatement
        default:
            break;
    }

    return 0;
}


int translateExpression(SgNode * n, string thisIRI)
{
    SgExpression * expr = isSgExpression(n);
    if (expr == NULL) {
        return 1;
    }

    // if (thisIRI.empty())
    //     cout << "empty IRI in translateExpression" <<endl;

    switch(expr->variantT())
    {
        // ##SgExpression
        // ======================
        case V_SgExpression:
            break;
        case V_SgActualArgumentExpression:
            break;

        // ###SgBinaryOp
        case V_SgAddOp:
        case V_SgAndOp:
        case V_SgArrowExp:
        case V_SgArrowStarOp:    
        case V_SgAssignOp:
        case V_SgBitAndOp:
        case V_SgBitOrOp:
        case V_SgBitXorOp:
        case V_SgCommaOpExp:
        // ####SgCompoundAssignOp
        case V_SgAndAssignOp:
        case V_SgDivAssignOp:
        case V_SgExponentiationAssignOp:
        case V_SgIntegerDivideAssignOp:
        case V_SgIorAssignOp:
        case V_SgLshiftAssignOp:
        case V_SgPlusAssignOp: // +=
        case V_SgMinusAssignOp: // -=    
        case V_SgModAssignOp:
        case V_SgMultAssignOp:
        case V_SgRshiftAssignOp:
        case V_SgXorAssignOp:
            break;
        // ####
        case V_SgDivideOp:
        case V_SgDotExp:
        case V_SgDotStarOp: // C++
        case V_SgEqualityOp:
        case V_SgExponentiationOp:
        case V_SgGreaterOrEqualOp: // >=
        case V_SgGreaterThanOp: // >         
        case V_SgIntegerDivideOp:
        case V_SgLessOrEqualOp: // <=
        case V_SgLessThanOp: // <
            break;
        case V_SgLshiftOp:
        case V_SgMembershipOp: // . or ->
        case V_SgModOp:
        case V_SgMultiplyOp:
        case V_SgNonMembershipOp:
        case V_SgNotEqualOp:
        case V_SgOrOp:
        case V_SgPntrArrRefExp: //@ x[i] lhs x rhs i
            break;
        case V_SgPointerAssignOp: //@
        case V_SgRshiftOp:
        case V_SgScopeOp: // C++
            break;
        case V_SgSubtractOp:
            break;
        case V_SgUserDefinedBinaryOp:
            break;
        // ###   

        case V_SgCallExpression:
            break;
        case V_SgFunctionCallExp:
        {
            SgCallExpression * ce = isSgCallExpression(expr);
            SgExpression * fe = ce->get_function();
            if (fe) MCObjectPropertyAssertionBetweenConstructs("call", thisIRI, generateShortIRI(fe));

            SgExprListExp * alist = ce->get_args();
            if (alist) MCObjectPropertyAssertionBetweenConstructs("hasArgumentList", thisIRI, generateShortIRI(alist));

            // --> VarRefExp
            SgExpressionPtrList & expr_vec = alist->get_expressions();
            for (size_t i = 0; i < expr_vec.size(); ++i) {
                string arg = generateShortIRI(expr_vec[i]);
                MCObjectPropertyAssertionBetweenConstructs("hasArgumentExpr", thisIRI, arg);
                MDataPropertyAssertion("NthArg", arg, N2S(i));
            }
        }
            break;
        case V_SgColonShapeExp:
            break;
        case V_SgConditionalExp:
            break;
        case V_SgExprListExp:
            break; 
        case V_SgFunctionRefExp:
        {
            SgFunctionRefExp * fref = isSgFunctionRefExp(expr);
            SgFunctionSymbol * fsym = fref->get_symbol();
            if (fsym) {
                SgFunctionDeclaration * fdecl = fsym->get_declaration();
                MCObjectPropertyAssertionBetweenConstructs("referTo", thisIRI, generateShortIRI(fdecl) );
                MDataPropertyAssertion("hasName", thisIRI, (fsym->get_name().str()) );
            }
        }
            break;

        // ###
        case V_SgInitializer:
            break;
        case V_SgAggregateInitializer:
            break;
        case V_SgAssignInitializer:
        {
            if (SgAssignInitializer * initor = isSgAssignInitializer(expr)) {
                SgExpression * val_exp = initor->get_operand();
                MCObjectPropertyAssertionBetweenConstructs("hasInitialValue", thisIRI, generateShortIRI(val_exp));
            }
        }
            break;
        case V_SgCompoundInitializer:
            break;
        case V_SgDesignatedInitializer: // int a[6] = { [4] = 29, [2] = 15 };
            break;
        // ###

        case V_SgLabelRefExp:
            break; // to-do

        case V_SgNaryOp:
            break; // to-do

        case V_SgNullExpression:
        case V_SgRefExp: // This class represents the "&" operator (applied to any lvalue). ?
            break;
        case V_SgSizeOfOp:
            break;
        case V_SgStatementExpression:
            break;
        case V_SgStringConversion:
            break;
        case V_SgSubscriptExpression: //@
            break;

        // ###SgUnaryOp
        case V_SgAddressOfOp: //@
            break;
        case V_SgBitComplementOp:
            break;
        case V_SgCastExp:
            break;
        case V_SgConjugateOp:
            break;
        case V_SgExpressionRoot:
            break;
        case V_SgMinusMinusOp: // --
            break;
        case V_SgMinusOp:
            break;
        case V_SgNotOp:
            break;
        case V_SgPlusPlusOp: // ++
            break;
        case V_SgPointerDerefExp: //!
            break;
        case V_SgUnaryAddOp:
            break; 
        // ###

        // ###SgValueExp
        case V_SgBoolValExp:
        {
            int v = isSgBoolValExp(expr)->get_value();
            MDataPropertyAssertion("hasValue", thisIRI, N2S(v));
        } 
            break;
        case V_SgCharVal:
        {
            string c = isSgCharVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, c);
        } 
            break;
        case V_SgDoubleVal:
        {
            string v = isSgDoubleVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        // case V_SgEnumVal:
        // {
        //     SgEnumVal * ev = isSgEnumVal(expr);
        //     SgEnumDeclaration * edecl = ev->get_declaration();
        //     if (edecl) MCObjectPropertyAssertionBetweenConstructs("declaredBy", thisIRI, generateShortIRI(edecl));
        //     SgName nm = ev->get_name();
        //     if (!nm.is_null()) MDataPropertyAssertion("hasName", thisIRI, nm.str());
        //     int v = ev->get_value();
        //     MDataPropertyAssertion("hasValue", thisIRI, N2S(v));
        // }
        //     break;
        case V_SgFloatVal:
        {
            string v = isSgFloatVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgIntVal: // Note IntVal includes CharVal like 'C', 'S', etc
        {
            // string v = isSgIntVal(expr)->get_valueString();
            int v = isSgIntVal(expr)->get_value();
            MDataPropertyAssertion("hasValue", thisIRI, N2S(v));
        }
            break;
        case V_SgLongDoubleVal:
        {
            string v = isSgLongDoubleVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgLongIntVal:
        {
            string v = isSgLongIntVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgLongLongIntVal:
        {
            string v = isSgLongLongIntVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgNullptrValExp:
            break;
        case V_SgShortVal:
        {
            string v = isSgShortVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        // case V_SgStringVal:
        // {
        //     string v = isSgStringVal(expr)->get_value();
        //     MDataPropertyAssertion("hasValue", thisIRI, v);
        // }
        //     break;
        case V_SgUnsignedIntVal:
        {
            string v = isSgUnsignedIntVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgUnsignedLongLongIntVal:
        {
            string v = isSgUnsignedLongLongIntVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgUnsignedLongVal:
        {
            string v = isSgUnsignedLongVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        case V_SgUnsignedShortVal:
        {
            string v = isSgUnsignedShortVal(expr)->get_valueString();
            MDataPropertyAssertion("hasValue", thisIRI, v);
        }
            break;
        // case V_SgWcharVal:
        // {
        //     string v = isSgWcharVal(expr)->get_valueString();
        //     MDataPropertyAssertion("hasValue", thisIRI, v);
        // }
        //     break;
        // ###

        case V_SgVarArgCopyOp: // ?
        case V_SgVarArgEndOp:
        case V_SgVarArgOp:
        case V_SgVarArgStartOneOperandOp:
        case V_SgVariantExpression:
            break;
        case V_SgVarRefExp:
        {
            SgVarRefExp * vref = isSgVarRefExp(expr);
            // if (vref->isLValue()) {
            //     MClassAssertion("Lvalue", thisIRI);
            // }
            SgVariableSymbol * sym = vref->get_symbol();
            if (sym) {
                if (SgInitializedName * init = sym->get_declaration()) {
                    MCObjectPropertyAssertionBetweenConstructs("referTo", thisIRI, generateShortIRI(init));
                }
                MDataPropertyAssertion("hasName", thisIRI, (sym->get_name().str()) );
            }
         }   
            break;
         // ## End of Expression
        default:
            break;
    }

    return 0;
}


int translateLocatedNodeSupport(SgNode * n, string thisIRI)
{
    // ##SgLocatedNodeSupport
    // http://www.rosecompiler.org/ROSE_HTML_Reference/classSgInitializedName.html
    if (!isSgLocatedNodeSupport(n)) {
        return 1;
    }

    switch(n->variantT())
    {
        case V_SgInitializedName:
        {
            SgInitializedName * initname = isSgInitializedName(n);
            if (initname == NULL) break;

            SgDeclarationStatement * decl = initname->get_declaration();
            if (decl) {
                MCObjectPropertyAssertionBetweenConstructs("declaredBy", thisIRI, generateShortIRI(decl));
            }
            decl = initname->get_definition();
            if (decl) {
                MCObjectPropertyAssertionBetweenConstructs("definedBy", thisIRI, generateShortIRI(decl));
            }
            SgInitializer * initor = initname->get_initializer();
            if (initor) {
                MCObjectPropertyAssertionBetweenConstructs("hasInitializer", thisIRI, generateShortIRI(initor));
            }
            SgScopeStatement * scp = initname->get_scope();
            if (scp) MCObjectPropertyAssertionBetweenConstructs("hasScope", thisIRI, generateShortIRI(scp));

            SgType * ty = initname->get_type();
            if (ty) {
                string type_str = translateType(ty, thisIRI); 
                MHas("hasType", thisIRI, type_str);
            }
        
            SgStorageModifier &st = initname->get_storageModifier();
            if (!st.isUnknown()) {
            	string mod_str = translateModifier(&st);
                MHas("hasStorageClass", thisIRI, mod_str);
            }

            SgSymbol * sym = initname->search_for_symbol_from_symbol_table();
            if (sym) MDataPropertyAssertion("hasName", thisIRI, (sym->get_name().str()) );
        }
        break;
        // #
        default:
            break;
    }

    return 0;
}

// Convert an ROSE AST type into generic C ontology type concepts
string translateType(SgType * t, string thisIRI, bool returnOnly)
{
    if (!isSgType(t)) {
        return ("invalid_type");
    }

    string type_str = "unknown_type", tmp_str;

    SgType * hidden_ty = t->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE);

    switch (t->variantT()) {
        case V_SgArrayType:
        {
            type_str = "array_type";
            if (returnOnly) break;

            SgArrayType * arr_ty = isSgArrayType(t);            
            tmp_str = translateType(arr_ty->get_base_type(), "", true);
            MHas("hasArrayBaseType", thisIRI, tmp_str);
            int rank = arr_ty->get_rank();
            MDataPropertyAssertion("hasArrayRank", thisIRI, N2S(rank));
        } 
            break;
        case V_SgDeclType:
            break;
        case V_SgFunctionType:
        {
            type_str = "function_type";
            if (returnOnly) break;

            SgFunctionType * fty = isSgFunctionType(t);
            tmp_str = translateType(fty->get_return_type(), "", true);
            MHas("hasReturnType", thisIRI, tmp_str);
            // get_arguments() or get_argument_list()->get_arguments() 
        }
            break;

        case V_SgModifierType:
            break;
        // ### 
        case V_SgNamedType:
            break;
        case V_SgEnumType:
        {
            type_str = "enum_type";
        }
            break;
        case V_SgTypedefType:
        {
            // type_str = "TypedefType";
            // if (returnOnly) break;

            // tmp_str = translateType(hidden_ty, "", true);
            // MHas("hasHiddenType", thisIRI, tmp_str);
            type_str = translateType(hidden_ty, "", true);
        }
            break;
        // ###
        case V_SgPointerType:
        {
            type_str = "pointer_type";
            if (returnOnly) break;

            tmp_str = translateType(t->findBaseType(), "", true);
            MHas("hasBaseType", thisIRI, tmp_str);
        }
            break;
        case V_SgPointerMemberType:
            break;
        case V_SgQualifiedNameType:
            break;
        case V_SgReferenceType: // C++
        {
            type_str = "ref_type";
            if (returnOnly) break;

            tmp_str = translateType(t->findBaseType(), "", true);
            MHas("hasBaseType", thisIRI, tmp_str);
        }
            break;
        case V_SgRvalueReferenceType: // C++
            break;
        case V_SgTypeBool:
            type_str = "bool";
            break;
        case V_SgTypeChar:
            type_str = "char";
            break;
        case V_SgTypeComplex:
            type_str = "complex";
            break;
        case V_SgTypeDefault:
            break;
        case V_SgTypeDouble:
            type_str = "double";
            break;
        case V_SgTypeEllipse:
            break;
        case V_SgTypeFloat:
            type_str = "float";
            break;
        case V_SgTypeGlobalVoid:
            type_str = "void_type";
            break;
        case V_SgTypeImaginary:
            break;
        case V_SgTypeInt:
            type_str = "int";
            break;
        case V_SgTypeLabel:
            break;
        case V_SgTypeLong:
            type_str = "long";
            break;
        case V_SgTypeLongDouble:
            type_str = "long_double";
            break;
        case V_SgTypeLongLong:
            type_str = "long_long";
            break;
        case V_SgTypeNullptr:
            type_str = "null_pointer";
            break;
        case V_SgTypeShort:
            type_str = "short";
            break;
        case V_SgTypeSignedChar:
            type_str = "signed_char";
            break;
        case V_SgTypeSignedInt:
            type_str = "signed_int";
            break;
        case V_SgTypeSignedLong:
            type_str = "signed_long";
            break;
        case V_SgTypeSignedLongLong:
            type_str = "signed_long_long";
            break;
        case V_SgTypeSignedShort:
            type_str = "signed_short";
            break;
        case V_SgTypeString:
            type_str = "array_type";
            break;
        case V_SgTypeUnknown:
            break;
        case V_SgTypeUnsignedChar:
            type_str = "unsigned_char";
            break;
        case V_SgTypeUnsignedInt:
            type_str = "unsigned_int";
            break;
        case V_SgTypeUnsignedLong:
            type_str = "unsigned_long";
            break;
        case V_SgTypeUnsignedLongLong:
            type_str = "unsigned_long_long";
            break;
        case V_SgTypeUnsignedShort:
            type_str = "unsigned_short";
            break;
        case V_SgTypeVoid:
            type_str = "void_type";
            break;
        case V_SgTypeWchar:
            type_str = "wchar";
            break;
        default:
        	SgNamedType *snt = isSgNamedType(t);
            if (snt) {
            	type_str = snt->get_name().str();
            }
            break;
    }

    return type_str;
}


// input: SgNode
// output: type of the node or C concept name (no prefix)
std::string generateConceptIRI(SgNode * n)
{
  string class_str = "UnknownClass";

  switch (n->variantT()) 
  {
    case V_SgLocatedNodeSupport:
    case V_SgInitializedName:
      class_str = "Variable";
      break;
    case V_SgHeaderFileBody:
      break;
    case V_SgStatement:
    case V_SgScopeStatement:
      class_str = "Statement";
      break;
    case V_SgGlobal:
      class_str = "Global";
      break;
    case V_SgBasicBlock:
      class_str = "Block";
      break;
    case V_SgIfStmt:
      class_str = "IfElseStatement";
      break;
    case V_SgForStatement:
      class_str = "ForStatement";
      break;
    case V_SgFunctionDefinition:
      class_str = "FunctionDefinition";
      break;
    case V_SgWhileStmt:
      class_str = "WhileStatement";
      break;
    case V_SgDoWhileStmt:
      class_str = "DoWhileStatement";
      break;
    case V_SgSwitchStatement:
      class_str = "SwitchStatement";
      break;
    case V_SgFunctionTypeTable:
      break;
    case V_SgDeclarationStatement:
      class_str = "Declaration";
      break;
    case V_SgFunctionParameterList:
      class_str = "FunctionParameterList";
      break;
    case V_SgVariableDeclaration:
      class_str = "VariableDecl";
      break;
    case V_SgVariableDefinition:
      class_str = "VariableDefinition";
      break;
    case V_SgClinkageDeclarationStatement:
      break;
    case V_SgClinkageStartStatement:
      break;
    case V_SgClinkageEndStatement:
      break;
    case V_SgEnumDeclaration:
      class_str = "EnumDecl";
      break;
    case V_SgAsmStmt:
      break;
    case V_SgTypedefDeclaration:
      class_str = "TypedefDecl";
      break;
    case V_SgPragmaDeclaration:
      break;
    case V_SgDerivedTypeStatement:
      break;
    case V_SgFunctionDeclaration:
      class_str = "FunctionDecl";
      break;

    case V_SgStmtDeclarationStatement: //?
      break;
    case V_SgExprStatement:
      class_str = "ExpressionStatement";
      break;
    case V_SgLabelStatement:
      class_str = "LabeledStatement";
      break;
    case V_SgCaseOptionStmt:
      class_str = "CaseOptionStatement"; 
      break;
    case V_SgDefaultOptionStmt:
      class_str = "DefaultOptionStatement";
      break;
    case V_SgBreakStmt:
      class_str = "BreakStatement";
      break;
    case V_SgContinueStmt:
      class_str = "ContinueStatement";
      break;
    case V_SgReturnStmt:
      class_str = "ReturnStatement";
      break;
    case V_SgGotoStatement:
      class_str = "GotoStatement";
      break;
    case V_SgNullStatement:
      class_str = "NullStatement";
      break;
    case V_SgVariantStatement: //?
      break;
    case V_SgForInitStatement:
      class_str = "ForInitStatement";
      break;
    case V_SgIOStatement:
      class_str = "IOStatement";
      break;
    case V_SgAssignStatement:
      class_str = "ExpressionStatement";
      break;
    case V_SgExpression:
      class_str = "Expression";
      break;
    case V_SgUnaryOp:
      class_str = "UnaryOp";
      break;
    case V_SgExpressionRoot:
      break;
    case V_SgMinusOp:
      class_str = "PrefixMinusOp";
      break;
    case V_SgUnaryAddOp:
      class_str = "PrefixPlusOp";
      break;
    case V_SgNotOp:
      class_str = "LogicalNegOp";
      break;
    case V_SgPointerDerefExp:
      class_str = "IndirectionOp";
      break;
    case V_SgAddressOfOp:
      class_str = "AddressOp";
      break;
    case V_SgMinusMinusOp:
      class_str = "PostfixDecrementOp";
      break;
    case V_SgPlusPlusOp:
      class_str = "PostfixIncrementOp";
      break;
    case V_SgBitComplementOp:
      class_str = "OnesComplementOp";
      break;
    case V_SgCastExp:
      class_str = "CastOp";
      break;
    case V_SgRealPartOp:
    case V_SgImagPartOp:
    case V_SgConjugateOp:
      break;
    case V_SgUserDefinedUnaryOp:
      break;
    case V_SgBinaryOp:
      class_str = "BinaryOp";
      break;
    case V_SgArrowExp:
      class_str = "MemberRef";
      break;
    case V_SgDotExp:
      class_str = "MemberRef";
      break;
    case V_SgDotStarOp:
    case V_SgArrowStarOp: // C++
      break;
    case V_SgEqualityOp:
      class_str = "EqualOp";
      break;
    case V_SgLessThanOp:
      class_str = "LessOp";
      break;
    case V_SgGreaterThanOp:
      class_str = "GreaterOp";
      break;
    case V_SgNotEqualOp:
      class_str = "NeqOp";
      break;
    case V_SgLessOrEqualOp:
      class_str = "LeqOp";
      break;
    case V_SgGreaterOrEqualOp:
      class_str = "GeqOp";
      break;
    case V_SgAddOp:
      class_str = "AddOp";
      break;
    case V_SgSubtractOp:
      class_str = "SubOp";
      break;
    case V_SgMultiplyOp:
      class_str = "MulOp";
      break;
    case V_SgDivideOp:
      class_str = "DivOp";
      break;
    case V_SgIntegerDivideOp:
      class_str = "DivOp";
      break;
    case V_SgModOp:
      class_str = "ModOp";
      break;
    case V_SgAndOp:
      class_str = "LogicalAndOp";
      break;
    case V_SgOrOp:
      class_str = "LogicalOrOp";
      break;
    case V_SgBitXorOp:
      class_str = "XorOp";
      break;
    case V_SgBitAndOp:
      class_str = "AndOp";
      break;
    case V_SgBitOrOp:
      class_str = "OrOp";
      break;
    case V_SgCommaOpExp:
      class_str = "CommaOp";
      break;
    case V_SgLshiftOp:
      class_str = "ShlOp";
      break;
    case V_SgRshiftOp:
      class_str = "ShrOp";
      break;
    case V_SgPntrArrRefExp:
      class_str = "ArraySub";
      break;
    case V_SgScopeOp:
      break;
    case V_SgAssignOp:
      class_str = "AssignOp";
      break;
    case V_SgPointerAssignOp:
      break;
    case V_SgUserDefinedBinaryOp:
      break;
    case V_SgCompoundAssignOp:
      break;
    case V_SgPlusAssignOp:
      class_str = "AddAssignOp";
      break;
    case V_SgMinusAssignOp:
      class_str = "SubAssignOp";
      break;
    case V_SgAndAssignOp:
      class_str = "AndAssignOp";
      break;
    case V_SgIorAssignOp:
      class_str = "OrAssignOp";
      break;
    case V_SgMultAssignOp:
      class_str = "MulAssignOp";
      break;
    case V_SgDivAssignOp:
      class_str = "DivAssignOp";
      break;
    case V_SgModAssignOp:
      class_str = "ModAssignOp";
      break;
    case V_SgXorAssignOp:
      class_str = "XorAssignOp";
      break;
    case V_SgLshiftAssignOp:
      class_str = "ShlAssignOp";
      break;
    case V_SgRshiftAssignOp:
      class_str = "ShrAssignOp";
      break;
    case V_SgIntegerDivideAssignOp:
      break;
    case V_SgMembershipOp:
      class_str = "";
      break;
    case V_SgExprListExp:
      class_str = "ExpressionList";
      break;
    case V_SgListExp:
      class_str = "ListExpr"; //?
      break;

    case V_SgVarRefExp:
      class_str = "VariableRef";
      break;
    case V_SgFunctionRefExp:
      class_str = "FunctionRef";
      break;
    case V_SgValueExp:
    case V_SgBoolValExp:
    case V_SgStringVal:
    case V_SgShortVal:
    case V_SgCharVal:
    case V_SgUnsignedCharVal:
    case V_SgWcharVal:
    case V_SgUnsignedShortVal:
    case V_SgIntVal:
    case V_SgEnumVal:
    case V_SgUnsignedIntVal:
    case V_SgLongIntVal:
    case V_SgLongLongIntVal:
    case V_SgUnsignedLongLongIntVal:
    case V_SgUnsignedLongVal:
    case V_SgFloatVal:
    case V_SgDoubleVal:
    case V_SgLongDoubleVal:
    case V_SgComplexVal:
      class_str = "Value"; //TODO: need sub concepts of values? based on types?
      break;
    case V_SgCallExpression:
      break;
    case V_SgFunctionCallExp:
      class_str = "FunctionCall";
      break;
    case V_SgSizeOfOp:
      class_str = "SizeofOp";
      break;

    case V_SgConditionalExp:
      class_str = "ConditionalOp";
      break;
    case V_SgRefExp:
      break;
    case V_SgInitializer:
    case V_SgAggregateInitializer:
    case V_SgCompoundInitializer:
      class_str = "Initializer";
      break;
    case V_SgAssignInitializer:
      class_str = "AssignInitializer";
      break;
    case V_SgDesignatedInitializer:
      class_str = "AssignInitializer";
      break;
    case V_SgVarArgStartOp:
    case V_SgVarArgOp:
    case V_SgVarArgEndOp:
    case V_SgVarArgCopyOp:
    case V_SgVarArgStartOneOperandOp:
      break;
    case V_SgNullExpression:
      class_str = "NullExpr";
      break;
    case V_SgVariantExpression: // ?
      break;
    case V_SgSubscriptExpression:
      class_str = "ArraySub";
      break;
    case V_SgColonShapeExp: // ?
      break;
    case V_SgAsteriskShapeExp:
      break;
    case V_SgStatementExpression: // ?
      break;
    case V_SgAsmOp:
      break;
    case V_SgLabelRefExp:
      break;
    case V_SgActualArgumentExpression: // ?
      break;
    case V_SgUnknownArrayOrFunctionReference:
      break;
    case V_SgNaryOp:
      break;
    case V_SgNaryBooleanOp:
      break;
    case V_SgNaryComparisonOp:
      break;
    case V_SgStringConversion:
      break;
    // Liao, 10/20/2015, add support for C++ templates
    case V_SgTemplateFunctionDeclaration:
      class_str = "TemplateFunctionDeclaration";
      break;
    case V_SgTemplateClassDeclaration:
      class_str = "TemplateClassDeclaration";
      break;
    case V_SgTemplateInstantiationFunctionDecl:
      class_str = "TemplateFunctionInstantiationDeclaration";
      break;
    case V_SgTemplateInstantiationDecl:
      class_str = "TemplateClassInstantiationDeclaration";
      break;
    default:
      class_str = n->class_name();
      break;
  }

  return class_str;
}


string translateModifier(SgModifier * n)
{
    string modifier = "UnknownModifier";
    SgStorageModifier * stm;
    if ((stm = isSgStorageModifier(n))) {
        SgStorageModifier::storage_modifier_enum ste = stm->get_modifier();
        switch (ste) {
            case SgStorageModifier::e_default:
                modifier = "default";
                break;
            case SgStorageModifier::e_extern:
                modifier = "extern";
                break;
            case SgStorageModifier::e_static:
                modifier = "static";
                break;
            case SgStorageModifier::e_auto:
                modifier = "auto";
                break;
            case SgStorageModifier::e_unspecified:
                modifier = "unspecified";
                break;
            case SgStorageModifier::e_register:
                modifier = "register";
                break;
            case SgStorageModifier::e_typedef:
                modifier = "typedef";
                break;
            default:
                break;
        }
    }
    
    return modifier;
}


// not useful
bool sideEffectAnalysis(SgStatement * n, string thisIRI)
{
    std::set<SgInitializedName *> readVars;
    std::set<SgInitializedName *> writeVars;
    bool flag = collectReadWriteVariables(n, readVars, writeVars);
    if (flag) {
        for (std::set<SgInitializedName *>::iterator wi = writeVars.begin(), we = writeVars.end(); wi != we; ++wi) {
            MCObjectPropertyAssertionBetweenConstructs("write", thisIRI, generateShortIRI((*wi)) );
        }

    } else {
        cout << "side effect analysis error for " << generateShortIRI(n) << endl;
    }
    return flag;
}

} // END OF NAMESPACE

