// This code demonstrates a fundamental problem in the
// unparsing of #include directives, this will be fixed
// when better hanlding of the token stream is available.

const char * const jpeg_std_message_table[] = {
#include "test2007_101.h"
  "bar"
};

int main(){};

