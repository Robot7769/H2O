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
        
    //argument 1 počet kyslíků
    count_o = atoi(argv[1]);
    
    //argument 2 počet vodíků
    count_h = atoi(argv[2]);

    //argument 3 čas čekání na zařazení do fronty
    time_i = atoi(argv[3]);
    if (time_i > TIME_MAX) {
        error_exit("TI není v rozsaho 0<=TI<=1000\n");
    }

    //argument 4 čas vytváření molekul
    time_b = atoi(argv[4]);
    if (time_b > TIME_MAX) {
        error_exit("TB není v rozsaho 0<=TB<=1000\n");
    }

    printf("\ndone! %ld %ld %ld %ld\n", count_o, count_h, time_i,time_b);
    return 0;
}
