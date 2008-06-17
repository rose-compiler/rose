/*
* Various usage of shared 
* Liao 
* 6/9/2008
*/
typedef shared [3] void upc_lock_t;
int * shared[3] p1; /* shared to private*/
int * shared p11;   
shared [10] int *p2;  /* private to shared */
shared  int *p22;  
shared [10] int * shared[3] p3; /* shared to shared */
shared int * shared p33;
shared[] int* shared p4; /* shared to shared data allocated to thread 0*/
