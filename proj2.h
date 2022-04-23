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
    size_t count_o;
    size_t count_h;
    size_t time_i;
    size_t time_b;
}shmem_t;


#endif // PROJ2_LIB