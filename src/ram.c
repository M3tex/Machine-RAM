#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ram.h"
#include "ram_utils.h"
#include "bande.h"



/**
 * @brief Initialise la machine ram.
 * 
 * @param e La chaîne représentant la bande d'entrée, au format:
 * "1, 2, 3, 4" par exemple (les nombres sont séparés par des virgules
 * ou bien des espaces, "1 2 3 4" marche également).
 * @return ram
 */
ram *init_ram(char *e)
{
    ram *r = (ram *) malloc(sizeof(ram));
    check_alloc(r);


    instruction *instr;
    instr = (instruction *) calloc(DFT_PROG_SIZE, sizeof(instruction));
    check_alloc(instr);
    r -> instructions = instr;
    r -> compteur_ord = r -> nb_exec = r -> nb_instr = 0;
    r -> idx_e = r -> idx_s = 0;


    for (int i = 0; i < MEM_SIZE; i++) r -> memoire[i] = INT_MIN;

    r -> entree = init_bande();
    r -> sortie = init_bande();

    r -> delay = 500;


    if (e != NULL)
    {
        int tmp;
        char *token = strtok(e, ", ");
        while (token != NULL)
        {
            tmp = atoi(token);
            ecrire_bande(&(r -> entree), tmp);
            token = strtok(NULL, ", ");
        }
    }
    

    return r;
}



/**
 * @brief Retourne le numéro de l'instruction à partir de la chaine
 * de caractère la représentant.
 * Par exemple str_to_num_instr("READ") renverra READ (= 0)
 * 
 * @param s 
 * @return instr_ram 
 */
instr_ram str_to_num_instr(char *s)
{
    if (!strcmp(s, "READ")) return READ;
    else if (!strcmp(s, "WRITE")) return WRITE;
    else if (!strcmp(s, "LOAD")) return LOAD;
    else if (!strcmp(s, "STORE")) return STORE;
    else if (!strcmp(s, "DEC")) return DEC;
    else if (!strcmp(s, "INC")) return INC;
    else if (!strcmp(s, "ADD")) return ADD;
    else if (!strcmp(s, "SUB")) return SUB;
    else if (!strcmp(s, "MUL")) return MUL;
    else if (!strcmp(s, "DIV")) return DIV;
    else if (!strcmp(s, "MOD")) return MOD;
    else if (!strcmp(s, "JUMP")) return JUMP;
    else if (!strcmp(s, "JUMZ")) return JUMZ;
    else if (!strcmp(s, "JUML")) return JUML;
    else if (!strcmp(s, "JUMG")) return JUMG;
    else if (!strcmp(s, "STOP")) return STOP;
    else if (!strcmp(s, "NOP")) return NOP;
    else return UNDEF;
}



instruction init_instr()
{
    instruction result;
    strcpy(result.nom, "XXXXX");
    result.adr = -1;
    result.num_instr = -1;
    result.type_adr = '\0';
    return result;
}



void free_ram(ram *r)
{
    free(r -> instructions);
    free_bande(&(r -> entree));
    free_bande(&(r -> sortie));
    free(r);
}


/**
 * @brief Lit le fichier contenant les instructions et les place dans
 * le tableau passé en paramètre.
 * 
 * Le fichier doit respecter la syntaxe suivante:
 *  - une seule instruction par ligne
 *  - le type d'adressage doit être séparé de l'instruction par au moins
 *    un espace (il peut y en avoir +).
 *  - l'adresse peut-être séparée du type d'adressage par 0, 1 ou 
 *    plusieurs espaces.
 *  - tout ce qui est situé à droite d'un ; sur la même ligne qu'une
 *    instruction est un commentaire
 *  - Il peut y avoir des lignes vides
 *  - Il peut y avoir des lignes de commentaires, il suffit de préfixer
 *    la ligne de # (en réalité de n'importe quel caractère non
 *    alphabétique)
 * 
 * @param nom 
 * @param result 
 * @return int Le nombre d'instructions lues
 */
int lire_fichier(char *nom, instruction *result)
{
    int max_prog_size = DFT_PROG_SIZE;

    FILE *f = fopen(nom, "r");
    if (f == NULL)
    {
        quit(E_FOPEN, 1, "Impossible d'ouvrir le fichier");
    }

    /* On considère qu'une ligne ne fera pas + de 1024 caractères */
    char buff[1024];

    char tmp;
    int i = 0;
    while (!feof(f))
    {
        /* On réalloue si nécessaire */
        if (i >= max_prog_size)
        {
            max_prog_size *= 2;
            result = realloc(result, sizeof(instruction) * max_prog_size);
        }

        /* Pour supporter les lignes vides (et commentaires) */
        skip_spaces(f);
        tmp = fgetc(f);
        if (!isalpha(tmp))
        {
            if (tmp != '\n') skip_line(f);
            continue;
        }
        fseek(f, -1, SEEK_CUR);     /* reset du curseur */


        /* 
         * On lit la partie de la ligne qui nous intéresse (avant le ;)
         * Regex avec scanf:
         * %[c1c2...cn] -> tous les mots constitués de c1, c2, ..., cn
         * %[^c1c2...cn] -> tous les mots ne contenant pas c1, ..., cn
         * %*[format] -> le * permet de pas stocker ce qui est lu
         * 
         * De +, lorsque l'on met un espace dans le format, scanf tente
         * de lire 0, 1 ou plusieurs espaces.
         * Ainsi avec le format "x: %d !", les chaînes:
         *  - "x:10!"
         *  - "x:      10!"
         *  - "x: 10            !"
         *  etc. seront toutes reconnues
         */
        if (fscanf(f, "%[^;\n]", buff) != 1) break;


        /* On traite ce qu'on a lu */
        lire_ligne(buff, i, &result[i]);

        /* On passe à la ligne suivante (ce qui reste est un comm) */
        skip_line(f);       
        i += 1;
    }

    fclose(f);

    return i;
}



/**
 * @brief Lit une ligne et remplis les champs correspondant dans la 
 * struct.
 * Par exemple si appelée avec "LOAD @10" on aura dans instr:
 *  - nom:        LOAD
 *  - num_instr:  2
 *  - adr:        10
 *  - type_adr:   @
 * 
 * @param line 
 * @param nb_line 
 * @param instr 
 */
void lire_ligne(char *line, int nb_line, instruction *instr)
{
    int i, num;
    char buff[256];
    for (i = 0; i < 256; buff[i++] = '\0');    /* valgrind râle sinon */


    /* On lit le nom de l'instruction (scanf lit jusqu'au 1er espace) */
    sscanf(line, " %s", buff);

    /* On copie dans le bon champ en mettant en majuscules */
    for (i = 0; i < 5; i++) (instr -> nom)[i] = toupper(buff[i]);

    num = str_to_num_instr(instr -> nom);

    /* Si l'instruction n'est pas dans le jeu d'instructions */
    if (num == UNDEF)
    {
        quit(E_SYNTAX, 0, "Erreur de syntaxe: instruction '%s' "
                        "inconnue", instr -> nom);
    }
    instr -> num_instr = num;
    

    /* Seuls READ WRITE NOP ET STOP n'ont pas d'adresse en param */    
    if (num == READ || num == WRITE || num == NOP || num == STOP)
    {
        instr -> adr = -1;
        return;
    }


    /* On essaye de lire le type d'adressage */
    if (sscanf(line, "%*s %[@# ] %*d", buff) == 1)
    {
        if (buff[0] != '#' && buff[0] != '@')
        {
            quit(E_SYNTAX, 0, "Ligne %d: type d'adressage '%c' "
                              "inconnu", nb_line, buff[0]);
        }
        instr -> type_adr = buff[0];
    }
    else instr -> type_adr = '~';    /* Adressage direct */
    
    /* Il ne reste plus qu'à lire l'adresse */
    if (sscanf(line, "%*s%*[#@ ] %d", &(instr -> adr)) != 1)
    {
        quit(E_SYNTAX, 0, "Ligne %d: Adresse non-spécifiée", nb_line);
    }
}



/* DEBUG */
void print_instr(instruction instr)
{
    printf("%s %c%d\n", instr.nom, instr.type_adr, instr.adr);
}



void evaluer_instr(ram *r, int *boucle)
{
    if (r -> compteur_ord >= r -> nb_instr)
    {
        quit(NO_CLEAN_STOP, 0, "Programme arrêté sans STOP");
    }


    /* Pour éviter d'avoir des lignes à rallonge */
    instruction instr = (r -> instructions)[r -> compteur_ord];

    int is_indirect = instr.type_adr == '@';
    int is_numeric = instr.type_adr == '#';
    int is_direct = instr.type_adr == '~';
    instr_ram num = instr.num_instr;
    int adr = instr.adr;
    int *memoire = r -> memoire;

    

    /* On vérifie que l'adresse ne soit pas trop grande */
    if (adr >= MEM_SIZE || (is_indirect && memoire[adr] >= MEM_SIZE))
    {
        quit(REGISTER_UNINIT, 0, "Ligne %d: adresse trop grande", 
            r -> compteur_ord);
    }

    /*
     * On vérifie que le registre utilisé est bien initialisé, sauf si
     * l'instruction est STORE ou un JUMP.
     * Si l'adresse est en indirect il faut vérifier le bon registre.
     */
    if (!is_numeric && num != STORE && !(num == JUMP || num == JUMZ
        || num == JUML || num == JUMG))
    {
        if (is_indirect && memoire[memoire[adr]] == INT_MIN)
        {
            quit(REGISTER_UNINIT, 0, "Ligne %d: registre n°%d "
            "non-initialisé", r -> compteur_ord, memoire[adr]);
        }
        else if (is_direct && memoire[adr] == INT_MIN)
        {
            quit(REGISTER_UNINIT, 0, "Ligne %d: registre n°%d "
            "non-initialisé", r -> compteur_ord, adr);
        }
    }

    /* On l'incrémente ici car les JUMPs le modifient */
    (r -> compteur_ord)++;

    switch (num)
    {
    case READ:  /* Lit la bande d'entrée et met la valeur dans ACC */
        memoire[0] = lire_bande(&(r -> entree));    
        (r -> idx_e)++;
        break;
    
    case WRITE: /* Écrit la valeur de l'ACC sur la bande de sortie */
        ecrire_bande(&(r -> sortie), memoire[0]);
        (r -> idx_s)++;
        break;
    
    case LOAD:  /* Charge dans l'ACC, depuis la mémoire ou un entier */
        if (is_indirect) memoire[0] = memoire[memoire[adr]];
        else if (is_numeric) memoire[0] = adr;
        else memoire[0] = memoire[adr];
        break;
    
    case STORE: /* Met le contenu de l'ACC à l'adresse spécifiée */
        if (is_indirect) memoire[memoire[adr]] = memoire[0];
        else memoire[adr] = memoire[0];
        break;
    
    case DEC:   /* Décrémente la valeur stockée à l'adresse spécifiée */
        if (is_indirect) memoire[memoire[adr]]--;
        else memoire[adr]--;
        break;

    case INC:   /* Incrémente la valeur stockée à l'adresse spécifiée */
        if (is_indirect) memoire[memoire[adr]]++;
        else memoire[adr]++;
        break;
    
    case ADD:   /* Ajoute à l'ACC, depuis la mémoire ou entier */
        if (is_indirect) memoire[0] += memoire[memoire[adr]];
        else if (is_numeric) memoire[0] += adr;
        else memoire[0] += memoire[adr];
        break;
    
    case SUB:   /* Soustrait à l'ACC, depuis la mémoire ou entier */
        if (is_indirect) memoire[0] -= memoire[memoire[adr]];
        else if (is_numeric) memoire[0] -= adr;
        else memoire[0] -= memoire[adr];
        break;
    
    case MUL:   /* Multiplie à l'ACC, depuis la mémoire ou entier */
        if (is_indirect) memoire[0] *= memoire[memoire[adr]];
        else if (is_numeric) memoire[0] *= adr;
        else memoire[0] *= memoire[adr];
        break;
    
    case DIV:   /* Divise l'ACC, depuis la mémoire ou entier */
        if (is_indirect) memoire[0] /= memoire[memoire[adr]];
        else if (is_numeric) memoire[0] /= adr;
        else memoire[0] /= memoire[adr];
        break;

    case MOD:   /* Prend le mod de l'ACC, depuis la mémoire ou entier */
        if (is_indirect) memoire[0] %= memoire[memoire[adr]];
        else if (is_numeric) memoire[0] %= adr;
        else memoire[0] %= memoire[adr];
        break;
    
    case JUMP:  /* Change le compteur ordinal */
        if (is_indirect) r -> compteur_ord = memoire[adr];
        else r -> compteur_ord = adr;
        break;
    
    case JUMZ:  /* Change le compteur ordinal si ACC = 0 */
        if (memoire[0] == 0)
        {
            if (is_indirect) r -> compteur_ord = memoire[adr];
            else r -> compteur_ord = adr;
        }
        break;

    case JUML:  /* Change le compteur ordinal si ACC < 0 */
        if (memoire[0] < 0)
        {
            if (is_indirect) r -> compteur_ord = memoire[adr];
            else r -> compteur_ord = adr;
        }
        break;
    
    case JUMG:  /* Change le compteur ordinal si ACC > 0 */
        if (memoire[0] > 0)
        {
            if (is_indirect) r -> compteur_ord = memoire[adr];
            else r -> compteur_ord = adr;
        }
        break;

    case STOP:  /* Arrête le programme */
        *boucle = 0;
        return;
    
    case NOP:   /* Ne fait rien */
        break;
    default:
        break;
    }

    (r -> nb_exec)++;
}