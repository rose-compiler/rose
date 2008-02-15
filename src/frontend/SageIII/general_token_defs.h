/* DQ (9/24/2007): This is part of Rama's Lexer */
#ifndef ROSE_LEX_TOKEN_DEFINITIONS
#define ROSE_LEX_TOKEN_DEFINITIONS 1

struct token_element
   {
  // This is the simplist contect of a Token

  // value of the token
     std::string token_lexeme;

  // The classification for the token (language dependent)
     int token_id;
   };

struct file_pos_info
   {
  // Source code position information for token
     int line_num;
     int column_num;
   };

// At some point I want to replace this with a SgToken 
// IR node until then this is an intermediate step.
struct stream_element
   {
  // This is the element in the token stream.

  // This is the pointer to the token
     struct token_element * p_tok_elem;

  // Positon of the start of the token
     struct file_pos_info beginning_fpi;

  // Position of the end of the token
     struct file_pos_info ending_fpi;
   };

// At present this is an STL list, but I would like it to be a vector at some point
typedef std::list<stream_element*> LexTokenStreamType;
typedef LexTokenStreamType* LexTokenStreamTypePointer;

// endif for ROSE_LEX_TOKEN_DEFINITIONS
#endif
