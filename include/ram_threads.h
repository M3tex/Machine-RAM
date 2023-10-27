#ifndef _RAM_THREADS_HEADER
#define _RAM_THREADS_HEADER

#include <pthread.h>


/*
 * On utilisera 2 threads:
 *  - 1 qui s'occupera exclusivement de la simulation de la machine RAM.
 *    Il s'occupera d'exécuter le programme
 *  - l'autre s'occupera de tout ce qui est lié aux entrées-sorties:
 *    Principalement l'affichage et la gestion des raccourcis clavier.
 * 
 * En effet, si l'on ne sépare pas les 2, le sleep réalisé entre 
 * l'exécution de 2 instructions impactera également le traitement des
 * entrées-sorties.
 * 
 * On n'utilisera pas de structure d'exclusion mutuelle type mutex
 * car seul le thread de simulation de la machine RAM réalisera des 
 * modifications sur les données partagées, le thread d'affichage ne
 * réalisera que de la lecture.
 * 
 * Pour communiquer des évènements comme la mise en pause, le thread
 * d'affichage enverra des signaux qui seront traités par le thread
 * de simulation (SIGUSR1 et SIGUSR2, respectivement pour mettre en
 * pause et pour exécuter l'instruction suivante).
 * 
 * Le thread de simulation simulera un changement de taille du terminal
 * pour signifier au thread d'affichage qu'il faut mettre à jour 
 * l'affichage.
 */


void *launch_simu_thread(void *data);
void handle_pause(int sig);
void handle_next_instr(int sig);
void send_update_ui_sig();


#endif
