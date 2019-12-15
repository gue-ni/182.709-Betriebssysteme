/**
 * @file util.h
 */
#ifndef util_H__
#define util_H__

#define SHM_NAME    "/shared"
#define FREE_SEM    "/fb_free"
#define USED_SEM    "/fb_used"
#define MUTEX       "/fb_mutex"
#define MAX_DATA    (256)

struct circ_buf {
    int read_pos;
    int write_pos;
    int data[MAX_DATA];
};

//void write_message(char *prog, char *msg);
void test(void);
#endif /* util_H__ */

