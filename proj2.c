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

    pid_t kyslik = fork();
    if (kyslik == 0) {
        //main
            printf("START\n");
  
        pid_t vodik = fork();
        if (vodik) {
            //funkce vodik
            printf("začátek vodíku %d\n", vodik);
            mem->count_h--;
            usleep(rand() % 500);
            printf("konec vodíku %d\n", vodik);
            exit(0);
        }
        usleep(1005);
        printf("stop\n");
    } else {
        //funkce kyslik
        printf("začátek kyslíku %d\n", kyslik);
        mem->count_o--;
        usleep(rand() % 500);
        
        printf("konec kyslíku %d\n", kyslik);
        exit(0);
    }
    

    printf("\ndone! %ld %ld %ld %ld\n", mem->count_o, mem->count_h, mem->time_i,mem->time_b);
    munmap(mem, sizeof(shmem_t));
    return 0;
}
