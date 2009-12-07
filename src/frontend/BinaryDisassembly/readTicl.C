#include "readTicl.h"
#include <string.h>




using namespace ticl;

// DQ (12/1/2009): Fixed Use of "// std::out ..." in define (below) with "\" line continuation (reported by MSVC).
#define FILL_IN_STRUCT(STRUCT_NAME,FORMAT, FILENAME)			\
  string line;								\
  directory+="/";							\
  std::string filename;							\
  filename =directory+string( FILENAME );				\
  ifstream myfile;							\
  /*vector<STRUCT_NAME> vec_##STRUCT_NAME;*/ /*= new vector<STRUCT_NAME>();*/ \
  /* std::cout << "Opening file:" << filename << std::endl;	*/	\
  myfile.open(filename.c_str());					\
									\
  if (myfile.is_open())							\
    {									\
      /*vec_##STRUCT_NAME = new vector<STRUCT_NAME>();*/		\
      while (! myfile.eof() )						\
	{								\
	  getline (myfile,line);					\
									\
	  if(line != "")						\
	    {								\
	      vec_STRUCT.push_back( STRUCT_NAME () );			\
	      STRUCT_NAME* elem_##STRUCT_NAME = &vec_STRUCT.back();	\
	      /* STRUCT_NAME* elem_##STRUCT_NAME = new STRUCT_NAME();*/ \
	      /*std::cout << "line:" << line << std::endl; */		\
	      read_file(line, FORMAT, ARGS_##STRUCT_NAME);		\
	    }								\
	}								\
									\
      myfile.close();							\
    } else cout << "Unable to open file " + filename;			\
  /*return vec_STRUCT*/


using namespace std;

//Read words from file into string*'s provided in variable length
//argument list
void read_file (std::string line, const char *format, ...) {
  va_list ap; /* declare an argument pointer to a variable arg list */
  va_start(ap, format); /* initialize arg pointer using last known arg */

  const char *p = format;
  std::string* temp_string;

  char* s;
  string str;
  const char* delimiters =	"\t\n";
  const char* quote_delimiters =	"\"";

  s = strtok((char*)line.c_str(), delimiters);

  //	" \t;()\"<>:{}[]+-=&*#.,/\\~";
  while (*p) {
    //Get next word in line

#if 0
    if( s== NULL)
      {
	std::cerr << "Format error on line: " << line << std::endl;
	std::cerr << "Exprected:" << format << std::endl;

	assert(s!=NULL);
      }
#endif
    if(s != NULL)
      str = string(s);
    else
      str = "";

    //Assign argument string to word
    switch(*p) {
    case 's':
      {
	temp_string = va_arg(ap, string*);
	(*temp_string) = str;
	//	std::cout << "temp_string: " << *temp_string << std::endl;
	++p;
	break;
      }
    default:
      ++p;
      break;
    } // switch


    s = strtok(0, delimiters);


  } // while


  va_end(ap); /* restore any special stack manipulations */

} // maximum

void ticl::get_modules(std::string directory, vector<modules>& vec_STRUCT)
{
#define ARGS_modules  &elem_modules->id, &elem_modules->name, &elem_modules->md5, &elem_modules->sha1, &elem_modules->comment, &elem_modules->import_time
  FILL_IN_STRUCT(modules, "ssssss", "modules.tsv");
};

void ticl::get_functions_1(std::string directory, vector<functions_1>& vec_STRUCT)
{
#define ARGS_functions_1  &elem_functions_1->address, &elem_functions_1->name, &elem_functions_1->function_type, &elem_functions_1->name_md5
  FILL_IN_STRUCT(functions_1, "ssss", "functions_1.tsv");
};

void ticl::get_basic_blocks_1(std::string directory, vector<basic_blocks_1>& vec_STRUCT)
{
#define ARGS_basic_blocks_1  &elem_basic_blocks_1->id, &elem_basic_blocks_1->parent_function, &elem_basic_blocks_1->address
  FILL_IN_STRUCT(basic_blocks_1, "sss", "basic_blocks_1.tsv");
};

void ticl::get_instructions_1(std::string directory, vector<instructions_1>& vec_STRUCT)
{
#define ARGS_instructions_1  &elem_instructions_1->address, &elem_instructions_1->basic_block_id, &elem_instructions_1->mnemonic, \
    &elem_instructions_1->sequence, &elem_instructions_1->data
  FILL_IN_STRUCT(instructions_1, "sssss","instructions_1.tsv");
};

void ticl::get_callgraph_1(std::string directory, vector<callgraph_1>& vec_STRUCT)
{
#define ARGS_callgraph_1  &elem_callgraph_1->id, &elem_callgraph_1->src, &elem_callgraph_1->src_basic_block_id,	\
    &elem_callgraph_1->src_address, &elem_callgraph_1->dst
  FILL_IN_STRUCT(callgraph_1, "sssss","callgraph_1.tsv");
};

void ticl::get_control_flow_graph_1(std::string directory, vector<control_flow_graph_1>& vec_STRUCT)
{
#define ARGS_control_flow_graph_1  &elem_control_flow_graph_1->id, &elem_control_flow_graph_1->parent_function, \
    &elem_control_flow_graph_1->src, &elem_control_flow_graph_1->dst,	\
    &elem_control_flow_graph_1->kind
  FILL_IN_STRUCT(control_flow_graph_1, "sssss", "control_flow_graph_1.tsv");
};

void ticl::get_operand_strings_1(std::string directory, vector<operand_strings_1>& vec_STRUCT)
{
#define ARGS_operand_strings_1  &elem_operand_strings_1->id, &elem_operand_strings_1->str
  FILL_IN_STRUCT(operand_strings_1, "ss", "operand_strings_1.tsv");
};

void ticl::get_expression_tree_1(std::string directory, vector<expression_tree_1>& vec_STRUCT)
{
#define ARGS_expression_tree_1  &elem_expression_tree_1->id, &elem_expression_tree_1->expr_type, \
    &elem_expression_tree_1->symbol, &elem_expression_tree_1->immediate, \
    &elem_expression_tree_1->position, &elem_expression_tree_1->parent_id
  FILL_IN_STRUCT(expression_tree_1, "ssssss", "expression_tree_1.tsv");

};

void ticl::get_operand_tuples_1(std::string directory, vector<operand_tuples_1>& vec_STRUCT)
{
#define ARGS_operand_tuples_1  &elem_operand_tuples_1->address, &elem_operand_tuples_1->operand_id, \
    &elem_operand_tuples_1->position
  FILL_IN_STRUCT(operand_tuples_1, "sss", "operand_tuples_1.tsv");
};

void ticl::get_expression_substitutions_1(std::string directory, vector<expression_substitutions_1>& vec_STRUCT)
{
#define ARGS_expression_substitutions_1  &elem_expression_substitutions_1->id, &elem_expression_substitutions_1->address, \
    &elem_expression_substitutions_1->operand_id, &elem_expression_substitutions_1->expr_id, \
    &elem_expression_substitutions_1->replacement
  FILL_IN_STRUCT(expression_substitutions_1, "sssss","expression_substitutions_1.tsv");

};

void ticl::get_operand_expressions_1(std::string directory, vector<operand_expressions_1>& vec_STRUCT)
{
#define ARGS_operand_expressions_1  &elem_operand_expressions_1->operand_id, &elem_operand_expressions_1->expr_id
  FILL_IN_STRUCT(operand_expressions_1, "ss","operand_expressions_1.tsv");
};

void ticl::get_address_references_1(std::string directory, vector<address_references_1>& vec_STRUCT)
{
#define ARGS_address_references_1 &elem_address_references_1->address, &elem_address_references_1->target, &elem_address_references_1->kind
  FILL_IN_STRUCT(address_references_1, "sss","address_references_1.tsv");
};

void ticl::get_address_comments_1(std::string directory, vector<address_comments_1>& vec_STRUCT)
{
#define ARGS_address_comments_1  &elem_address_comments_1->address, &elem_address_comments_1->comment
  FILL_IN_STRUCT(address_comments_1, "ss", "address_comments_1.tsv");

};





