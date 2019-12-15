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
struct shared_mem *circ_buf;
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
    quit = 1;
    write(0, "handle signal\n", 14);
}

void free_resources(void)
{
    write_message("free resources\n");
    if (shmfd != -1){
        if (munmap(circ_buf, sizeof(*circ_buf)) == -1)
            exit_error("munmap failed");

        close(shmfd);
        shm_unlink(SHM_NAME);
        sem_close(free_sem);
        sem_close(used_sem);
        sem_unlink(FREE_SEM);
        sem_unlink(USED_SEM);

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

    if (ftruncate(shmfd, sizeof(struct shared_mem)) < 0)
        exit_error("ftruncate failed");

    circ_buf = mmap(NULL, sizeof(*circ_buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (circ_buf == MAP_FAILED) exit_error("mmap failed");

    free_sem = sem_open(FREE_SEM, O_CREAT | O_EXCL, 0600, MAX_DATA);
    used_sem = sem_open(USED_SEM, O_CREAT | O_EXCL, 0600, 0);
    
    free_sem = sem_open(FREE_SEM, 0);
    if (free_sem == SEM_FAILED)
        exit_error("free_sem failed");
  
    used_sem = sem_open(USED_SEM, 0);
    if (used_sem == SEM_FAILED)
        exit_error("used_sem failed");
 


//    fprintf(stderr, "free_sem: %d, used_sem: %d\n", *free_sem, *used_sem);

    circ_buf->read_pos = 0;
    circ_buf->write_pos = 0;

    int value;
    while(!quit){
        fprintf(stderr, "waiting to read...\n");

        if (sem_wait(used_sem) == -1){
            if (errno == EINTR)
                continue;
            exit_error("something happended");
        }
        value = circ_buf->data[circ_buf->read_pos];
        sem_post(free_sem);
        circ_buf->read_pos = (circ_buf->read_pos + 1) % MAX_DATA;

        write_message("read something");
        print_result(value);

        

    }
    return EXIT_SUCCESS;
}