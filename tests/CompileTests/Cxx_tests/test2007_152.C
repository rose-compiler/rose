// This works fine in ROSE, but it is not clear if it has been tested previously.

// Typedef with multiple types declared (each is an typedef of int to x,y,z)
// typedef int x,y,z; is equivalent to:
// typedef int x;
// typedef int y;
// typedef int z;
typedef int x,y,z;
