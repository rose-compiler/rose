#ifndef __token_h__
#define __token_h__

#include "JavaSourceCodePosition.h"

#if 0
typedef struct Token
   {
  /* The minimum four fields.  */
     std::string text;
     int line_number;
     int column_number;

  // I don't think I need the token type.
  // int type;
   } Token_t;
#else
class Token_t
   {
     private:
          std::string text;
          JavaSourceCodePosition * pos_info;
  // I don't think I need the token type.
  // int type;
     public:
          Token_t (std::string s, JavaSourceCodePosition * pos_info);
          JavaSourceCodePosition * getSourcecodePosition();
          std::string getText();
   };
#endif



#if 0
  /* These are declared and initialized in token.c.  */
extern Token_t **token_list;
extern int token_list_size;
extern int num_tokens;

Token_t *create_token(int line, int col, int type, const char *text);
void free_token(Token_t *tmp_token);
void free_token_list(void);
void register_token(Token_t *tmp_token);
void print_token(Token_t *tmp_token);
#endif
  
#endif
