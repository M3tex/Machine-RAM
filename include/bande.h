#ifndef _BANDE_HEADER
#define _BANDE_HEADER


/* Liste chaînée pour des entiers */
typedef struct _maillon {
    int val;
    struct _maillon *suiv;
} maillon, *liste;


/**
 * @brief Structure représentant une bande de la machine RAM.
 * Selon les spécifications de la machine RAM, on peut seulement lire
 * sur la bande d'entrée et écrire sur la bande de sortie, sans
 * possibilité de revenir en arrière.
 * 
 * Les seules opérations réalisées sur les listes seront donc:
 *  - Pour la bande d'entrée on peut seulement lire des valeurs.
 *    La lecture sera une opération de dépilage.
 *  - Pour la bande de sortie on peut seulement écrire des valeurs.
 *    L'écriture sera une opération d'enfilage.
 * 
 * Les champs `current_idx` et `size` ne sont pas nécessaires pour la
 * partie fonctionnelle mais sont utilisés pour l'affichage des bandes.
 */
typedef struct {
    liste start;
    liste end;
    int size;
    int current_idx;
} bande;


liste init_maillon(int val);
void free_liste(liste l);
liste copy_liste(liste l);
char *list_to_str(liste l);
void print_liste(const liste l, char nl);

bande init_bande();
void free_bande(bande *b);
int lire_bande(bande *b);
void ecrire_bande(bande *b, int val);

#endif