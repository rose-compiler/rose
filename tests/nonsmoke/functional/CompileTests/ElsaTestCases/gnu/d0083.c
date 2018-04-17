// ./balsa-2.0.6-1/ab-window-67O9.i.c_out:4:/home/ballAruns/tmpfiles/./balsa-2.0.6-1/ab-window-67O9.i:31990:5:
// error: there is no type called `LibBalsaMessageFlag'

// looks like you can make a typedef to a enum that doesn't exist yet

typedef enum F0 F;

enum F0 {
    f1 = 1,
};

struct S {
    F flags;
};
