#ifndef UTIL_TYPES_H
#define UTIL_TYPES_H

typedef struct vec2 vec2;
typedef struct pose pose;

struct vec2 { int x; int y; }; 
struct pose { vec2 pos; int dir; }; 


const vec2 vec2_zero = { .x = 0, .y = 0 };


vec2 add(vec2 v1, vec2 v2)
{
    vec2 v = { .x = v1.x + v2.x, .y = v1.y + v2.y };
    return v;
}


bool is_equal(vec2 v1, vec2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}


int dist2(vec2 v1, vec2 v2)
{
    int dx = (v1.x - v2.x);
    int dy = (v1.y - v2.y);
    return dx * dx + dy * dy;
}

#endif