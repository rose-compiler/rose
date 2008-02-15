// duplicate modifiers in C as warning, not error (?)

// originally found in package ffmpeg_0.cvs20050108-1

// a.i:2:1: error: duplicate modifier: const

// ERR-MATCH: duplicate modifier:

const const int x;

//ERROR(1): signed signed int y;
