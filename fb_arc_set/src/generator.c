#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>  
#include <fcntl.h> 
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "util.h"

static int shmfd = -1;
sem_t *free_sem, *used_sem, *mutex;
struct circ_buf *cbuf;
struct edge *E;
char *prog;

void write_message(char *msg)
{
    printf("[%s] %s\n", prog, msg);
}

void print_solution(struct edge *sol, int solsize)
{
    printf("[%s] solution with %d edges: ", prog, solsize);
    for (int i = 0; i < solsize; i++){
        printf("%d-%d ", sol[i].u, sol[i].v);
    }
    printf("\n");


}

void exit_error(char *msg)
{
    fprintf(stderr, "%s %s\n", prog, msg);
    exit(EXIT_FAILURE);
}

void allocate_resources(void)
{
    if ((shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600)) == -1)
        exit_error("shm_open failed");

    cbuf = mmap(NULL, sizeof(*cbuf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (cbuf == MAP_FAILED) exit_error("mmap failed");

    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED)
        exit_error("free_sem failed");
  
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED)
        exit_error("used_sem failed");
   
    mutex = sem_open(MUTEX, O_CREAT, 0600, 1);
    if (mutex == SEM_FAILED)
        exit_error("mutex failed");
 
}

void free_resources(void)
{
    fprintf(stdout, "[%s] free resources\n", prog);
    free(E);
    if (munmap(cbuf, sizeof(*cbuf)) == -1) exit_error("munmap failed");
    if (close(shmfd) == -1) exit_error("close failed");
    if (sem_close(free_sem) == -1) exit_error("sem_close failed");
    if (sem_close(mutex) == -1) exit_error("sem_close failed");
    if (sem_close(used_sem) == -1) exit_error("sem_close failed");
    //if (sem_unlink(MUTEX) == -1) exit_error("sem_unlink failed");
    sem_unlink(MUTEX);

}

int max(int x, int y)
{
    return x > y ? x : y;
}

void parse_edge(struct edge *E, char *buf, int *m)
{
    char *endptr;
    E->u = strtol(buf, &endptr, 10);
    E->v = abs(strtol(endptr, NULL, 10));
    int local_max = max(E->u, E->v);
    *m = *m < local_max ? local_max : *m;  
    //fprintf(stdout, "[%s] converted %d-%d\n", prog, E->u, E->v);

}

void gen_lookup(int *a, int *b, int n)
{
    for (int i = 0; i < n; i++){
        a[b[i]] = i;
    }
}

void fisher_yates_shuffle(int *a, int n)
{
    int j = 0;
    int *tmp = malloc(sizeof(int) * n);
    int *source = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++){
        source[i] = i;
    }

    for (int i = 0; i < n; i++){
        j = rand() % (i+1);
        if (j != i)
            tmp[i] = tmp[j];
        tmp[j] = source[i];
    }

    gen_lookup(a, tmp, n);
    free(tmp);
    free(source);
}

int monte_carlo(struct edge *sol, int *perm, int nE)
{
    memset(sol, 0, sizeof(struct edge) * MAX_SOLUTION_SIZE);
    int solsize = 0;
    for (int i = 0; i < nE; i++){
        if (perm[E[i].u] > perm[E[i].v]){
            sol[solsize++] = E[i];
        }
    }
    return solsize;
}

int main(int argc, char *argv[])
{
    prog = argv[0];
    if (atexit(free_resources) != 0)
        exit_error("resources not freed");

    allocate_resources();

    int nV = 0, nE = argc - 1;
    E = malloc(sizeof(struct edge) * nE);
    
    for (int i = 1; i < argc; ++i){
        parse_edge(E+(i-1), argv[i], &nV);
    }
    nV++;

    int *perm = malloc(sizeof(int) * nV);
    struct edge *sol = malloc(sizeof(struct edge) * MAX_SOLUTION_SIZE);
    
    int solsize = 0, min_solution = __INT_MAX__;
    while (!cbuf->quit){
    
        fisher_yates_shuffle(perm, nV);
        solsize = monte_carlo(sol, perm, nE);        
//        printf("[%s] calculating...\n", prog); 

        if (solsize < min_solution){ // <= causes lockup
            min_solution = solsize;
            sem_wait(mutex);
            
            sem_wait(free_sem);
//            print_solution(sol, solsize);
 
            memcpy(cbuf->data[cbuf->write_pos], sol, sizeof(struct edge) * solsize);
            cbuf->solution_size[cbuf->write_pos] = solsize;

            sem_post(used_sem);
            cbuf->write_pos = (cbuf->write_pos + 1) % MAX_DATA;
            sem_wait(free_sem);

            sem_post(mutex);

        }
    }
    return EXIT_SUCCESS;
}

