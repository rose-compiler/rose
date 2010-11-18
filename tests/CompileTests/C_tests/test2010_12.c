/*
 * A complicate case for var_arg
 *
 *  From spec cpu 's gcc benchmark
 * Liao 11/11/2010
 * */

//#include <stdio.h>
#include <stdarg.h>

typedef struct
{
  va_list *format_args;

} output_state;

#define output_buffer_format_args(BUFFER) \
   *(((output_state *)(BUFFER))->format_args)

typedef struct 
{
  output_state state;

}output_buffer;

static void 
c_tree_printer (output_buffer* buffer)
{
  int t = va_arg (output_buffer_format_args (buffer), int);
}
