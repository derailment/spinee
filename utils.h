#ifndef __UTILS__
#define __UTILS__

#define IS_BIT_SET(n, pos) ((n & 1 << pos) != 0)
#define SET_BIT(n, pos) (n = n | 1 << pos)

typedef enum bool_ bool_t; 

enum bool_{
    FALSE,
    TRUE
};

#endif