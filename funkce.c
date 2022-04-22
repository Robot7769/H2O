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

#endif // PROJ2_FCE