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

void memory_setup(shmem_t *memory) {
    memory->out_o = 0;
    memory->out_h = 0;
    memory->molecule = 0;   
    memory->bar = -3;
}

int sem_start(sems_t *sems) {
    sems->mutex = sem_open("/ios_proj2_H2O_mutex", O_CREAT | O_EXCL, 0666, 1);
    if (sems->mutex == SEM_FAILED) {
        return 1;
    }
    sems->queue_o = sem_open("/ios_proj2_H2O_queue_o", O_CREAT | O_EXCL, 0666, 0);
    if (sems->queue_o == SEM_FAILED) {
        sem_unlink("/ios_proj2_H2O_mutex");
        sem_close(sems->mutex);
        return 1;
    }
    sems->queue_h = sem_open("/ios_proj2_H2O_queue_h", O_CREAT | O_EXCL, 0666, 0);
    if (sems->queue_h == SEM_FAILED) {
        sem_unlink("/ios_proj2_H2O_mutex");
        sem_close(sems->mutex);
        sem_unlink("/ios_proj2_H2O_queue_o");
        sem_close(sems->queue_o);
        return 1;
    }
    sems->barrier = sem_open("/ios_proj2_H2O_barrier", O_CREAT | O_EXCL, 0666, 0);
    if (sems->barrier == SEM_FAILED) {
        sem_unlink("/ios_proj2_H2O_mutex");
        sem_close(sems->mutex);
        sem_unlink("/ios_proj2_H2O_queue_o");
        sem_close(sems->queue_o);
        sem_unlink("/ios_proj2_H2O_queue_h");
        sem_close(sems->queue_h);
        return 1;
    }
    sems->print = sem_open("/ios_proj2_H2O_print", O_CREAT | O_EXCL, 0666, 1);
    if (sems->print == SEM_FAILED) {
        sem_unlink("/ios_proj2_H2O_mutex");
        sem_close(sems->mutex);
        sem_unlink("/ios_proj2_H2O_queue_o");
        sem_close(sems->queue_o);
        sem_unlink("/ios_proj2_H2O_queue_h");
        sem_close(sems->queue_h);
        sem_unlink("/ios_proj2_H2O_barrier");
        sem_close(sems->barrier);
        return 1;
    }
    sems->print = sem_open("/ios_proj2_H2O_molecule", O_CREAT | O_EXCL, 0666, 1);
    if (sems->print == SEM_FAILED) {
        sem_unlink("/ios_proj2_H2O_mutex");
        sem_close(sems->mutex);
        sem_unlink("/ios_proj2_H2O_queue_o");
        sem_close(sems->queue_o);
        sem_unlink("/ios_proj2_H2O_queue_h");
        sem_close(sems->queue_h);
        sem_unlink("/ios_proj2_H2O_barrier");
        sem_close(sems->barrier);
        sem_unlink("/ios_proj2_H2O_print");
        sem_close(sems->print);
        return 1;
    }
    return 0;
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

void molecul_creator(sems_t sems,shmem_t *mem, FILE *output, const char c, size_t id, size_t mol_count) {
    sem_wait(sems.molecule);
    print_out(sems,mem, output,"%c %d: creating molecule %d\n", c, id, mol_count);
    if (mem->time_b != 0) {
        usleep(rand() % ((mem->time_b + 1)*1000));
    }
    print_out(sems,mem, output,"%c %d: molecule %d created\n", c, id, mol_count);
    (mem->bar)++;
    printf("bar: %d\n", mem->bar);
    if (mem->bar == 0 ) {
        sem_post(sems.barrier);
        sem_post(sems.barrier);
        sem_post(sems.barrier);
        mem->bar = -3;
    }
    sem_post(sems.molecule);
}

void oxygen(size_t ido, sems_t sems, shmem_t *mem, FILE *output) {
    srand(time(NULL) ^ (getpid()<<16));
    print_out(sems,mem, output,"O %d: started\n",ido);
    if (mem->time_i != 0) {
        usleep(rand() % ((mem->time_i + 1)*1000));
    }
    sem_wait(sems.mutex);
    mem->out_o +=1;
    print_out(sems,mem, output,"O %d: going to queue\n", ido);
    if (mem->out_h >= 2) {
        mem->molecule +=1;
        sem_post(sems.queue_h);
        sem_post(sems.queue_h);
        mem->out_h -= 2;
        sem_post(sems.queue_o);
        mem->out_o -= 1;
    } else {
        sem_post(sems.mutex);
        print_out(sems,mem,output, "O %d: not enough H\n", ido);
    }
    printf("wait_O\n");
    sem_wait(sems.queue_o);
    printf("NOT_wait_O\n");
    //! bont
    int mol = mem->molecule;
    molecul_creator(sems, mem, output, 'O', ido, mol);
    
    printf("wait_BAR %ld\n", ido);
    sem_wait(sems.barrier);
    printf("NOT_wait_BAR %ld\n", ido);
    sem_post(sems.mutex);
    fclose(output);
}

void hydrogen(size_t idh, sems_t sems, shmem_t *mem, FILE *output) {
    srand(time(NULL) ^ (getpid()<<16));
    print_out(sems,mem, output,"H %d: started\n", idh);
    if (mem->time_i != 0) {
        usleep(rand() % ((mem->time_i + 1)*1000));
    }
    sem_wait(sems.mutex);
    mem->out_h +=1;
    print_out(sems,mem, output,"H %d: going to queue\n", idh);
    if (mem->out_h >= 2 && mem->out_o >= 1) {
        mem->molecule +=1;
        sem_post(sems.queue_h);
        sem_post(sems.queue_h);
        mem->out_h -= 2;
        sem_post(sems.queue_o);
        mem->out_o -= 1;
    } else {
        sem_post(sems.mutex);
        print_out(sems,mem,output, "H %d: not enough O or H\n", idh);
    }
    printf("wait_H %ld\n",idh);
    sem_wait(sems.queue_h);
    printf("NOT_wait_H %ld\n", idh);
    //! bont
    int mol = mem->molecule;
    molecul_creator(sems, mem, output, 'H', idh, mol);

    printf("wait_BAR %ld\n",idh);
    sem_wait(sems.barrier);
    printf("NOT_wait_BAR %ld\n", idh);
    fclose(output);
}

void clean_seme_mem(sems_t *sems, shmem_t *mem) {
    sem_unlink("/ios_proj2_H2O_mutex");
    sem_close(sems->mutex);

    sem_unlink("/ios_proj2_H2O_queue_o");
    sem_close(sems->queue_o);

    sem_unlink("/ios_proj2_H2O_queue_h");
    sem_close(sems->queue_h);

    sem_unlink("/ios_proj2_H2O_barrier");
    sem_close(sems->barrier);

    sem_unlink("/ios_proj2_H2O_print");
    sem_close(sems->print);

    sem_unlink("/ios_proj2_H2O_molecule");
    sem_close(sems->molecule);
    
    munmap(mem, sizeof(shmem_t));
}


#endif // PROJ2_FCE