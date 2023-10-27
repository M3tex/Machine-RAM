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
 * Pour la partie affichage: https://stackoverflow.com/a/54879016 et
 * https://sourceware.org/git?p=glibc.git;a=blob_plain;f=stdio-common/printf.c;hb=HEAD
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
        vfprintf(stdout, fmt, arglist);
        va_end(arglist);
        printf("\n");
    }
    exit(exit_code);
}



/**
 * @brief Affiche les informations sur l'utilisation de la commande.
 * 
 */
void print_help()
{
    printf("Utilisation: ");
    printf("ram [-hm] [-e bande-entree] nom_fichier.ram\n\n");

    printf("-h Affiche cette aide\n");
    printf("-m Affichage 'minimal' (ACC + bande de sortie)\n");

    printf("-e Pour spécifier le contenu de la bande d'entrée\n");
    printf("Les entiers de la bande d'entrée doivent être ");
    printf("séparés par des espaces ou des virgules.\n");
    printf("Par exemple: ram -e '10, 12, 9, 17, 0' moyenne.ram\n\n");

    printf("Le fichier.ram doit respecter la syntaxe suivante:\n");
    printf("- une ligne ne doit pas faire + de 1024 caractères\n");
    printf("- une seule instruction par ligne\n");
    printf("- le type d'adressage doit être séparé de l'instruction par"
           " au moins un espace (il peut y en avoir plusieurs)\n");
    printf("- l'adresse peut-être collée au type d'adressage, ou bien "
           "séparée de ce dernier par un ou plusieurs espaces\n");
    printf("- tout ce qui est situé à droite d'un `;` sur la même ligne" 
           " qu'une instruction est un commentaire\n");
    printf("- il peut y avoir des lignes vides\n");
    printf("- toutes les lignes commençant par un caractère "
           "non-alphabétique autre qu'un espace ou une tabulation est "
           "un commentaire\n");
}