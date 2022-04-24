/**
 * @file proj2.h
 * @author Jan Škrabal (xskrab12)-FIT
 * @brief IOS projekt 2 knihovna synchronizace (Building H2O)
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef PROJ2_LIB
#define PROJ2_LIB

#include <stdlib.h>

#define ARG_COUNT 5
#define TIME_MAX 1000

//! struktura
typedef struct shmem {
    size_t id;
    size_t count_o;
    size_t count_h;
    size_t time_i;
    size_t time_b;

    size_t out_o;
    size_t out_h;
    size_t index_o;
    size_t index_h;
    size_t molecule;
}shmem_t;

sem_t mutex;
sem_t queue_o;
sem_t queue_h;
sem_t barier;
sem_t print;


#endif // PROJ2_LIB