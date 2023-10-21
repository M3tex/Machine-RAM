#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "ram_io.h"
#include "ram_utils.h"



int main(int argc, char **argv)
{
    int option;
    char *valid_args = "he:";
    char *entree_str = NULL;

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

    launch(entree_str, argv[optind]);

    return 0;
}