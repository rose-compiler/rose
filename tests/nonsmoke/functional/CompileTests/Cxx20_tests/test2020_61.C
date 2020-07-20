template<class T> concept C = requires {
    new int[-(int)sizeof(T)]; // invalid for every T: ill-formed, no diagnostic required
};


