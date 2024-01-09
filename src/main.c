#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "ram_io.h"
#include "ram_utils.h"


/* Partie gestion des options */
static int is_minimal = 0;
static char *entree_str = NULL;
static int pause = -1;
void handle_options(int argc, char **argv);




int main(int argc, char **argv)
{
    handle_options(argc, argv);

    ram *r = init_ram(entree_str);
    r->nb_instr = lire_fichier(argv[optind], r -> instructions);
    r->pause_at = pause;

    if (is_minimal) launch_minimal(r);
    else launch(r);

    free_ram(r);

    return 0;
}


void handle_options(int argc, char **argv)
{
    int opt;
    const struct option options[] = {
        {"minimal", no_argument, NULL, 'm'},
        {"paused", optional_argument, NULL, 'p'},
        {NULL, 0, NULL, '\0'}
    };

    while((opt = getopt_long(argc, argv, "e:mp::h", options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'm':
            is_minimal = 1;
            break;
        case 'e':
            entree_str = (char *) malloc(sizeof(char) * strlen(optarg));
            check_alloc(entree_str);
            strcpy(entree_str, optarg);
            break;
        case 'p':
            pause = optarg == NULL ? 0 : atoi(optarg);
            break;
        case 'h':
        default:
            print_help();
            exit(1);
            break;
        }
    }

    if (optind >= argc)
    {
        printf("Pas de fichier.ram spécifié !\n");
        print_help();
        exit(1);
    }
}