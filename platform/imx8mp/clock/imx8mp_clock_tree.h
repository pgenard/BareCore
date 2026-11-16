#ifndef IMX8MP_CLOCK_TREE_H
#define IMX8MP_CLOCK_TREE_H

typedef enum {
    CLOCK_FIXED,
    CLOCK_PLL,
    CLOCK_FIXED_FACTOR,
    CLOCK_MUX,
    CLOCK_DIVIDER,
    CLOCK_GATE,
    CLOCK_ROOT
} clock_type_t;

typedef struct clock_desc {
    const char *name;
    unsigned int id;
    clock_type_t type;
    unsigned int parent_id;
    unsigned long frequency;
    unsigned int enabled;
    unsigned int valid;
} clock_desc_t;

#endif
