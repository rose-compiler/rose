/* Liao, 5/14/2009
 * A test case extracted from h264ref of the spec cpu 2006.
 * */
typedef struct
{
    unsigned int  Elow;
} EncodingEnvironment;

typedef EncodingEnvironment *EncodingEnvironmentPtr;

#define Elow            (eep->Elow)
void arienco_start_encoding(EncodingEnvironmentPtr eep)
{
    Elow = 0;
}
