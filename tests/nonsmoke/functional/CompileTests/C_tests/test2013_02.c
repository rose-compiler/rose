// Test the handling of __PRETTY_FUNCTION__ within if statements
// The hidden declaration for it should be inserted into global scope, instead of if-stmt
// The code is extracted from irssi-0.8.15/src/fe-text/textbuffer-view.c
// Liao 1/15/2013

void g_assert_warning (
    const char *pretty_function);


void textbuffer_view_get_line_cache(void *view)
{
  if (view !=0)
  { }
  else
    g_assert_warning (
            __PRETTY_FUNCTION__);

  if (view) 
  { }
  else
    g_assert_warning (
            __PRETTY_FUNCTION__);


}

