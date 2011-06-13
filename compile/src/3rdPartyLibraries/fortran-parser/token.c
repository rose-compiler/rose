/******************************************************************************
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL), which
 * is operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "token.h"
//#include "fortran_error_handler.h"

#define INIT_TOKEN_LIST_SIZE 1024

  Token_t **token_list   = NULL;
  Token_t *token_current = NULL;
  int token_list_size = 0;
  int num_tokens = 0;
  
  Token_t *create_token(int line, int col, int type, const char *text)
  {
	 Token_t *tmp_token = NULL;

	 tmp_token = (Token_t*)malloc(sizeof(Token_t));
	 tmp_token->line = line;
	 tmp_token->col = col;
	 tmp_token->type = type;
	 /* Make a copy of our own to make sure it isn't freed on us.  */
	 if(text != NULL)
		tmp_token->text = strdup(text);
	 else
		tmp_token->text = NULL;

	 return tmp_token;
  }

  void free_token(Token_t *tmp_token)
  {
	 /* Nothing to do.  */
	 if(tmp_token == NULL)
		return;
  
	 /* Free up the memory allocated for the string.  */
	 if(tmp_token->text != NULL)
		free(tmp_token->text);
	 /* Free up the token memory itself.  */
	 free(tmp_token);
  }

  void free_token_list()
  {
     int i;

     /* Laksono 2009.12.15: free the error handler */
     //  fortran_error_handler_end();

     for (i = 0; i < num_tokens; i++) {
        if (token_list[i] != NULL) {
           free_token(token_list[i]);
        }
     }

     if (token_list != NULL) {
        num_tokens = 0;
        token_list_size = 0;
        free(token_list);
        token_list = NULL;
     }

     return;
  }

  void register_token(Token_t *tmp_token)
  {
	 if(token_list == NULL)
	 {
		int i;
		
		/* We haven't allocated a token list yet.  This is done here because
			we currently don't have a single entry point into the actions, so we
			don't have a place to do this.  We could expect the user to call an
			init routine, as one option....  */
		token_list = (Token_t**)malloc(INIT_TOKEN_LIST_SIZE * sizeof(Token_t *));
		if(token_list == NULL)
		{
		  fprintf(stderr, "Error: Unable to allocate token_list!\n");
		  exit(1);
		}
		token_list_size = INIT_TOKEN_LIST_SIZE;

		/* Initialize the array pointers to NULL.  */
		for(i = 0; i < token_list_size; i++)
		  token_list[i] = NULL;

                /* Laksono 2009.12.05:  intercept SIGABRT */
                //fortran_error_handler_begin();

	 }
	 else if(num_tokens == token_list_size)
	 {
		int i;
		int orig_size = token_list_size;
		
		/* The list is full so reallocate to twice it's current size.  */
		token_list = (Token_t**)realloc(token_list,(token_list_size << 1) * sizeof(Token_t *));
		if(token_list == NULL)
		{
		  fprintf(stderr, "Error: Out of memory for token_list\n");
		  exit(1);
		}
		token_list_size = token_list_size << 1;

		/* Initialize all new elements to NULL.  */
		for(i = orig_size; i < token_list_size; i++)
		  token_list[i] = NULL;
	 }

	 /* The list is allocated and there is enough room for the new element.  */
	 token_list[num_tokens] = tmp_token;
	 num_tokens++;
         if (tmp_token->line >0)
            token_current = tmp_token;
         //printf("token %d: l=%d t=%d s=%s\n", num_tokens, tmp_token->line, tmp_token->type, tmp_token->text);

	 if(num_tokens >= INT_MAX)
	 {
		fprintf(stderr, "Error: Maximum number of tokens reached!\n");
		exit(1);
	 }

	 return;
  }

  void print_token(Token_t *tmp_token)
  {
	 /* The fields of these are printed in a form similar to what ANTLR would
		 print for a Token printed as a String.  However, we don't copy all of
		 ANLTR's internal info for the Token, so the first field (@0) and the
		 '0:0=' part of the second field have no meaning for our C tokens but
		 are there for similarity.  */
	 printf("[@0, '0:0=%s', <%d>, %d:%d]\n", 
			  (tmp_token->text == NULL ? "null" : tmp_token->text),
			  tmp_token->type, tmp_token->line, tmp_token->col);
  }

  Token_t *get_latest_token()
  {
     int ltok = num_tokens-1;
     Token_t *current_token = token_current;
/*
     if (token_list == NULL) return NULL;
     do {
        current_token = token_list[ltok];
        ltok--; 
     } while (ltok>=0 && current_token != NULL && current_token->line == 0);
*/
     //printf(">> get_latest_token %d %d:'%s'\n", ltok, current_token->line, current_token->text);
     return current_token;
  }

#ifdef __cplusplus
} /* End extern C. */
#endif
