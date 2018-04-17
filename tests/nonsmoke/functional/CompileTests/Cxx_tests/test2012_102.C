// This is interesting code!!!  But now it works...
struct T { enum c { iiii }; };
struct U : T { using T::iiii; };

