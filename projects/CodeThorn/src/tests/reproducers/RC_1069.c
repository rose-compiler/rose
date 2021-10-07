
struct cmdline_option {
  const char *long_name;
  char short_name;
  enum {
    OPT_VALUE,
  } type;
  const void *data;
  int argtype;
};

static struct cmdline_option option_data[] =
  {
    { "dot-style", 0, OPT_VALUE, "dotstyle", -1 } /* problematic comment */
  };

int main() {}
