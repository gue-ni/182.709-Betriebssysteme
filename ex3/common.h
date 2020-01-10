/**
 * @file common.h
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 10.01.2020 
 * 
 * @brief Defines various structs and constants for use in generator
 * and supervisor
 */
#ifndef COMMON_H__
#define COMMON_H__

#include <signal.h>
#include <stdint.h>

#define SHM_NAME            "/11809618_shared_mem"
#define FREE_SEM            "/11809618_fb_free"
#define USED_SEM            "/11809618_fb_used"
#define MUTEX               "/11809618_fb_mutex"
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
 * Circular buffer where generators report their solutions to 
 * the supervisor. Writing and reading positions are stored in 
 * the rp and wp variables. The quit variable is set to 1 when the 
 * supervisor  receives SIGINT or SIGTERM and signals to the 
 * generators when to quit and free all resources. 
 */
typedef struct circ_buf {
    volatile sig_atomic_t quit; /**<  quit: generators and supervisor exit when set to 1 */
    uint8_t rp;                 /**< read position on circular buffer */
    uint8_t wp;                 /**< write position on circular buffer */
    uint8_t size[MAX_DATA];     /**< size of the solution written to the buffer, not be larger than 8 */
    edge_t data[MAX_DATA][MAX_SOLUTION_SIZE]; /**< array containing the solution arrays */
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