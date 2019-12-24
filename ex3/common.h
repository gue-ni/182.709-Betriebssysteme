/**
 * @file common.h
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 2019-12-16
 * @brief
 */
#ifndef COMMON_H__
#define COMMON_H__

#include <signal.h>
#include <stdint.h>
#define SHM_NAME            "/shared_mem"
#define FREE_SEM            "/fb_free"
#define USED_SEM            "/fb_used"
#define MUTEX               "/fb_mutex"
#define MAX_DATA            (128)
#define MAX_SOLUTION_SIZE   (8)

/**
 * Represents an edge from u->v
 */
typedef struct edge {
    uint8_t u; /**< Vertex u */
    uint8_t v; /**< Vertex v */
} edge_t;

/**
 * 
 */
typedef struct circ_buf {
    volatile sig_atomic_t quit; /**<  quit: generators exit when set to 1 */
    uint8_t rp;                 /**< read position on circular buffer */
    uint8_t wp;                 /**< write position on circular buffer */
    uint8_t size[MAX_DATA];     /**< size of the solution written to the buffer, not be larger than 8 */
    edge_t data[MAX_DATA][MAX_SOLUTION_SIZE]; /**< array containing the solutions */
} circ_buf_t;

/**
 * @brief Prints error message and exits with EXIT_FAILURE
 * @details Does not check anything and simply prints the error message
 * and exits with EXIT_FAILURE
 * @param msg Message to print
 * @param p Program name
 **/
void error_exit(char *p, char *msg);
#endif /* COMMON_H__ */