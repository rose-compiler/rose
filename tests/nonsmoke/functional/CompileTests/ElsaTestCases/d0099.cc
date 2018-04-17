//  /home/ballB/festival-1.4.2-3/EST_Chunk-4Asc.ii
//  error: function definition of `operator<<' must appear in a namespace that encloses the original declaration
class X {
    friend void operator << (int &i, const X &x) { };
};
