/**
 * @file common.h
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 2019-12-16
 * @brief
 */
#ifndef common_H__
#define common_H__

#include <signal.h>
#define SHM_NAME            "/shared"
#define FREE_SEM            "/fb_free"
#define USED_SEM            "/fb_used"
#define MUTEX               "/fb_mutex"
#define MAX_DATA            (256)
#define MAX_SOLUTION_SIZE   (8)

/**
 *
 */
struct edge {
    int u;
    int v;
};

/**
 *
 */
struct circ_buf {
    int read_pos;
    int write_pos;
    volatile sig_atomic_t quit;
    int solution_size[MAX_DATA];
    struct edge data[MAX_DATA][MAX_SOLUTION_SIZE];
};

/**
 * @brief
 * @details
 * @param
 * @return
 */
void exit_error(char *p, char *msg);
#endif /* common_H__ */