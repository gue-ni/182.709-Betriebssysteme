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
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

static char *prog; /** < */
static struct circ_buf *buf; /** < */
static int shmfd = -1;/** < */
static struct edge *edges;/** < */
static sem_t *free_sem, *used_sem, *mutex; /** < */

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void allocate_resources(void)
{
    if ((shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600)) == -1)
        exit_error(prog, "shm_open failed");

    buf = mmap(NULL, sizeof(*buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (buf == MAP_FAILED) exit_error(prog, "mmap failed");

    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED)
        exit_error(prog, "free_sem failed");
  
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED)
        exit_error(prog, "used_sem failed");
   
    mutex = sem_open(MUTEX, O_CREAT, 0600, 1);
    if (mutex == SEM_FAILED)
        exit_error(prog, "mutex failed");
}

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void free_resources(void)
{
    //fprintf(stdout, "[%s] free resources\n", prog);
    if (munmap(buf, sizeof(*buf)) == -1) 
        exit_error(prog, "munmap failed");

    if (close(shmfd) == -1) 
        exit_error(prog, "close failed");

    if (sem_close(free_sem) == -1) 
        exit_error(prog, "sem_close failed");

    if (sem_close(mutex) == -1) 
        exit_error(prog, "sem_close failed");

    if (sem_close(used_sem) == -1) 
        exit_error(prog, "sem_close failed");

    sem_unlink(MUTEX);
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
static void parse_edge(struct edge *edge, char *buf, int *m)
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
static int monte_carlo(struct edge *solution, int *perm, int n)
{
    memset(solution, 0, sizeof(struct edge) * MAX_SOLUTION_SIZE);
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

    if (atexit(free_resources) != 0) exit_error(prog, "resources not freed");

    allocate_resources();
    struct edge solution[MAX_SOLUTION_SIZE];

    int nV = 0, nE = argc - 1;
    edges = malloc(sizeof(struct edge) * nE);
    if (edges == NULL) exit_error(prog, "malloc failed");
    
    for (int i = 1; i < argc; i++){
        parse_edge(edges+(i-1), argv[i], &nV);
    }
    nV++;

    if (nV > 255 || nE > 255) 
        exit_error(prog, "too many vertices or edges");

    int *lookup = malloc(sizeof(int) * nV);
    int *perm   = malloc(sizeof(int) * nV);

    if (perm == NULL) exit_error(prog, "malloc failed");
    if (lookup == NULL) exit_error(prog, "malloc failed");
    
    int size = 0, min_solution = INT_MAX;
    while (!buf->quit){

        fisher_yates(perm, lookup, nV);
        size = monte_carlo(solution, lookup, nE); 

        if (size == -1) 
            continue; // solution is too large anyway

        if (size < min_solution && size <= MAX_SOLUTION_SIZE){ // <= causes lockup
            min_solution = size;
            
            sem_wait(mutex);
            sem_wait(free_sem);

            if (size > 0){
                memcpy(buf->data[buf->wp], solution, sizeof(struct edge) * size);
            }

            buf->size[buf->wp] = size;

            sem_post(used_sem);
            buf->wp = (buf->wp + 1) % MAX_DATA;
            sem_post(mutex);
        }
    }
    
    free(perm);
    free(edges);
    free(lookup);
    return EXIT_SUCCESS;
}
