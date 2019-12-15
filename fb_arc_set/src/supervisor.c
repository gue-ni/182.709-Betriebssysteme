#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>  
#include <fcntl.h> 
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

static int shmfd = -1;

void exit_error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

    
    

    


void free_resources(void)
{
    if (shmfd != -1){
        if (shm_unlink(SHM_NAME) == -1)
            exit_error("shm_unlink failed");
        fprintf(stdout, "closing shmfd\n");
    }
}

int main(int argc, char *argv[])
{
    if (atexit(free_resources) != 0)
        exit_error("resources not freed");

    if ((shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600)) == -1)
        exit_error("shm_open failed");

    if (ftruncate(shmfd, sizeof(struct shared_mem)) < 0)
        exit_error("ftruncate failed");


    struct shared_mem *circ_buf;
    circ_buf = mmap(NULL, sizeof(*circ_buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (circ_buf == MAP_FAILED)
        exit_error("mmap failed");

    if (munmap(circ_buf, sizeof(*circ_buf)) == -1)
        exit_error("munmap failed");


    return 0;
}