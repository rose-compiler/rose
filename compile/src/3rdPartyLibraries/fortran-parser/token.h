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

#ifndef __token_h__
#define __token_h__

#ifdef __cplusplus
extern "C" {
#endif

  /* Later we will typedef Token to being the ANTLR C runtime common
	  token.  This should allow any references to Token or Token *
	  variables to work with the ANTLR C runtime w/o drastic changes to
	  their code.  */
  typedef struct Token
  {
	 /* The minimum four fields.  */
	 int line;
	 int col;
	 int type;
	 char *text;
  }Token_t;

  /* These are declared and initialized in token.c.  */
  extern Token_t **token_list;
  extern int token_list_size;
  extern int num_tokens;

  Token_t *create_token(int line, int col, int type, const char *text);
  void free_token(Token_t *tmp_token);
  void free_token_list(void);
  void register_token(Token_t *tmp_token);
  void print_token(Token_t *tmp_token);
  Token_t *get_latest_token();
#ifdef __cplusplus
} /* End extern C. */
#endif

#endif
