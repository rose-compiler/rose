#ifndef __ANDREAS_MACRO_H_
#define __ANDREAS_MACRO_H_

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>

#ifndef MACDB_NAME
#define MACDB_NAME "macros-sqlite"
#endif

typedef struct macro_def_id_tag {
  char * filename;
  int lineno;
} macro_def_id_t;

typedef struct macro_def_tag {
  long long int id;
  char * filename;
  int lineno, colno;
  char * name;
  char * def;
  std::vector<char *> args;
} macro_def_t;

typedef struct macro_call_id_tag {
  char * filename;
  int lineno;
  int colno;
} macro_call_id_t;

typedef struct macro_call_tag {
  long long int id;
  char * filename;
  int lineno, colno;
  std::vector<char *> tokenlist;
  std::vector<char *> args;
  int macdef_id;
  char * expanded;
  char * unparsed_statements;
} macro_call_t;


int add_macro_def(const std::string& filename, int lineno, int colno, const std::string& name, 
		  const std::string& def, std::vector<std::string>& args);
int add_macro_call(const std::string& filename, int lineno, int colno, std::vector<std::string>& tokenlist,
                   std::vector<std::string>& args, const std::string& macdeffilename, int macdeflineno,
		   const std::string& expanded, const std::string& unparsed);
int replace_macro_call(const std::string& filename, int lineno, int colno, std::vector<std::string>& tokenlist,
                       std::vector<std::string>& args, const std::string& macdeffilename, int macdeflineno,
		       const std::string& expanded, const std::string& unparsed);

bool exist_macro_def(const std::string& filename, int lineno); // if found, return true; otherwise, return false. TODO: check uniqueness
bool exist_macro_call(const std::string& filename, int lineno, int colno); // if found, return true; otherwise, return false. TODO: check uniqueness

macro_def_t find_macro_def(const std::string& filename, int lineno); // if not found, set the .id to -1;
macro_call_t find_macro_call(const std::string& filename, int lineno, int colno); // if not found, set the .id to -1;

void output_macdef(std::ostream & out, const macro_def_t &);
void output_maccall(std::ostream & out, const macro_call_t &);

void iterate_macro_defs_calls();
bool compare_macro_tokenlists(std::vector<char*> mcall, std::vector<char*> first_mcall, bool macro_def_printed, bool filter = false);
bool compare_macro_tokenlists(std::vector<std::string> mcall, std::vector<std::string> first_mcall, bool filter = false);
void compare_macro_tokenlists(std::vector<macro_def_t*>& macdefs, std::vector<std::pair<macro_def_t*, std::vector<macro_call_t> > >& maccalls); /* TODO: implement the bug detection alg. */

/* The following seems unnecessary for now because of all data are
   stored into C++ vectors. cf. comments in the definition of
   "iterate_macro_defs_calls". */
char * get_varchar_in_macro_def(macro_def_id_t mid, const char * field);
int get_int_in_macro_def(macro_def_id_t mid, const char * field);

char * get_varchar_in_macro_call(macro_call_id_t mid, const char * field);
int get_int_in_macro_call(macro_call_id_t mid, const char * field);
char * get_macro_def_from_call(macro_call_id_t mid);


int init_macdb(const char * pathtodb); /* return 0 if succeed; if the db file doesn't exist, it will create a new one. */
int close_macdb();

/* We may need more generic delete_* functions... */
int delete_macro_defs_in_file(const char * filename);
int delete_macro_calls_in_file(const char * filename);
int delete_all_defs_calls();

#endif
