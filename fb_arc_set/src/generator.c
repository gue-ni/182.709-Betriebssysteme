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

int *perm; 
char *prog;
struct circ_buf *buf;
static int shmfd = -1;
struct edge *E, *solution;
sem_t *free_sem, *used_sem, *mutex;

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
//    fprintf(stdout, "[%s] free resources\n", prog);
    free(E);
    free(perm);
    free(solution);
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
static int max(int x, int y){ return x > y ? x : y; }

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void parse_edge(struct edge *e, char *buf, int *m)
{
    char *endptr;
    e->u = strtol(buf, &endptr, 10);
    e->v = abs(strtol(endptr, NULL, 10));

    int local_max = max(e->u, e->v);
    *m = max(local_max, *m);
}

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void fisher_yates(int *a, int n)
{
    int j = 0;
    for (int i = 0; i < n; i++){
        j = rand() % (i + 1);
        a[i] = a[j];
        a[j] = i;
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
        if (size >= MAX_SOLUTION_SIZE){
            return -1;
        }

        if (perm[ E[i].u ] > perm[ E[i].v ]){ // invalid write here
            solution[size++] = E[i];
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
        exit_error(prog, "resources not freed");

    allocate_resources();

    int nV = 0, nE = argc - 1;
    E = malloc(sizeof(struct edge) * nE);
    if (E == NULL) 
        exit_error(prog, "malloc failed");
    
    for (int i = 1; i < argc; ++i){
        parse_edge(E+(i-1), argv[i], &nV);
    }
    nV++;
//    printf("number of vertices: %d, number of edges: %d\n", nV, nE);

    perm  = malloc(sizeof(int) * nV);
    if (perm == NULL) 
        exit_error(prog, "malloc failed");

    solution = malloc(sizeof(struct edge) * MAX_SOLUTION_SIZE);
    if (solution == NULL) 
        exit_error(prog, "malloc failed"); 
    
    int size = 0, min_solution = INT_MAX;
    while (!buf->quit){
    
        fisher_yates(perm, nV);
        size = monte_carlo(solution, perm, nE); 
        if (size == -1) // solution is too large anyway
            continue;       

        if (size < min_solution && size <= MAX_SOLUTION_SIZE){ // <= causes lockup
           
            min_solution = size;
            
            sem_wait(mutex);
            sem_wait(free_sem);
 
            if (size > 0){
                memcpy(buf->data[buf->write_pos], solution, sizeof(struct edge) * size);
            }
            
            buf->solution_size[buf->write_pos] = size;
            sem_post(used_sem);
            buf->write_pos = (buf->write_pos + 1) % MAX_DATA;
            sem_post(mutex);

        }
    }
    return EXIT_SUCCESS;
}

