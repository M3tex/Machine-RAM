#include "bande.h"
#include "ram_utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>



bande init_bande()
{
    bande result;

    result.start = NULL;
    result.end = NULL;
    result.size = 0;
    result.current_idx = -1;

    return result;
}



void free_bande(bande *b)
{
    if (b == NULL) return;

    free_liste(b -> start);

}



void free_liste(liste l)
{
    liste aux;
    while (l != NULL)
    {
        aux = l -> suiv;
        free(l);
        l = aux;
    }
}



liste init_maillon(int val)
{
    liste result = (liste) malloc(sizeof(maillon));
    check_alloc(result);

    result -> val = val;
    result -> suiv = NULL;

    return result;
}



/**
 * @brief Ajoute la valeur à la fin de la bande (= enfilage).
 * 
 * @param b 
 * @param val 
 */
void ecrire_bande(bande *b, int val)
{
    if (b == NULL) quit(NULL_PTR, 0, "Erreur: la bande n'existe pas");

    liste to_add = init_maillon(val);

    /* liste vide */
    if (b -> size == 0) b -> start = b -> end = to_add;
    else
    {
        to_add -> suiv = b -> end -> suiv;
        b -> end -> suiv = to_add;
        b -> end = to_add;
    }

    b -> size += 1;
    b -> current_idx += 1;
}



int depiler(liste *l)
{
    /* Si la liste est vide on ne peut pas dépiler */
    if (l == NULL || *l == NULL) 
    {
        quit(EMPTY_BAND, 0, "Erreur: la bande est vide");
    }


    int result = (*l) -> val;
    liste aux = *l;
    *l = (*l) -> suiv;
    free(aux);

    return result;
}



/**
 * @brief Récupère la valeur au début de la bande (= dépilage).
 * 
 * @param b 
 * @return int 
 */
int lire_bande(bande *b)
{
    if (b == NULL) quit(NULL_PTR, 0, "Erreur: la bande n'existe pas");
    
    return depiler(&(b -> start));
}



/**
 * @brief Affiche une liste au format Python.
 * Par exemple la liste 1 -> 2 -> 3 sera affichée [1, 2, 3] et la liste
 * vide sera affichée [].
 * 
 * Utilisé pour du debug principalement.
 * 
 * @param l La liste à afficher
 * @param nl 1 si on veut sauter une ligne après l'affichage, 0 sinon.
 */
void print_liste(const liste l, char nl)
{
    liste aux = l;
    printf("[");
    while (aux)
    {
        printf("%d", aux -> val);
        if (aux -> suiv != NULL) printf(", ");
        aux = aux -> suiv;
    }
    printf("]");

    if (nl) printf("\n");
}



/**
 * @brief Retourne la liste sous sa représentation en string.
 * Par exemple la liste [1, 2, 30, 0] retournera "1 2 30 0"
 * 
 * @param l La liste concernée
 * @return char* 
 */
char *list_to_str(liste l)
{
    /* On compte le nombre d'éléments */
    liste aux = l;
    size_t size = 0;
    while (aux != NULL)
    {
        size++;
        aux = aux -> suiv;
    }

    /*
     * Un entier tient au + sur log10(INT_MAX) + 1 caractères
     * On fait x size car il y a size entiers, et  + size + 1 car il y a
     * size - 1 espaces et 1 \0
     */
    size = (log10(INT_MAX) + 1) * size + size + 1;
    char *result = (char *) calloc(size, sizeof(char));
    check_alloc(result);

    char buff[64];
    
    aux = l;
    while (aux != NULL)
    {
        sprintf(buff, "%d", aux -> val);
        strcat(result, buff);
        if (aux -> suiv != NULL) strcat(result, " ");

        aux = aux -> suiv;
    }

    return result;
}



liste copy_liste(liste l)
{
    if (l == NULL) return NULL;

    liste result = init_maillon(l -> val);
    liste aux = result;
    l = l -> suiv;

    while (l != NULL)
    {
        aux -> suiv = init_maillon(l -> val);;
        aux = aux -> suiv;
        l = l -> suiv;
    }

    return result;
}