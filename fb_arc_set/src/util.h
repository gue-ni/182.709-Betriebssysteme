/**
 * @file util.h
 */
#ifndef util_H__
#define util_H__

#include <signal.h>
#define SHM_NAME    "/shared"
#define FREE_SEM    "/fb_free"
#define USED_SEM    "/fb_used"
#define MUTEX       "/fb_mutex"
#define MAX_DATA    (256)
#define MAX_SOLUTION_SIZE   (8)

struct edge {
    int u;
    int v;
};

struct circ_buf {
    int read_pos;
    int write_pos;
    volatile sig_atomic_t quit;
    int solution_size[MAX_DATA];
    struct edge data[MAX_DATA][MAX_SOLUTION_SIZE];
};

//void write_message(char *prog, char *msg);
void test(void);
#endif /* util_H__ */

