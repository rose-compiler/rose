// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<class T> concept C = requires {
    new int[-(int)sizeof(T)]; // invalid for every T: ill-formed, no diagnostic required
};


