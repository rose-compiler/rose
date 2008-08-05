void strcpy_noncompliant(char *dest, const char *source)
{
    while (*dest++ = *source++)
        ;
}

void strcpy_compliant(char *dest, const char *source)
{
    char last = *source;
    do {
        last = *source;
        *dest++ = *source++;
    } while (last != '\0');
}
