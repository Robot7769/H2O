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

size_t count_o = 0;
size_t count_h = 0;
size_t time_i = 0;
size_t time_b = 0;

/**
 * @brief 
 * 
 * @param ptr 
 * @return true 
 * @return false 
 */
bool is_number( char const *ptr) {
    for (size_t i = 0; i < strlen(ptr[1]); i++) {
        if (!(ptr[i] >= '0' && ptr[i] <= '9')){
            return true;
            //error_exit("Argument není celé kladné číslo\n");
        }
    }
    return false;
}

  
int main(int argc, char const *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Nesprávný počet argumentů!\n");
        exit(1);
    }
    //argument 1 počet kyslíků
    if (is_number(argv[1])) {
        count_o = atoi(argv[1]);
    }
    //argument 2 počet vodíků
    if (is_number(argv[2])) {
        count_o = atoi(argv[2]);
    }
    //argument 3 čas vytváření molekul
    if (is_number(argv[3])) {
        count_o = atoi(argv[3]);
    }
    //argument 3 čas vytváření molekul
    if (is_number(argv[4])) {
        count_o = atoi(argv[4]);
    }

    return 0;
}
