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
#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "error.h"

#define ARG_COUNT 5
#define TIME_MAX 1000


size_t count_o = 0;
size_t count_h = 0;
size_t time_i = 0;
size_t time_b = 0;

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

  
int main(int argc, char const *argv[]) {
    if (argc != ARG_COUNT) {
        error_exit("Nesprávný počet argumentů!\n");
    }
    for (int i = 1; i < ARG_COUNT; i++) { // validace že jsou všechny zadané paramentry čísla
        if (is_number(argv[i])) {
            error_exit("Argument %d není celé kladné číslo\n", i);
        }
    }
        
    //argument 1 počet kyslíků
    count_o = atoi(argv[1]);
    
    //argument 2 počet vodíků
    count_h = atoi(argv[2]);

    //argument 3 čas čekání na zařazení do fronty
    time_i = atoi(argv[3]);

    //argument 4 čas vytváření molekul
    time_b = atoi(argv[4]);

    printf("\ndone!\n");
    return 0;
}
