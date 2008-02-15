#include "QRUnparser.h"

using namespace std;

QRAstCoords::QRAstCoords() {
    m_main_box = NULL;
}
    

void QRAstCoords::push_coord_box(SgNode *node) {
	
	
    QRCoordBox *box = new QRCoordBox();
    box->setNode(node);
    if (!m_box_stack.empty()) {
	m_box_stack.top()->insertBox(box);
    } else {
	m_main_box = box;
    }
    
    m_box_stack.push(box);  
    if (!node_has_multi_coord(node)) {
	TypeNodeCoords::iterator iter = m_node_coords.find(node);
        if (iter == m_node_coords.end()) {
            m_node_coords[node] = box;        
	} else {
	    m_node_coords.erase(iter);
	    m_nodes_multi_coord.insert(node);
	}
    }
}

void QRAstCoords::pop_coord_box(int line0, int col0, int line1, int col1) {
    QRCoordBox *box = m_box_stack.top(); m_box_stack.pop();
    box->setCoord(line0, col0, line1, col1);
}

TypeNodeCoords* QRAstCoords::node_coords() { return &m_node_coords; }

vector<QRCoordBox *> *QRAstCoords::get_target_boxes(int line, int col) {
   m_target_boxes.clear();
   QRCoordBox *box = m_main_box;   
   get_target_boxes_aux(box, line, col);
   return &m_target_boxes;   
}


void QRAstCoords::get_target_boxes_aux(QRCoordBox *box, int line, int col) {
   m_target_boxes.push_back(box);
   list<QRCoordBox *> *enclosed_boxes = box->children();
   if (!enclosed_boxes) return;
   bool found_target_box = false;
   list<QRCoordBox *>::iterator iter = enclosed_boxes->begin();
   while (!found_target_box && iter != enclosed_boxes->end()) {
       QRCoordBox *new_box = *iter;
       if (new_box->isTarget(line, col)) {
	   get_target_boxes_aux(new_box, line, col);
	   found_target_box = true;
       } else {
	   iter++;
       }
   }
}


bool QRAstCoords::node_has_multi_coord(SgNode *node) {
    set<SgNode *>::iterator iter = m_nodes_multi_coord.find(node);
    return iter != m_nodes_multi_coord.end();
}
            
QRUnparser::QRUnparser( QRAstCoords *ast_coords, ostream* localStream, char* filename, Unparser_Opt info, int lineNumberToUnparse,
	UnparseFormatHelp *h, UnparseDelegate* repl):
	Unparser(localStream, filename, info, lineNumberToUnparse, h, repl)
{
        ROSE_ASSERT(ast_coords);
	m_ast_coords = ast_coords;
}

void QRUnparser::unparseStatement (SgStatement* stmt, SgUnparse_Info& info) {
   int line0, col0, line1, col1;
   m_ast_coords->push_coord_box(stmt);
   UnparseFormat& unparse_format = get_output_stream();
   line0 = unparse_format.current_line()-1; col0 = unparse_format.current_col();
   Unparser::unparseStatement(stmt, info);
   line1 = unparse_format.current_line()-1; col1 = unparse_format.current_col();
   m_ast_coords->pop_coord_box(line0, col0, line1, col1);
}

void QRUnparser::unparseExpression(SgExpression* expr, SgUnparse_Info& info) {
  int line0, col0, line1, col1;
   UnparseFormat& unparse_format = get_output_stream();
   m_ast_coords->push_coord_box(expr);
   line0 = unparse_format.current_line()-1; col0 = unparse_format.current_col();
   Unparser::unparseExpression(expr, info);
   line1 = unparse_format.current_line()-1; col1 = unparse_format.current_col();
   m_ast_coords->pop_coord_box(line0, col0, line1, col1);   
}
    
void QRUnparser::unparseSymbol(SgSymbol* sym, SgUnparse_Info& info)  {
   int line0, col0, line1, col1;
   UnparseFormat& unparse_format = get_output_stream();
   m_ast_coords->push_coord_box(sym);
   line0 = unparse_format.current_line()-1; col0 = unparse_format.current_col();
   Unparser::unparseSymbol(sym, info);
   line1 = unparse_format.current_line()-1; col1 = unparse_format.current_col();
   m_ast_coords->pop_coord_box(line0, col0, line1, col1);      
}


void QRUnparser::unparseType(SgType* type, SgUnparse_Info& info) {
  int line0, col0, line1, col1;
  UnparseFormat& unparse_format = get_output_stream();
  m_ast_coords->push_coord_box(type);
  line0 = unparse_format.current_line()-1; col0 = unparse_format.current_col();
  Unparser::unparseType(type, info);
  line1 = unparse_format.current_line()-1; col1 = unparse_format.current_col();
  m_ast_coords->pop_coord_box(line0, col0, line1, col1);
}


string QRUnparseToString (SgFile* astFile, QRAstCoords *ast_coords)
{
  // This global function permits any SgNode (including it's subtree) to be turned into a string

     string returnString;

     SgUnparse_Info *inheritedAttributeInfoPointer = new SgUnparse_Info();
     inheritedAttributeInfoPointer->unset_SkipComments();    // generate comments
     inheritedAttributeInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code


  // all options are now defined to be false. When these options can be passed in
  // from the prompt, these options will be set accordingly.
     bool _auto                         = FALSE;
     bool linefile                      = FALSE;
     bool useOverloadedOperators        = FALSE;
     bool num                           = FALSE;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = TRUE;

     bool caststring                    = FALSE;
     bool _debug                        = FALSE;
     bool _class                        = FALSE;
     bool _forced_transformation_format = FALSE;
     bool _unparse_includes             = FALSE;

  // printf ("In globalUnparseToString(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

     Unparser_Opt roseOptions( _auto,
                               linefile,
                               useOverloadedOperators,
                               num,
                               _this,
                               caststring,
                               _debug,
                               _class,
                               _forced_transformation_format,
                               _unparse_includes );

     int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

     // Initialize the Unparser using a special string stream inplace of the usual file stream
     ostringstream outputString;

     ROSE_ASSERT(isSgFile(astFile));
     SgGlobal *astGlobal = astFile->get_root();
     ROSE_ASSERT(isSgGlobal(astGlobal));

     char* fileNameOfStatementsToUnparse = NULL;
     fileNameOfStatementsToUnparse = astGlobal->getFileName();

     Unparser *unparser;

     if (ast_coords)
        unparser = new QRUnparser(ast_coords, &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );
     else
        unparser = new Unparser(&outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );


  // Information that is passed down through the tree (inherited attribute)
  // Use the input SgUnparse_Info object if it is available.
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

  // DQ (1/12/2003): Only now try to trap use of SgUnparse_Info default constructor
  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
    SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

    unparser->unparseStatement (astGlobal, inheritedAttributeInfo);

    // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
    SgUnparse_Info::forceDefaultConstructorToTriggerError = false;

   // MS: following is the rewritten code of the above outcommented
   //     code to support ostringstream instead of ostrstream.
    returnString = outputString.str();


    // delete the allocated SgUnparse_Info object
    delete inheritedAttributeInfoPointer;

    // delete unparser
    delete unparser;

    return returnString;
}
