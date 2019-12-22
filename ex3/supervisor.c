/**
 * @file supervisor.c
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date
 */ 
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>  
#include <fcntl.h> 
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "common.h"

static char *prog;
static struct circ_buf *buf;
static int shmfd = -1;
static sem_t *free_sem, *used_sem;
volatile sig_atomic_t quit = 0;

/**
 * @brief
 * @details
 * @param solution
 * @param size
 * @return void
 */
static void print_solution(struct edge *s, int size)
{
    printf("[%s] Solution with %d edges: ", prog, size);
    for (int i = 0; i < size; i++){
        printf("%d-%d ", s[i].u, s[i].v);
    }
    printf("\n");
}

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void handle_signal(int s)
{
    quit = 1;
    buf->quit = 1;
}

/**
 * @brief
 * @details
 * @param
 * @return
 */
static void usage(void)
{
    fprintf(stderr, "USAGE: %s\n", prog);
    exit(EXIT_FAILURE);
}

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

    if (ftruncate(shmfd, sizeof(struct circ_buf)) < 0)
        exit_error(prog, "ftruncate failed");

    buf = mmap(NULL, sizeof(*buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (buf == MAP_FAILED) 
        exit_error(prog, "mmap failed");

    free_sem = sem_open(FREE_SEM, O_CREAT | O_EXCL, 0600, MAX_DATA);
    used_sem = sem_open(USED_SEM, O_CREAT | O_EXCL, 0600, 0);
    
    
    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED) 
        exit_error(prog, "free_sem failed");
     
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED) 
        exit_error(prog, "used_sem failed");



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
        if (munmap(buf, sizeof(*buf)) == -1) 
            exit_error(prog, "munmap failed");

        if (close(shmfd) == -1) 
            exit_error(prog, "close failed");

        if (shm_unlink(SHM_NAME) == -1) 
            exit_error(prog, "shm_unlink failed");

        if (sem_close(free_sem) == -1)  
            exit_error(prog, "sem_close failed");

        if (sem_close(used_sem) == -1)  
            exit_error(prog, "sem_close failed");

        if (sem_unlink(FREE_SEM) == -1) 
            exit_error(prog, "sem_unlink failed");

        if (sem_unlink(USED_SEM) == -1) 
            exit_error(prog, "sem_uknlink failed");
    }
}

/**
 * @brief
 * @details
 * @param
 * @return EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
    prog = argv[0];

    if (argc != 1)
        usage();

    if (atexit(free_resources) != 0)
        exit_error(prog, "resources not freed");
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    allocate_resources();

    buf->rp   = 0;
    buf->wp   = 0;
    buf->quit = 0;

    struct edge solution[MAX_SOLUTION_SIZE];
    if (solution == NULL) exit_error(prog, "malloc failed");

    int solution_size, min_solution = INT_MAX;
    while(!quit){

        if (sem_wait(used_sem) == -1){
            if (errno == EINTR) 
                continue;
            exit_error(prog, "something happended");
        }
            
        solution_size = buf->size[buf->rp];

        if (solution_size < min_solution){
            if (solution_size == 0){
                printf("[%s] graph is acyclic!\n", prog);

            } else {
                memcpy(solution, buf->data[buf->rp], solution_size * sizeof(struct edge));
                print_solution(solution, solution_size);

            }
            min_solution = solution_size;
        } 
        
        sem_post(free_sem);
        buf->rp = (buf->rp + 1) % MAX_DATA;
    }

    return EXIT_SUCCESS;
}
