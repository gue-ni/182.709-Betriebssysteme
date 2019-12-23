/**
 * @file generator.c
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date
 */ 
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>  
#include <fcntl.h> 
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

static char *prog; /** < */
static edge_t *edges;/** < */
static int shmfd = -1;
static circ_buf_t *buf = MAP_FAILED;
static sem_t *free_sem = SEM_FAILED;
static sem_t *used_sem = SEM_FAILED;
static sem_t *mutex = SEM_FAILED; /** < */

/**
 * @brief
 * @details
 * @param
 * @return
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
   
    mutex = sem_open(MUTEX, O_CREAT, 0600, 1);
    if (mutex == SEM_FAILED)
        error_exit(prog, "sem_open (mutex) failed");
}

/**
 * @brief
 * @details
 * @param
 * @return
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
        sem_unlink(MUTEX);
        mutex = SEM_FAILED;
    }
}

/**
 * @brief
 * @details
 * @param
 * @return
 */
static int max(int x, int y)
{ 
    return x > y ? x : y; 
}

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void parse_edge(edge_t *edge, char *buf, int *m)
{
    char *endptr;
    edge->u = strtol(buf, &endptr, 10);
    edge->v = abs(strtol(endptr, NULL, 10));
    *m = max(max(edge->u, edge->v), *m); 
}


/** Shuffle vertices with Fisher-Yates algorithm
 * @brief shuffle vertices
 * @details implements the Fisher-Yates shuffle algorithm
 * @param a array to store shuffled data
 * @param n size of array
 * @return void
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
 * @brief
 * @details
 * @param
 * @return size of solution, -1 if solution is larger than MAX_SOLUTION_SIZE
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
 * @brief
 * @details
 * @param
 * @return
 */
int main(int argc, char *argv[])
{
    prog = argv[0];

    if (atexit(free_resources) != 0) 
        error_exit(prog, "resources not freed");

    allocate_resources();
    edge_t solution[MAX_SOLUTION_SIZE];

    int nV = 0, nE = argc - 1;
    edges = malloc(sizeof(edge_t) * nE);
    if (edges == NULL) error_exit(prog, "malloc failed");
    
    for (int i = 1; i < argc; i++){
        parse_edge(edges+(i-1), argv[i], &nV);
    }
    nV++;

    if (nV > 255 || nE > 255) 
        error_exit(prog, "too many vertices or edges");

    int *lookup = malloc(sizeof(int) * nV);
    int *permutation   = malloc(sizeof(int) * nV);

    if (permutation == NULL) 
        error_exit(prog, "malloc failed");

    if (lookup == NULL) 
        error_exit(prog, "malloc failed");
    
    srand(getpid()); // seed random number generator

    int size = 0, min_solution = INT_MAX;
    while (!buf->quit){

        fisher_yates(permutation, lookup, nV);
        size = monte_carlo(solution, lookup, nE); 

        if (size == -1) 
            continue; // solution is too large anyway

        if (size < min_solution && size <= MAX_SOLUTION_SIZE){ // <= causes lockup
            min_solution = size;
            
            sem_wait(mutex);
            sem_wait(free_sem);

            if (size > 0){
                memcpy(buf->data[buf->wp], solution, sizeof(edge_t) * size);
            }

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
