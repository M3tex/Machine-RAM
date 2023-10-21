#ifndef _RAM_UTILS_HEADER
#define _RAM_UTILS_HEADER


#include <stdio.h>


/* Codes d'erreur */
#define E_FOPEN 1
#define E_SYNTAX 2
#define ALLOC_FAILED 3
#define NULL_PTR 4
#define EMPTY_BAND 5
#define REGISTER_UNINIT 6
#define NO_COLORS_SUPPORT 7
#define NO_CLEAN_STOP 8


void skip_line(FILE *f);
void skip_spaces(FILE *f);
void check_alloc(void *ptr);

void print_help();
void quit(int exit_code, int use_perror, char *fmt, ...);


#endif