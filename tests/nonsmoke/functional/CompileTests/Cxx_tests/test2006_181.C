
#if __cplusplus
enum numbersEnum { I,J,K } Numbers;
#else
enum numbersEnum { I } Numbers;
#endif

enum numbersEnum;

#if __cplusplus
void foo (numbersEnum x);
#else
void foo (enum numbersEnum x);
#endif

/* We can only repeat this declaration using the enum once (not twice) */
#if __cplusplus
void foo (enum numbersEnum x);
#endif
