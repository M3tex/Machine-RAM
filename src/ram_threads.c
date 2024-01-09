#include <pthread.h>
#include <signal.h>
#include <ncurses.h>    /* Seulement pour napms */

#include "ram.h"
#include "ram_threads.h"


static int is_paused;
static int exec_next_instr;

extern int thread_is_running;


void *launch_simu_thread(void *data)
{
    ram *r = (ram *) data;

    is_paused = 0;
    exec_next_instr = 0;

    signal(SIGUSR1, handle_pause);
    signal(SIGUSR2, handle_next_instr);

    /* Pour avoir le temps de voir la 1ère instruction */
    send_update_ui_sig();
    napms(r -> delay);
    
    int loop = 1;
    while(loop)
    {
        if (r->pause_at == r->compteur_ord)
        {
            is_paused = 1;
            
            /* Seulement la 1ère fois */
            r->pause_at = -1;
        }

        if (!is_paused)
        {
            evaluer_instr(r, &loop);
            send_update_ui_sig(); 
            napms(r -> delay);
        }
        else if (is_paused && exec_next_instr)
        {
            evaluer_instr(r, &loop);
            exec_next_instr = 0;

            send_update_ui_sig();
        }
    }

    thread_is_running = 0;
    return NULL;
}



void handle_pause(int sig)
{
    is_paused = !is_paused;
}



void handle_next_instr(int sig)
{
    if (is_paused) exec_next_instr = 1;
}



/**
 * @brief Signale au thread d'affichage qu'il faut mettre à jour
 * l'affichage.
 * 
 * J'utilisais le signal SIGWINCH mais il semblerait que ncurses
 * le traite déjà pour mettre à jour les dimensions du terminal.
 * En le traitant moi-même il semblerait que ncurses ne le traitait plus
 * (et donc ne mettait plus à jours LINES et COLS).
 * 
 * On utilise donc le fait que ncurses traite cet évènement en simulant
 * un resize.
 * 
 */
void send_update_ui_sig()
{
    resizeterm(LINES, COLS);
}