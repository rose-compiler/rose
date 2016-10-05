// C99 bug from Mozzila by Andreas.

int main(){

    typedef enum {
       finish_started,
    } block_state;
    
    if (0) return (0) ? finish_started : finish_started;
};
