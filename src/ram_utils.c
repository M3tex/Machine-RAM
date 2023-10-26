#include "ram_utils.h"
#include <stdlib.h>
#include <stdarg.h>
#include <ncurses.h>





/**
 * @brief Fait avancer le curseur du fichier tant qu'on lit des espaces.
 * 
 * @param f Le fichier
 */
void skip_spaces(FILE *f)
{
    char tmp;
    do {
        tmp = fgetc(f);
    } while ((tmp == ' ' || tmp == '\t') && !feof(f));

    /* Car on est allé 1 caractère trop loin */
    if (!feof(f)) fseek(f, -1, SEEK_CUR);
}



/**
 * @brief Fait avancer le curseur du fichier jusqu'à sauter une ligne
 * 
 * @param f Le fichier
 */
void skip_line(FILE *f)
{
    char tmp;
    do {
        tmp = fgetc(f);
    } while (!feof(f) && tmp != '\n');
}



/**
 * @brief À appeler à la suite d'un appel à malloc ou calloc.
 * Permet de vérifier que l'allocation s'est bien déroulée, et si
 * ça n'est pas le cas termine l'exécution du programme.
 * 
 * @param ptr Le pointeur obtenu via malloc/calloc
 */
void check_alloc(void *ptr)
{
    if (ptr == NULL)
    {
        quit(ALLOC_FAILED, 1, "Erreur lors de l'allocation mémoire");
    }
}



/**
 * @brief Quitte le programme en s'assurant que le terminal quitte le
 * raw-mode.
 * 
 * Pour la partie affichage: https://stackoverflow.com/a/54879016
 * 
 * @param exit_code 
 */
void quit(int exit_code, int use_perror, char *fmt, ...)
{
    delwin(stdscr);
    endwin();

    /* Affichage de l'erreur */
    if (use_perror) perror(fmt);
    else
    {
        va_list arglist;
        va_start(arglist, fmt);

        int length = vsnprintf(NULL, 0, fmt, arglist) + 1;
        char *buffer = malloc(length * sizeof *buffer); 
        vsnprintf(buffer, length, fmt, arglist);
        va_end(arglist);

        puts(buffer);
        free(buffer);
    }
    exit(exit_code);
}




void print_help()
{
    printf("Utilisation:\n");
    printf("ram [-e \"x1, x2, ..., xn\"] nom_fichier.ram\n");

    // printf("Avec x1, x2, ..., xn les n entiers de la bande d'entrée ");
    // printf("(à noter qu'ils peuvent également être séparés par des ");
    // printf("espaces)\n");
}