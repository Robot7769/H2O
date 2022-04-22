/**
 * @file error.h
 * @author Jan Škrabal (xskrab12)-FIT
 * @brief IJC-DU1 B hlavičkový soubor vypisování error zpráv
 * @date 2022-02-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef IJC_ERROR_LIB
#define IJC_ERROR_LIB

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // funkce s proměným počtem  argumentů
//vfprintf();
//va_start;
//....

extern void warning_msg(const char *fmt, ...);

extern void error_exit(const char *fmt, ...);

/*
#define error_exit(__ftm,...) \
    fprintf(stderr, __ftm); \
    exit(EXIT_FAILURE); \*/

#endif  // IJC_ERROR_LIB