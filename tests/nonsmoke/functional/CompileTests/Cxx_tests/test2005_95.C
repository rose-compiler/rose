// DQ (6/24/2005): Not a very important bug in ROSE.
// This test code demonstrates the use of enums declarations with empty fileds.
// Basically "enum numbersEnum { } Numbers;" is unparsed as "enum numbersEnum Numbers;" which is an error.

// enum numbers;

#if __cplusplus
// Initial defining declaration of enum
enum numbers {};
// redundent declaration (not output by ROSE)
enum numbers;
#endif

enum letters { a,b,c };
#if __cplusplus
void foo1 (letters l);
#else
void foo1 (enum letters l);
#endif

#if __cplusplus
// enum numbersEnum Numbers;
// enum numbersEnum { one,two } Numbers;
// enum numbersEnum { } Numbers;
enum numbersEnum { } Numbers;
#else
enum numbersEnum { I,J,K } Numbers;
#endif
enum numbersEnum;
enum lettersEnum { x,y,z } Letters;

#if __cplusplus
void foo2 (numbersEnum x);
#else
void foo2 (enum numbersEnum x);
#endif

/* We can only repeat this declaration using the enum once (not twice) */
#if __cplusplus
void foo2 (enum numbersEnum x);
#endif
