/**
 * @file funkce.c
 * @author Jan Škrabal (xskrab12)-FIT
 * @brief IOS projekt 2 Funkce synchronizace (Building H2O)
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef PROJ2_FCE
#define PROJ2_FCE


#include <stdbool.h>
#include <string.h>

/**
 * @brief Funkce na validaci že řetězec obsahuje pouze číslice
 * 
 * @param ptr 
 * @return true 
 * @return false 
 */
bool is_number( char const *ptr) {
    for (size_t i = 0; i < strlen(ptr); i++) {
        if (!(ptr[i] >= '0' && ptr[i] <= '9')){
            return true;
        }
    }
    return false;
}

void print_out(sems_t sems,shmem_t *mem, FILE *output,const char *fmt, ...) {
    sem_wait(sems.print);
    mem->id +=1;
    va_list(argumenty);
    va_start(argumenty,fmt);
    fprintf(output, "%ld: ", mem->id);
    fflush(output);
    //fprintf(stdout, "%ld: ", mem->id); //! test
    vfprintf(output,fmt,argumenty);
    fflush(output);
    //vfprintf(stdout,fmt,argumenty); //! test
    va_end(argumenty);
    sem_post(sems.print);
}

void oxygen(sems_t sems, shmem_t *mem, FILE *output) {
    sem_wait(sems.mutex);
    mem->index_o +=1;       // ID kyslíku
    print_out(sems,mem, output,"O %d: started\n", mem->index_o);
    if (mem->time_i != 0) {
        usleep(rand() % mem->time_i + 1);
    }
    mem->out_o +=1;
    print_out(sems,mem, output,"O %d: going to queue\n", mem->index_o);
    if (mem->out_h >= 2) {
        mem->molecule +=1;
        print_out(sems,mem, output,"O %d: creating molecule %d\n", mem->index_o, mem->molecule);
        if (mem->time_b != 0) {
            usleep(rand() % mem->time_b + 1);
        }
        sem_post(sems.queue_h);
        sem_post(sems.queue_h);
        mem->out_h -= 2;
        sem_post(sems.queue_o);
        mem->out_o -= 1;
        print_out(sems,mem, output,"O %d: molecule %d created\n", mem->index_o, mem->molecule);
    } else {
        sem_post(sems.mutex);
    }
    sem_wait(sems.queue_o);
    //! bont
    
    sem_wait(sems.barrier);
    sem_post(sems.mutex);
    fclose(output);
}

void hydrogen(sems_t sems, shmem_t *mem, FILE *output) {
    sem_wait(sems.mutex);
    mem->index_h +=1;       // ID vodíku
    print_out(sems,mem, output,"H %d: started\n", mem->index_h);
    if (mem->time_i != 0) {
        usleep(rand() % mem->time_i + 1);
    }
    mem->out_h +=1;
    print_out(sems,mem, output,"H %d: going to queue\n", mem->index_h);
    if (mem->out_h >= 2 && mem->count_o >= 1) {
        mem->molecule +=1;
        print_out(sems,mem, output,"H %d: creating molecule %d\n", mem->index_h, mem->molecule);
        if (mem->time_b != 0) {
            usleep(rand() % mem->time_b + 1);
        }
        sem_post(sems.queue_h);
        sem_post(sems.queue_h);
        mem->out_h -= 2;
        sem_post(sems.queue_o);
        mem->out_o -= 1;
        print_out(sems,mem, output,"H %d:molecule %d created\n", mem->index_h, mem->molecule);
    } else {
        sem_post(sems.mutex);
    }
    sem_wait(sems.queue_h);
    //! bont
    
    sem_wait(sems.barrier);
    fclose(output);
}


#endif // PROJ2_FCE