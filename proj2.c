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

void* oxygen(shmem_t *mem, FILE *output);

void* hydrogen(shmem_t *mem, FILE *output);
  
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
    
    sem_init(&mutex ,0 ,1);
    sem_init(&queue_o ,0 ,0);
    sem_init(&queue_h ,0 ,0);
    sem_init(&barier ,0 ,3);
    sem_init(&print ,0 ,1);
        
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
        error_exit("Nepodařilo se otevřít soubor pro zápis");
    }


    /**
    pid_t kyslik = 0;
    if (kyslik == 0) {
        //main
        printf("START\n");
        for (size_t i = 0; i < 2; i++) {
            kyslik = fork();
        }
  
        
    } else {
        //funkce kyslik
        printf("začátek kyslíku %d\n", kyslik);
        mem->count_o--;
        usleep(rand() % 500);
        
        printf("konec kyslíku %d\n", kyslik);
        exit(0);
    }
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
    */
    pthread_t t1,t2, t3;
    pthread_create(&t1,NULL,hydrogen(mem,output),NULL);
    pthread_create(&t2,NULL,hydrogen(mem,output),NULL);
    pthread_create(&t3,NULL,oxygen(mem,output),NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);


    printf("\ndone! %ld %ld %ld %ld\n", mem->count_o, mem->count_h, mem->time_i,mem->time_b);
    sem_destroy(&mutex);
    sem_destroy(&queue_o);
    sem_destroy(&queue_h);
    sem_destroy(&barier);
    sem_destroy(&print);
    munmap(mem, sizeof(shmem_t));
    return 0;
}


void print_out(shmem_t *mem, FILE *output,const char *fmt, ...) {
    sem_wait(&print);
    mem->id +=1;
    va_list(argumenty);
    va_start(argumenty,fmt);
    fprintf(output, "%ld: ", mem->id);
    fprintf(stdout, "%ld: ", mem->id); //! test
    vfprintf(output,fmt,argumenty);
    vfprintf(stdout,fmt,argumenty); //! test
    va_end(argumenty);
    sem_post(&print);
}

void* oxygen(shmem_t *mem, FILE *output) {
    sem_wait(&mutex);
    mem->index_o +=1;       // ID kyslíku
    print_out(mem, output,"O %d: started\n", mem->index_o);
    if (mem->time_i != 0) {
        usleep(rand() % mem->time_i + 1);
    }
    mem->out_o +=1;
    print_out(mem, output,"O %d: going to queue\n", mem->index_o);
    if (mem->out_h >= 2) {
        mem->molecule +=1;
        print_out(mem, output,"O %d: creating molecule %d\n", mem->index_o, mem->molecule);
        if (mem->time_b != 0) {
            usleep(rand() % mem->time_b + 1);
        }
        sem_post(&queue_h);
        sem_post(&queue_h);
        mem->out_h -= 2;
        sem_post(&queue_o);
        mem->out_o -= 1;
        print_out(mem, output,"O %d:molecule %d created\n", mem->index_o, mem->molecule);
    } else {
        sem_post(&mutex);
    }
    sem_wait(&queue_o);
    //! bont
    
    sem_wait(&barier);
    sem_post(&mutex);
}

void* hydrogen(shmem_t *mem, FILE *output) {
    sem_wait(&mutex);
    mem->index_h +=1;       // ID vodíku
    print_out(mem, output,"H %d: started\n", mem->index_h);
    if (mem->time_i != 0) {
        usleep(rand() % mem->time_i + 1);
    }
    mem->out_h +=1;
    print_out(mem, output,"H %d: going to queue\n", mem->index_h);
    if (mem->out_h >= 2 && mem->count_o >= 1) {
        mem->molecule +=1;
        print_out(mem, output,"H %d: creating molecule %d\n", mem->index_h, mem->molecule);
        if (mem->time_b != 0) {
            usleep(rand() % mem->time_b + 1);
        }
        sem_post(&queue_h);
        sem_post(&queue_h);
        mem->out_h -= 2;
        sem_post(&queue_o);
        mem->out_o -= 1;
        print_out(mem, output,"H %d:molecule %d created\n", mem->index_h, mem->molecule);
    } else {
        sem_post(&mutex);
    }
    sem_wait(&queue_h);
    //! bont
    
    sem_wait(&barier);
}
