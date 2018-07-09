#if 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49

class node;

class node;

class node {
    public: 
    class node* prev;
    class node* next;
    unsigned int val;
    unsigned int array1[PTR2_SIZE];

    void set_val(unsigned int input) {
        val = input;
    }

    unsigned int get_val() {
        return val;
    }

};
#endif

int* fn1(int int_array[][3], signed char char_array[]) {
    
    unsigned int counter = 0;
    unsigned int index1 = sizeof(int_array)/sizeof(int_array[0]);

    printf("fn1: Printing int_array\n");
    for(; counter < index1; counter++) {
        unsigned int counter2 = 0;
        for(; counter2 < 3; counter2++) {
            printf("%d\n", int_array[counter][counter2]);
        }
    }
    printf("fn1: Done\n");

    index1 = sizeof(char_array)/sizeof(char);
    printf("fn1: Printing char_array\n");
    char char_counter = 0;
    for(; char_counter < index1; char_counter++) {
        printf("%c\n", char_array[char_counter]);
    }
    printf("fn1: Done\n");

    return NULL;
}

int main() {
       
    #if 1
    unsigned int *no_init_ptr;
    printf("before ptr\n");
    unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    printf("before ptr2\n");
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));
    printf("before ptr_index\n");
    unsigned int* ptr_index = ptr;
    unsigned int counter = 0;
    printf("before loop1\n");
    for(; ptr_index < (ptr + PTR_SIZE); ptr_index++) {
        *ptr_index = counter++;
        printf("%lu\n", reinterpret_cast<unsigned long>(ptr_index));
    }
    #else
    unsigned int counter = 0;
    #endif

    
    int int_array_complex[] = {1, 1, 2, 3, 5, 8, 13, 21};
    signed char schar_array_complex[] = {"abc"};
    int myPoints[][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9} };
    int arr3d[2][3][4] = { { {1, 2, 3, 4}, {1, 2, 3, 4}, {1, 2, 3, 4} },
                     { {1, 2, 3, 4}, {1, 2, 3, 4}, {1, 2, 3, 4} } };
    int* myPoints_ptr = (int*)myPoints;

    fn1(myPoints, schar_array_complex); 

    int* int_array_complex_ptr = (int*)int_array_complex;
    printf("int_array_complex begin\n");
    for(counter = 0; counter < (sizeof(int_array_complex)/sizeof(int)); counter++) {
        printf("%d\n", int_array_complex[counter]);
        printf("check: %d\n", *(int_array_complex_ptr 
                            + (counter*(sizeof(int_array_complex[0])/sizeof(int_array_complex[0])))));
    }
    printf("int_array_complex end\n");

    signed char* schar_array_complex_ptr = (signed char*)schar_array_complex;
    printf("schar_array_complex begin\n");
    for(counter = 0; counter < (sizeof(schar_array_complex)/sizeof(char)); counter++) {
        printf("%c\n", schar_array_complex[counter]);
        printf("check: %c\n", *(schar_array_complex_ptr 
                    + (counter*(sizeof(schar_array_complex[0])/sizeof(schar_array_complex[0])))));
    }
    printf("schar_array_complex end\n");

    #if 1
    printf("myPoints begin\n");
    for(counter = 0; counter < (sizeof(myPoints)/sizeof(myPoints[0])); counter++) {
        int counter2;
        for(counter2 = 0; counter2 < (sizeof(myPoints[counter])/sizeof(myPoints[0][0])); counter2++) {
            printf("%d\n", myPoints[counter][counter2]);
            #if 1
            printf("check: %d\n", *(myPoints_ptr + (counter*(sizeof(myPoints[0])/sizeof(myPoints[0][0])))
                + (counter2*(sizeof(myPoints[0][0])/sizeof(myPoints[0][0])))));
            #endif
        }
    }
    printf("myPoints end\n");
    #endif
    #if 1
    int* arr3d_ptr = (int*)arr3d;
    printf("arr3d begin\n");
    for(counter = 0; counter < (sizeof(arr3d)/sizeof(arr3d[0])); counter++) {
        int counter2;
        for(counter2 = 0; counter2 < (sizeof(arr3d[counter])/sizeof(arr3d[0][0])); counter2++) {
            int counter3;
            for(counter3 = 0; counter3 < (sizeof(arr3d[counter][counter2])/sizeof(arr3d[0][0][0])); counter3++) {
                printf("%d\n", arr3d[counter][counter2][counter3]);
                // TESTING
                #if 0
                int *temp = arr3d + (counter*(sizeof(arr3d[0])/sizeof(arr3d[0][0][0])));
                temp = arr3d[0] + (counter*(sizeof(arr3d[0])/sizeof(arr3d[0][0][0])));
                temp = arr3d[0][0] + (counter*(sizeof(arr3d[0])/sizeof(arr3d[0][0][0])));
                #endif
                #if 1
                printf("check: %d\n", *(arr3d_ptr + (counter*(sizeof(arr3d[0])/sizeof(arr3d[0][0][0])))
                                         + (counter2*(sizeof(arr3d[0][0])/sizeof(arr3d[0][0][0])))
                                         + (counter3*(sizeof(arr3d[0][0][0])/sizeof(arr3d[0][0][0])))));
                #endif
            }
        }
    }
    printf("arr3d end\n");
    #endif

    #if 1
    #if 1
    int array[PTR_SIZE];
    printf("before loop2\n");
    for(counter = 0; counter < PTR_SIZE; counter++) {
        array[counter] = counter;
    }

    printf("%u\n", *array); 
    printf("%u\n", array[0]);
    printf("%u\n", *array);
    printf("Done\n");
    #endif
    printf("before loop3\n");
    for(ptr_index = ptr + PTR_SIZE - 1; ptr_index >= ptr; ptr_index--) {
        printf("%lu\n", reinterpret_cast<unsigned long>(ptr));
        printf("%lu\n", reinterpret_cast<unsigned long>(ptr_index));
        printf("%u\n", *ptr_index);
    }
    
    #if 1
    printf("before loop4\n");
    class node class_array[PTR_SIZE];
    for(counter = 0; counter < PTR_SIZE; counter++) {
        class_array[counter].set_val(counter);
    }
    printf("Printing class array\n");
    printf("before loop5\n");
    for(counter = 0; counter < PTR_SIZE; counter++) {
        printf("%u\n", class_array[counter].get_val());
    }
    #endif


    #if 1
    int* array_ptr[PTR_SIZE];
    printf("before loop6\n");
    for(counter = 0; counter < PTR_SIZE; counter++) {
        array_ptr[counter] = (int*)malloc(sizeof(int));
        *array_ptr[counter] = counter;
    }
    printf("Printing array\n");
    printf("before loop7\n");
    for(counter = 0; counter < PTR_SIZE; counter++) {
        printf("%u\n", *array_ptr[counter]);
    }
    #endif

    #if 0
    class node* class_array_ptr[PTR_SIZE];
    printf("before loop8\n");
    for(counter = 0; counter < PTR_SIZE; counter++) {
        class_array_ptr[counter] = (class node*)malloc(sizeof(class node));
        class_array_ptr[counter]->set_val(counter);
    }
    printf("Printing class array\n");
    printf("before loop9\n");
    for(counter = 0; counter < PTR_SIZE; counter++) {
        printf("%u\n", class_array_ptr[counter]->get_val());
    }
    #endif
    #endif
    return 1;
}
