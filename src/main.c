#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "ram_io.h"
#include "ram_utils.h"



int main(int argc, char **argv)
{
    int option;
    char *valid_args = "hme:";
    char *entree_str = NULL;
    int is_minimal = 0;     /* Pour lancer en mode minimal ou non */

    while ((option = getopt(argc, argv, valid_args)) != -1)
    {
        switch (option)
        {
        case 'h':
            print_help();
            return 0;
        
        case 'e':
            entree_str = optarg;
            break;

        case 'm':
            is_minimal = 1;
            break;

        default:
            print_help();
            return 1;
        }
    }

    if (optind >= argc)
    {
        printf("Pas de fichier.ram spécifié !\n");
        print_help();
        return 1;
    }

    ram *r = init_ram(entree_str);
    r -> nb_instr = lire_fichier(argv[optind], r -> instructions);

    if (is_minimal) launch_minimal(r);
    else launch(r);

    free_ram(r);

    return 0;
}