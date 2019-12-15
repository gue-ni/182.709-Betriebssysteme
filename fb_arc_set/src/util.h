/**
 * @file util.h
 */
#ifndef util_H__
#define util_H__

#define SHM_NAME    "/shared"
#define FREE_SEM    "/free_sem"
#define USED_SEM    "/used_sem"
#define MUTEX       "/mutex_sem"
#define MAX_DATA    (256)
#define PROG "not defined"

struct shared_mem {
    int read_pos;
    int write_pos;
    int data[MAX_DATA];
};

void test(void);
#endif /* util_H__ */

