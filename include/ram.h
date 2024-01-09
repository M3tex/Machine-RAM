#ifndef _RAM_HEADER
#define _RAM_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "bande.h"


/*
 * On stockera au + 65536 entiers dans la mémoire.
 * Les entiers sont codés par des int [-2147483647, 2147483647] ce qui
 * prendra 0.26 Mo en mémoire sur la plupart des machines où les int
 * sont codés sur 4 octets.
 */
#define MEM_SIZE USHRT_MAX + 1
#define NB_INSTR 17
#define DFT_PROG_SIZE USHRT_MAX


/*
 * Jeu d'instructions de la machine RAM.
 * 17 instructions + UNDEF qui représente une instruction n'existant pas
 * (utilisé pour détecter une erreur de syntaxe).
 */
typedef enum {
    READ,
    WRITE,
    LOAD,
    STORE,
    DEC,
    INC,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    JUMP,
    JUMZ,
    JUML,
    JUMG,
    STOP,
    NOP,
    UNDEF
} instr_ram;



typedef struct {
    /*
     * Le nom de l'instruction. Les instructions les + longues font 5
     * caractères (WRITE, STORE, JUMPZ, etc.)
     */
    char nom[5];
    instr_ram num_instr;

    /* L'adresse concernée par l'instruction, -1 si aucune */
    int adr;

    /*
     * Le type d'adressage qui s'appliquera à l'adresse.
     * Peut être:
     *  - indirect  (préfixe: @)
     *  - numérique (préfixe: #)
     *  - direct    (pas de préfixe, ici sera représenté par '~')
     * 
     * Pour le numérique, l'adresse devient une valeur à charger dans 
     * l'ACC.
     * 
     * Par exemple (R étant la mémoire et R[i] le ième registre):
     * LOAD 1   -> direct, on charge le contenu de R[1]
     * LOAD @1  -> indirect, on charge le contenu R[R[1]]
     * LOAD #1  -> numérique, on charge la valeur 1 dans l'ACC
     */
    char type_adr;
} instruction;




typedef struct {
    /* 
     * Les registres seront initialisés à une valeur spéciale: INT_MIN.
     * Cette valeur simulera le fait que le registre n'a pas été
     * initialisé par le programme RAM.
     * 
     * memoire[0] est l'accumulateur (= ACC).
     */
    int memoire[MEM_SIZE];
    unsigned int compteur_ord;
    unsigned int nb_exec;
    unsigned int nb_instr;

    unsigned int idx_e;
    unsigned int idx_s;

    /* Délai en ms entre l'exécution de 2 instructions */
    int delay;

    /* Si jamais il faut mettre en pause dès le début */
    int pause_at;

    bande entree;
    bande sortie;

    instruction *instructions;
} ram;



instruction init_instr();
void print_instr(instruction instr);        // ! DEBUG
instr_ram str_to_num_instr(char *s);

ram *init_ram(char *e);
void free_ram(ram *r);

int lire_fichier(char *nom, instruction *result);
void lire_ligne(char *line, int nb_line, instruction *instr);
void evaluer_instr(ram *r, int *boucle);

#endif
