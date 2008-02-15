// DQ (6/24/2005): Not a very important bug in ROSE.
// This test code demonstrates the use of enums declarations with empty fileds.
// Basically "enum numbersEnum { } Numbers;" is unparsed as "enum numbersEnum Numbers;" which is an error.

// enum numbers;

#if 1
// Initial defining declaration of enum
enum numbers {};
// redundent declaration (not output by ROSE)
enum numbers;
#endif

#if 1
enum letters { a,b,c };
void foo (letters l);
#endif

#if 1
// enum numbersEnum Numbers;
// enum numbersEnum { one,two } Numbers;
enum numbersEnum { } Numbers;
enum numbersEnum;
enum lettersEnum { x,y,z } Letters;
#endif

#if 1
void foo (numbersEnum x);
void foo (enum numbersEnum x);
#endif

