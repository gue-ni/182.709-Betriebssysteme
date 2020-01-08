/**
 * @file generator.c
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date
 * 
 * @brief Generator Program
 */ 
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>  
#include <fcntl.h> 
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "common.h"

static char *prog = "not set";           /**< Name of program  */
static edge_t *edges;                    /**< Holds the input edges */
static int shmfd = -1;                   /**< File descriptor of the shared memory */
static circ_buf_t *buf  = MAP_FAILED;    /**< The Circular Buffer */
static sem_t *free_sem  = SEM_FAILED;    /**< Semaphore to coordinate write access for generator */
static sem_t *used_sem  = SEM_FAILED;    /**< Semaphore to coordinate read access for supervisor */
static sem_t *mutex     = SEM_FAILED;    /**< Mutex seamaphore to control access to the circular buffer */

/** 
 * @brief Print usage of ./generator
 * @details Prints the usage and exits with EXIT_FAILURE
 * @param void
 */
static void usage(void)
{
    fprintf(stderr, "USAGE: %s 0-1 1-2 2-0\n", prog);
    exit(EXIT_FAILURE);
}

/**
 * @brief Allocate and open shared memory and semaphores
 * @details
 * @param void
 */
static void allocate_resources(void)
{
    if ((shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600)) == -1)
        error_exit(prog, "shm_open failed");

    buf = mmap(NULL, sizeof(*buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (buf == MAP_FAILED) error_exit(prog, "mmap failed");

    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED)
        error_exit(prog, "sem_open (free_sem) failed");
  
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED)
        error_exit(prog, "sem_open (used_sem) failed");
   
    mutex = sem_open(MUTEX, 0);
    if (mutex == SEM_FAILED)
        error_exit(prog, "sem_open (mutex) failed");
}

/**
 * @brief Free and close shared memory and semaphores
 * @details
 * @param void
 */
static void free_resources(void)
{
    if (shmfd != -1){
        close(shmfd);
        shmfd = -1;
    }

    if (buf != MAP_FAILED){
        if (munmap(buf, sizeof(*buf)) == -1) 
            error_exit(prog, "munmap failed");
        buf = MAP_FAILED;
    }

    if (free_sem != SEM_FAILED){
        sem_close(free_sem);
        free_sem = SEM_FAILED;
    }

    if (used_sem != SEM_FAILED){
        sem_close(used_sem);
        used_sem = SEM_FAILED;
    }

    if (mutex != SEM_FAILED){
        sem_close(mutex);
//        sem_unlink(MUTEX);
        mutex = SEM_FAILED;
    }
}

/**
 * @brief Calculate max value
 * @details Calculates and returns the maximum of two values
 * @param x Value
 * @param y Value
 * @return Max value of x and y
 */
static int max(int x, int y)
{ 
    return x > y ? x : y; 
}

/**
 * @brief Parse an edge from a string
 * @details Parses an edge from a string and safes the max value of 
 * the vertice to m to calculate to number of vertices
 * @param edge Pointer to save the parsed edges
 * @param buf String that contains the edge 
 * @param m 
 */
static void parse_edge(edge_t *edge, char *buf, int *m)
{
    char *endptr;
    edge->u = strtol(buf, &endptr, 10);
    edge->v = abs(strtol(endptr, NULL, 10));
    *m = max( max( edge->u, edge->v ), *m ); 
}


/** Shuffle vertices with Fisher-Yates algorithm
 * @brief Shuffle vertices
 * @details Implements the Fisher-Yates shuffle algorithm
 * @param a Array to store shuffled data
 * @param n Size of array
 */
static void fisher_yates(int *a, int *l, int n)
{
    int j = 0;
    for (int i = 0; i < n; i++){
        j = rand() % (i+1);
        a[i] = a[j];
        a[j] = i;
    }
    for (int i = 0; i < n; i++){
        l[a[i]] = i;
    }
}

/** 
 * @brief Simple randomized algorithm for generating a feedback arc set
 * @details 
 * @param solution Array of edges that make up the possible solution
 * @param perm Permution of the vertices
 * @param n 
 * @return Size of solution, -1 if solution is larger than MAX_SOLUTION_SIZE
 */
static int monte_carlo(edge_t *solution, int *perm, int n)
{
    memset(solution, 0, sizeof(edge_t) * MAX_SOLUTION_SIZE);
    int size = 0;
    for (int i = 0; i < n; i++){

        if (size > MAX_SOLUTION_SIZE){
            return -1;
        }

        if (perm[ edges[i].u ] > perm[ edges[i].v ]){ // invalid write here
            solution[size++] = edges[i];
        }
    }
    return size;
}

/**
 * @brief Calculates Feeddback Arc Set
 * @details Implement an algorithm which removes cycles in a directed graph by removing 
 * the least edges possible. Whenever a smaller solution is found it is written to 
 * the shared memory
 * @param argc Argument count
 * @param argv  Argument vektor
 * @return EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
    prog = argv[0];
    if (argc == 1){
        usage();
    }

    if (atexit(free_resources) != 0) 
        error_exit(prog, "resources not freed");

    allocate_resources();
    edge_t solution[MAX_SOLUTION_SIZE];

    int nv = 0, ne = argc - 1;
    edges = malloc(sizeof(edge_t) * ne);
    if (edges == NULL) 
        error_exit(prog, "malloc failed");
    
    for (int i = 1; i < argc; i++){
        parse_edge(edges+(i-1), argv[i], &nv);
    }
    nv++;

    if (nv > 255 || ne > 255) 
        error_exit(prog, "too many vertices or edges");

    int *lookup         = malloc(sizeof(int) * nv);
    int *permutation    = malloc(sizeof(int) * nv);

    if (permutation == NULL) 
        error_exit(prog, "malloc failed");

    if (lookup == NULL) 
        error_exit(prog, "malloc failed");
    
    srand(getpid()); // seed random number generator

    int size = 0, min_solution = INT_MAX;

    while (!buf->quit){

        fisher_yates(permutation, lookup, nv);
        size = monte_carlo(solution, lookup, ne); 

        if (size == -1) 
            continue; // solution is too large anyway

        if (size < min_solution && size <= MAX_SOLUTION_SIZE){ // <= would cause lockup
            min_solution = size;
            
            sem_wait(mutex);
            sem_wait(free_sem);

            if (size > 0)
                memcpy(buf->data[buf->wp], solution, sizeof(edge_t) * size);

            buf->size[buf->wp] = size;

            sem_post(used_sem);
            buf->wp = (buf->wp + 1) % MAX_DATA;
            sem_post(mutex);
        }
    }
    
    free(permutation);
    free(edges);
    free(lookup);
    return EXIT_SUCCESS;
}
