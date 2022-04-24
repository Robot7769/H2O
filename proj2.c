/**
 * @file proj2.c
 * @author Jan Škrabal (xskrab12)-FIT
 * @brief IOS projekt 2 synchronizace (Building H2O)
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>


#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "error.h"

#include "proj2.h"
#include "funkce.c"

  
int main(int argc, char const *argv[]) {
    if (argc != ARG_COUNT) {
        error_exit("Nesprávný počet argumentů!\n");
    }
    for (int i = 1; i < ARG_COUNT; i++) { // validace že jsou všechny zadané paramentry čísla
        if (is_number(argv[i])) {
            error_exit("Argument %d není celé kladné číslo\n", i);
        }
    }
    shmem_t *mem = mmap(NULL, sizeof(shmem_t),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS ,-1,0);
    if (mem == MAP_FAILED) {
        error_exit("Nepodařilo se vytnořit sdílenou paměť\n");
    }
    mem->index_o = 0;
    mem->index_h = 0;
    mem->out_o = 0;
    mem->out_h = 0;
    mem->molecule = 0;
    sems_t sems;
    sems.mutex = sem_open("/ios_proj2_H2O_mutex", O_CREAT | O_EXCL, 0666, 1);
    sems.queue_o = sem_open("/ios_proj2_H2O_queue_o", O_CREAT | O_EXCL, 0666, 0);
    sems.queue_h = sem_open("/ios_proj2_H2O_queue_h", O_CREAT | O_EXCL, 0666, 0);
    sems.barrier = sem_open("/ios_proj2_H2O_barrier", O_CREAT | O_EXCL, 0666, 0);
    sems.print = sem_open("/ios_proj2_H2O_print", O_CREAT | O_EXCL, 0666, 1);
    if ((sems.mutex == SEM_FAILED) || (sems.queue_o == SEM_FAILED) || (sems.queue_h == SEM_FAILED) || (sems.barrier == SEM_FAILED) || (sems.print == SEM_FAILED)) {
        munmap(mem, sizeof(shmem_t));
        error_exit("Semafor se neotevřel\n");
    }
    

        
    //argument 1 počet kyslíků
    mem->count_o = atoi(argv[1]);
    
    //argument 2 počet vodíků
    mem->count_h = atoi(argv[2]);

    //argument 3 čas čekání na zařazení do fronty
    mem->time_i = atoi(argv[3]);
    if (mem->time_i > TIME_MAX) {
        munmap(mem, sizeof(shmem_t));
        error_exit("TI není v rozsaho 0<=TI<=1000\n");
    }

    //argument 4 čas vytváření molekul
    mem->time_b = atoi(argv[4]);
    if (mem->time_b > TIME_MAX) {
        munmap(mem, sizeof(shmem_t));
        error_exit("TB není v rozsaho 0<=TB<=1000\n");
    }

    FILE *output = fopen("proj.out","w");
    if (output == NULL) {
        munmap(mem, sizeof(shmem_t));
        error_exit("Nepodařilo se otevřít soubor pro zápis\n");
    }

    
    //pid_t process[mem->count_o + mem->count_h];
    for (size_t i = 0; i < (mem->count_o + mem->count_h); i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i < mem->count_o) {
                //funkce kyslik
                printf("začátek kyslíku %d  ", pid);
                oxygen(sems, mem, output);

                printf("konec kyslíku %d\n", pid);
                exit(0);
            } else {
                //funkce vodik
                printf("začátek vodíku %d  ", pid);

                hydrogen(sems, mem, output);

                printf("konec vodíku %d\n", pid);
                exit(0);       
            }
            
        }
    }
    usleep(1005);
    printf("stop\n");
    


    //printf("done! %ld %ld %ld %ld\n", mem->count_o, mem->count_h, mem->time_i,mem->time_b);
    sem_unlink("/ios_proj2_H2O_mutex");
    sem_close(sems.mutex);
    sem_unlink("/ios_proj2_H2O_queue_o");
    sem_close(sems.queue_o);
    sem_unlink("/ios_proj2_H2O_queue_h");
    sem_close(sems.queue_h);
    sem_unlink("/ios_proj2_H2O_barrier");
    sem_close(sems.barrier);
    sem_unlink("/ios_proj2_H2O_print");
    sem_close(sems.print);
    munmap(mem, sizeof(shmem_t));
    return 0;
}


