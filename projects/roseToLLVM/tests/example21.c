#include <stdio.h>
#include <stdlib.h>

//
// Test function that returns a smal structure with size <= 64 bits
//
struct node
{
    int index;
    struct node *next;
};
struct pair
{
    int left, right;
};
struct node **clitems;
struct node node_pool[] = { {0, NULL}, {1, NULL}, {2, NULL}, {3, NULL}, {4, NULL}};
struct pair pair_pool[] = { {0, 0}, {1, 2}, {2, 4}, {3, 6}, {4, 8}};
int node_pool_index = 0,
    pair_pool_index = 0;
struct node getNode() {
    int k = node_pool[node_pool_index].index;
    struct node temp = node_pool[node_pool_index++];
    struct node temp2 = temp;
    return temp;
}
struct pair getPair() { return pair_pool[pair_pool_index++]; }
int main(int argc, char *argv[]) {
     clitems = (struct node **) calloc(10, sizeof(struct node *));
     clitems[0] = (struct node *) malloc(sizeof(struct node));
     clitems[0] -> next = NULL; // ROSE converts this statement into:  (*clitems[0]).next = NULL;

     struct pair my_pair = getPair();
     my_pair = getPair();
     printf("Value of pair 1 is (%i, %i)\n", my_pair.left, my_pair.right);

     for(int i = 0; i < 5; i++) {
         struct node my_node = getNode();
         printf("Value at node %i is %i\n", i, my_node.index);
     }
 }
