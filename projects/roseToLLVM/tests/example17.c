#include <stdio.h>
#include <string.h>

//
// more pointer arithmetic tests
//
int main(int argc, char *argv[]) {
    char token[30] = { "Barack Obama" };
    char left_token[15],
         right_token[15];
    char *first_name,
         *last_name,
         *cut_first_name,
         *cut_last_name,
         *cut_name;
    int pointer_difference;

    strncpy(left_token, token, 6);
    left_token[6] = '\0';
    strcpy(right_token, token + 7);
    first_name = left_token;
    last_name = right_token;
    cut_first_name = first_name + 2;
    cut_last_name = 2 + last_name;
    cut_name = token + 3;
    pointer_difference = last_name - first_name;
    pointer_difference = right_token - left_token;

    printf("left-token = %s\n", left_token);
    printf("right-token = %s\n", right_token);
    printf("first-name = %s\n", first_name);
    printf("last-name = %s\n", last_name);
    printf("cut-first-name = %s\n", cut_first_name);
    printf("cut-last-name = %s\n", cut_last_name);
    printf("cut-name = %s\n", cut_name);
    printf("pointer_difference = %i\n", pointer_difference);

    cut_first_name++;
    cut_last_name--;
    printf("cut-first-name = %s\n", cut_first_name);
    printf("cut-last-name = %s\n", cut_last_name);

    --cut_first_name;
    ++cut_last_name;
    printf("cut-first-name = %s\n", cut_first_name);
    printf("cut-last-name = %s\n", cut_last_name);

    cut_first_name += 2;
    cut_last_name -= 2;
    printf("cut-first-name = %s\n", cut_first_name);
    printf("cut-last-name = %s\n", cut_last_name);

    char *name = first_name;

    *(name++) = 'v';
    *(++name) = 'o';
    ++(*name);
    (*name) += 1;
    --(*name);
    (*name) -= 1;

    printf("name = %s\n", name);
    printf("first-name = %s\n", first_name);

    return 0;
}
