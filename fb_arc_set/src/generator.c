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
char *prog;

void write_message(char *msg)
{
    printf("[%s] %s\n", prog, msg);
}

void exit_error(char *msg)
{
    fprintf(stderr, "%s %s\n", prog, msg);
    exit(EXIT_FAILURE);
}

void free_resources(void)
{
    write_message("free resources");
    munmap(cbuf, sizeof(*cbuf));
    close(shmfd);
    sem_close(mutex);
    sem_close(free_sem);
    sem_close(used_sem);
    sem_unlink(MUTEX);
}

int main(int argc, char *argv[])
{
    prog = argv[0];
    if (atexit(free_resources) != 0)
        exit_error("resources not freed");

    if ((shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600)) == -1)
        exit_error("shm_open failed");

    struct circ_buf *c_buf;
    c_buf = mmap(NULL, sizeof(*c_buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (c_buf == MAP_FAILED) exit_error("mmap failed");

    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED)
        exit_error("free_sem failed");
  
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED)
        exit_error("used_sem failed");
   
    mutex = sem_open(MUTEX, O_CREAT, 0600, 0);
    if (mutex == SEM_FAILED)
        exit_error("mutex failed");
   
    /* write */
    int value = 1;
    sem_wait(free_sem);
    c_buf->data[c_buf->write_pos] = value;
    sem_post(used_sem);
    c_buf->write_pos = (c_buf->write_pos + 1) % MAX_DATA;
    write_message("wrote something");
    return EXIT_SUCCESS;
}

