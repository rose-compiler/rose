int snippet_usage_counter = 0;
extern int VAR_SHOULD_NOT_BE_INJECTED;                  // externs are not injected
typedef int TYPE_SHOULD_NOT_BE_INJECTED;                // typedefs are not injected

void count() 
{
    ++snippet_usage_counter;
}
