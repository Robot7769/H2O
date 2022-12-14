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
#include <time.h>

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

    memory_setup(mem);

    sems_t sems;
    
    if (sem_start(&sems)) {
        clean_seme_mem(&sems, mem);
        error_exit("Semafor se neotevřel\n");
    }
    

        
    //argument 1 počet kyslíků
    mem->count_o = atoi(argv[1]);
    
    //argument 2 počet vodíků
    mem->count_h = atoi(argv[2]);

    //argument 3 čas čekání na zařazení do fronty
    mem->time_i = atoi(argv[3]);
    if (mem->time_i > TIME_MAX) {
        clean_seme_mem(&sems, mem);
        error_exit("TI není v rozsaho 0<=TI<=1000\n");
    }

    //argument 4 čas vytváření molekul
    mem->time_b = atoi(argv[4]);
    if (mem->time_b > TIME_MAX) {
        clean_seme_mem(&sems, mem);
        error_exit("TB není v rozsaho 0<=TB<=1000\n");
    }

    FILE *output = fopen(OUTPUT_FILE,"w");
    if (output == NULL) {
        clean_seme_mem(&sems, mem);
        error_exit("Nepodařilo se otevřít soubor pro zápis\n");
    }

    int molekuls_max_conut;
    sem_wait(sems.mutex);
    mem->max_craete_molekules = mem->count_h/2;
    if (mem->max_craete_molekules > mem->count_o) {
        mem->max_craete_molekules = mem->count_o;
        molekuls_max_conut = mem->max_craete_molekules;
    }
    //printf("vyrobím jenom %ld molekul vody\n", mem->max_craete_molekules);
    mem->max_craete_molekules = 3*(mem->max_craete_molekules);
    sem_post(sems.mutex);
    
    pid_t process[mem->count_o + mem->count_h];
    for (size_t i = 0; i < (mem->count_o + mem->count_h); i++) {
        pid_t pid = fork();
        if (pid == 0) {
            //printf("vel: %ld  ",mem->count_o + mem->count_h);
            if (i < mem->count_o) {
                //! funkce kyslik
                //printf("začátek kyslíku %d  ", pid);
                oxygen(i+1,sems, mem, output);

                //printf("konec kyslíku %d\n", pid);
                sem_wait(sems.to_end);
                mem->max_craete_molekules--;
                //printf("to end nead %ld\n", mem->max_craete_molekules);
                if (mem->max_craete_molekules == 0) {
                    sem_post(sems.end);
                }
                sem_post(sems.to_end);
                exit(0);
            } else {
                //! funkce vodik
                //printf("začátek vodíku %d  ", pid);

                hydrogen(i-mem->count_o+1,sems, mem, output);

                //printf("konec vodíku %d\n", pid);
                sem_wait(sems.to_end);
                mem->max_craete_molekules--;
                //printf("to end nead %ld\n", mem->max_craete_molekules);
                if (mem->max_craete_molekules == 0) {
                    sem_post(sems.end);
                }
                sem_post(sems.to_end);
                exit(0);
            }
            
        } else if (pid < 0) {
            for (size_t j = 0; j < i; j++) {
                kill(process[j],SIGKILL);       //ukončuje předchozí procesy při chybě
            }
            
            clean_seme_mem(&sems, mem);
            fclose(output);
            error_exit("Nepodařilo se otevřít proces\n");
        } else {
            process[i] = pid;
        }

    }

    //printf("start wait\n");
    sem_wait(sems.end);
    
    
    
    //sem_wait(sems.mutex);
    //printf("stop\n");

    //printf("out_h %ld",mem->out_h);
    for (size_t i = (mem->count_h -2*mem->max_craete_molekules); i > 0; i--) {
        //printf("pouštím H %ld\n", i);
        sem_post(sems.queue_h);
        //sleep(1);
    }
    //printf("stop2\n");
    for (size_t i = mem->count_o - mem->max_craete_molekules; i > 0; i--) {
        //printf("pouštím O %ld\n", i);
        sem_post(sems.queue_o);
        //sleep(1);
    }
    //printf("stop3\n");
    //sem_post(sems.mutex);
    
    
    for (size_t i = 0; i < (mem->count_o + mem->count_h); i++) {
        waitpid(process[i],NULL,0);
    }
    //printf("stop4\n");
    

    //printf("done! %ld %ld %ld %ld\n", mem->count_o, mem->count_h, mem->time_i,mem->time_b);
    clean_seme_mem(&sems, mem);
    fclose(output);
    return 0;
}


