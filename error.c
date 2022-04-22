/**
 * @file error.c
 * @author Jan Škrabal (xskrab12)-FIT
 * @brief IJC-DU1 B vypisování error zpráv
 * @date 2022-02-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "error.h"

void warning_msg(const char *fmt, ...){
    va_list(argumenty);
    va_start(argumenty,fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr,fmt,argumenty);
    va_end(argumenty);
}

void error_exit(const char *fmt, ...){
    va_list(argumenty);
    va_start(argumenty,fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr,fmt,argumenty);
    va_end(argumenty);
    exit(EXIT_FAILURE);
}