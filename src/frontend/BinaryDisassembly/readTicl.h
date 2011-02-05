#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdarg.h>
#include <cassert>
#include <vector>

namespace ticl {

struct modules
{
  std::string id;
  std::string name;
  std::string md5;
  std::string sha1;
  std::string comment;
  std::string import_time;
};

struct functions_1
{
  std::string address;
  std::string name;
  std::string function_type;
  std::string name_md5; 
};

struct basic_blocks_1
{
    std::string id;
        std::string parent_function;
        std::string address;
};

struct instructions_1
{
  std::string address;
  std::string basic_block_id;
  std::string mnemonic;
  std::string sequence;
  std::string data;

};

struct callgraph_1
{
  std::string id;
  std::string src;
  std::string src_basic_block_id;
  std::string src_address;
  std::string dst;
};

struct control_flow_graph_1
{
  std::string id;
  std::string parent_function;
  std::string src;
  std::string dst;
  std::string kind;
};

struct operand_strings_1
{
  std::string id;
  std::string str;
};

struct expression_tree_1
{
  std::string id;
  std::string expr_type;
  std::string symbol;
  std::string immediate;
  std::string position;
  std::string parent_id;
};

struct operand_tuples_1
{
  std::string address;
  std::string operand_id;
  std::string position;
};


struct  expression_substitutions_1
{
  std::string id;
  std::string address;
  std::string operand_id;
  std::string expr_id;
  std::string replacement;
};

struct operand_expressions_1{
  std::string operand_id;
  std::string expr_id;
};

struct  address_references_1
{
  std::string address;
  std::string target;
  std::string kind;
};

struct  data_1
{
  std::string address;
  std::string name;
  std::string length;
  std::string data;
  std::string is_string;
};

struct sections_1
{
  std::string name;
  std::string base;
  std::string start_address;
  std::string end_address;
  std::string length;
  std::string data;
};

struct address_comments_1
{
  std::string address;
  std::string comment;
};



void get_modules(std::string directory, std::vector<modules>& vec_STRUCT);

void get_functions_1(std::string directory, std::vector<functions_1>& vec_STRUCT);

void get_basic_blocks_1(std::string directory, std::vector<basic_blocks_1>& vec_STRUCT);

void get_instructions_1(std::string directory, std::vector<instructions_1>& vec_STRUCT);

void get_callgraph_1(std::string directory, std::vector<callgraph_1>& vec_STRUCT);

void get_control_flow_graph_1(std::string directory, std::vector<control_flow_graph_1>& vec_STRUCT);

void get_operand_strings_1(std::string directory, std::vector<operand_strings_1>& vec_STRUCT);

void get_expression_tree_1(std::string directory, std::vector<expression_tree_1>& vec_STRUCT);

void get_operand_tuples_1(std::string directory, std::vector<operand_tuples_1>& vec_STRUCT);

void get_expression_substitutions_1(std::string directory, std::vector<expression_substitutions_1>& vec_STRUCT);

void get_operand_expressions_1(std::string directory, std::vector<operand_expressions_1>& vec_STRUCT);

void get_address_references_1(std::string directory, std::vector<address_references_1>& vec_STRUCT);

void get_address_comments_1(std::string directory, std::vector<address_comments_1>& vec_STRUCT);

}

