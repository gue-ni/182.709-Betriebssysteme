/**
 * @file common.h
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 2019-12-16
 * @brief
 */
#ifndef common_H__
#define common_H__

#include <signal.h>
#include <stdint.h>
#define SHM_NAME            "/shared"
#define FREE_SEM            "/fb_free"
#define USED_SEM            "/fb_used"
#define MUTEX               "/fb_mutex"
#define MAX_DATA            (50)
#define MAX_SOLUTION_SIZE   (8)

/**
 * u->v
 */
struct edge {
    uint8_t u;
    uint8_t v;
};

/**
 * 
 * rp: read position on circular buffer
 * wp: write position on circular buffer
 * quit: generators exit when set to 1
 * size: size of the solution written to the buffer, not be larger than 8
 * data: array containing the solutions 
 */
struct circ_buf {
    uint8_t rp;
    uint8_t wp;
    volatile sig_atomic_t quit;
    uint8_t size[MAX_DATA];
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