#ifndef __FORTRAN_ERROR_HANDLER_H
#define __FORTRAN_ERROR_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "token.h"

void set_current_token(Token_t *token);

void fortran_error_handler_end();

void fortran_error_handler_begin();

#ifdef __cplusplus
}
#endif 

#endif
