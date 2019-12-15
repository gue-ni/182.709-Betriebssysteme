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
#include <signal.h>
#include "util.h"

static int shmfd = -1;
sem_t *free_sem, *used_sem;
volatile sig_atomic_t quit = 0;
struct circ_buf *cbuf;
char *prog;

void print_result(int value)
{
    printf("[%s] %d\n", prog, value);
}
void write_message(char *msg)
{
    printf("[%s] %s\n", prog, msg);
}

void exit_error(char *msg)
{
    fprintf(stderr, "%s %s\n", prog, msg);
    exit(EXIT_FAILURE);
}


int buf_read(int *buf, int *read_pos)
{
    sem_wait(used_sem);
    int val = buf[*read_pos];
    sem_post(free_sem);
    *read_pos = (*read_pos + 1) % MAX_DATA;
    fprintf(stderr, "read value: %d", val);
    return val;
}

void handle_signal(int s)
{
    write(0, "handle signal\n", 14);
    quit = 1;
}

void free_resources(void)
{
    if (shmfd != -1){
        write_message("free resources\n");
        if (munmap(cbuf, sizeof(*cbuf)) == -1) exit_error("munmap failed");
        if (close(shmfd) == -1) exit_error("close failed");
        if (shm_unlink(SHM_NAME) == -1) exit_error("shm_unlink failed");
        if (sem_close(free_sem) == -1) exit_error("sem_close failed");
        if (sem_close(used_sem) == -1) exit_error("sem_close failed");
        if (sem_unlink(FREE_SEM) == -1) exit_error("sem_unlink failed");
        if (sem_unlink(USED_SEM) == -1) exit_error("sem_uknlink failed");
    }
}

int main(int argc, char *argv[])
{
    prog = argv[0];
    if (atexit(free_resources) != 0)
        exit_error("resources not freed");
    
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);

    if ((shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600)) == -1)
        exit_error("shm_open failed");

    if (ftruncate(shmfd, sizeof(struct circ_buf)) < 0)
        exit_error("ftruncate failed");

    cbuf = mmap(NULL, sizeof(*cbuf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (cbuf == MAP_FAILED) exit_error("mmap failed");

    free_sem = sem_open(FREE_SEM, O_CREAT | O_EXCL, 0600, MAX_DATA);
    used_sem = sem_open(USED_SEM, O_CREAT | O_EXCL, 0600, 0);
    
    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED)
        exit_error("free_sem failed");
  
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED)
        exit_error("used_sem failed");

    cbuf->read_pos = 0;
    cbuf->write_pos = 0;

    int value;
    while(!quit){
        fprintf(stderr, "waiting to read...\n");

        if (sem_wait(used_sem) == -1){
            exit_error("something happended");
            if (errno == EINTR){
                fprintf(stderr, "[%s] sem_wait was interrupted\n", prog);
                continue;
            }
        }
        write_message("reading...");
        value = cbuf->data[cbuf->read_pos];
        sem_post(free_sem);
        cbuf->read_pos = (cbuf->read_pos + 1) % MAX_DATA;

        write_message("read something");
        print_result(value);
    
    }

    return EXIT_SUCCESS;
}