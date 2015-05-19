struct point { int y; };

// const int xv0 = 0;
// const int xv2 = 20;
// const int yv2 = 4;

// Should be unparsed as: struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };
// Or at least: struct point ptarray[10] = {[2UL].y = (2), [2UL].x = (20), [0UL].x = (0)};
// Currently unparses as: struct point ptarray[10] = {[2UL] = ({.y = (2)}), [2UL] = ({.x = (20)}), [0UL] = ({.x = (0)})};
// struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };

// Original code:
// struct point ptarray[10] = { [2].y = 4 };
struct point ptarray[10] = { [2].y = 4 };
