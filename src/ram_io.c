#include <ncurses.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "ram_io.h"
#include "ram.h"
#include "ram_utils.h"
#include "ram_threads.h"



static int need_refresh;
pthread_t exec_thread_id;


/**
 * @brief Initialise toutes les fonctions de ncurses nécessitant une
 * initialisation.
 * 
 */
void init_ui()
{
    initscr();
    
    if (!(has_colors() && can_change_color()))
    {
        char *msg = "Ce terminal ne prend pas bien en charge "
                    "les couleurs";
        quit(NO_COLORS_SUPPORT, 0, msg);
    }

    raw(); 
    curs_set(0); 
    start_color();
    use_default_colors();

    /* Pour savoir quand mettre à jour l'affichage */
    need_refresh = 1;

    /* Pour rendre getch non bloquant */
    nodelay(stdscr, TRUE);

    /* On ajoute nos couleurs */
    init_color(COLOR_INSTR_DFT, 760, 850, 600);
    init_color(COLOR_INSTR_JMP, 850, 440, 840);
    init_color(COLOR_INSTR_IO, 270, 800, 500);
    init_color(COLOR_ADR_IND, 0, 690, 1000);

    init_color(COLOR_ADR_NUM, 610, 860, 1000);
    init_color(COLOR_NB, 610, 860, 1000);
    init_color(COLOR_GRAY, 360, 450, 450);
    init_color(COLOR_STOP, 1000, 0, 0);

    /* Paires (bg et fg) de couleurs */
    init_pair(WHITEBG_BLACKFG, COLOR_BLACK, COLOR_WHITE);
    init_pair(WHITEBG_WHITEFG, COLOR_WHITE, COLOR_WHITE);

    /* 
     * On se permet de mettre l'id de couleur en id de paire car seule 
     * la couleur de fg change (l'id de couleur définit à lui seul 
     * la paire).
     * 
     * -1 indique qu'on laisse la couleur par défaut du terminal.
     */
    init_pair(COLOR_GRAY, COLOR_GRAY, -1);
    init_pair(COLOR_STOP, COLOR_STOP, -1);
    init_pair(COLOR_INSTR_DFT, COLOR_INSTR_DFT, -1);
    init_pair(COLOR_INSTR_JMP, COLOR_INSTR_JMP, -1);
    init_pair(COLOR_INSTR_IO, COLOR_INSTR_IO, -1);
    init_pair(COLOR_ADR_IND, COLOR_ADR_IND, -1);
    init_pair(COLOR_ADR_NUM, COLOR_ADR_NUM, -1);
    init_pair(COLOR_NB, COLOR_NB, -1);
}



/**
 * @brief Détecte si une touche est pressée.
 * Utilisée car on a rendu l'appel de getch() non bloquant avec nodelay.
 * 
 * https://stackoverflow.com/a/4028974
 * 
 * @return int 
 */
int is_keypressed()
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}



/**
 * @brief Affiche la barre blanche en bas du terminal.
 * 
 */
void draw_status_bar()
{
    const char *d = "Appuyez sur q pour quitter";
    const char *m = "Machine RAM";
    const char *f = "Universite de Toulon";

    /* On met le fond en blanc */
    attron(COLOR_PAIR(WHITEBG_WHITEFG));
    for (int i = 0; i < COLS; i++) mvaddch(LINES - 1, i, ' ');
    attroff(COLOR_PAIR(WHITEBG_WHITEFG));

    /* On affiche les infos */
    attron(COLOR_PAIR(WHITEBG_BLACKFG));
    mvprintw(LINES - 1, 1, "%s", d);
    mvprintw(LINES - 1, COLS / 2 - strlen(m) / 2, "%s", m);
    mvprintw(LINES - 1, COLS - strlen(f) - 1, "%s", f);
    attroff(COLOR_PAIR(WHITEBG_BLACKFG));
}



/**
 * @brief Affiche la fenêtre contenant les instructions
 * 
 * @param w 
 * @param r 
 */
void draw_instr_window(WINDOW *w, ram *r)
{
    if (w == NULL || r == NULL) quit(NULL_PTR, 0, "Pointeur null");

    char tmp;
    int pad = 1;
    int color, height;
    instruction instr;
    int co = r -> compteur_ord;
    char *title = "Instructions";
    int width = (COLS - SPACE_BTWN_W) / 2;

    werase(w);
    wresize(w, LINES - (3 + 2 + 1), width);
    box(w, ACS_VLINE, ACS_HLINE);
    mvwprintw(w, 0, width / 2 - strlen(title) / 2, "%s", title);

    height = getmaxy(w) - 1;
    
    /* Pour faire défiler si trop d'instructions */
    int i = 0;
    int scroll = height;
    while (co >= scroll)
    {
        i += height;
        scroll *= 2;
    }


    int cpt = 0;
    while (i < r -> nb_instr && cpt < height - 1)
    {
        instr = r -> instructions[i];

        /* On ajoute le marqueur du compteur ordinal au bon endroit */
        tmp = (i == co) ? '>' : ' ';
        mvwprintw(w, pad + cpt, 1, "%c", tmp);

        /* On affiche le numéro de l'instruction */
        wattron(w, COLOR_PAIR(COLOR_GRAY));
        mvwprintw(w, pad + cpt, pad + 1, "%d", i);
        wattroff(w, COLOR_PAIR(COLOR_GRAY));

        /* Puis le reste de l'instruction */
        color = get_instr_color(instr.num_instr);
        wattron(w, COLOR_PAIR(color));
        mvwprintw(w, pad + cpt, pad + 3 + 2, "%s", instr.nom);
        wattroff(w, COLOR_PAIR(color));

        /* Si l'instruction a un paramètre (= adresse) on l'affiche */
        if (instr.adr != -1)
        {
            waddch(w, ' ');
            if (instr.type_adr != '~')
            {
                color = get_adr_type_color(instr.type_adr);
                wattron(w, COLOR_PAIR(color));
                waddch(w, instr.type_adr);
                wattroff(w, COLOR_PAIR(color));
            }
            wattron(w, COLOR_PAIR(COLOR_NB));
            wprintw(w, "%d", instr.adr);
            wattroff(w, COLOR_PAIR(COLOR_NB));
        }
        i++;
        cpt++;
    }

    wrefresh(w);
}



void draw_mem_window(WINDOW *w, ram *r)
{
    if (w == NULL || r == NULL) quit(NULL_PTR, 0, "Pointeur NULL");

    int pad = 1;
    int width = (COLS - SPACE_BTWN_W) / 2;
    char *title = "Memoire";

    werase(w);
    wresize(w, LINES - (3 + 2 + 1), width);
    box(w, ACS_VLINE, ACS_HLINE);
    mvwprintw(w, 0, width / 2 - strlen(title) / 2, "%s", title);
    mvwin(w, 3, COLS - 3 - width);

    int height = getmaxy(w) - 1;

    mvwprintw(w, 1, pad + 1, "ACC");
    if ((r -> memoire)[0] != INT_MIN)
    {
        mvwprintw(w, 1, pad + 1 + 7, "%d", (r -> memoire)[0]);
    } 


    /*
     * On affiche seulement les registres initialisés. cpt permet de
     * compter le nombre de registres initialisés.
     * Ainsi, si seuls les registres 1, 2 et 35 sont initialisés, on
     * affichera:
     * ACC   [valeur de l'ACC]
     * 1     [valeur du registre 1]
     * 2     [valeur du registre 2]
     * 35    [valeur du registre 35]
     */
    int i = 1;
    int cpt = 1;
    while (i < MEM_SIZE && cpt < height)
    {
        if (r -> memoire[i] != INT_MIN)
        {
            mvwprintw(w, cpt + 1, pad + 1, "%d", i);
            mvwprintw(w, cpt + 1, pad + 1 + 7, "%d", (r -> memoire)[i]);
            cpt++;
        }
        i++;
    }
   
    wrefresh(w);
}




void draw_bands(ram *r, liste l, int is_entree)
{
    /* Bande d'entrée. Les valeurs déjà lues sont grisées */
    char *l_str = list_to_str(l);

    int i;
    int tmp;
    int inc;
    int nb_spaces = 0;
    int flg = 1;
    size_t len = strlen(l_str);

    int y = is_entree ? 1 : LINES - 3;

    move(y, COLS / 2 - len / 2);
    attron(COLOR_PAIR(COLOR_GRAY));

    for (i = 0; i < len; i++)
    {

        if (l_str[i] == ' ')
        {
            nb_spaces++;
            flg = 1;
        }

        if (nb_spaces == r -> idx_e && flg)
        {
            attroff(COLOR_PAIR(COLOR_GRAY));
            tmp = getcurx(stdscr);

            /* Pour le 1er il ne faut pas décaler d'un */
            inc = (r -> idx_e == 0) ? 0 : 1;

            mvaddch(y + 1, tmp + inc, '^');
            move(y, tmp);
            flg = 0;
        }
        addch(l_str[i]);
    }

    free(l_str);
}



/**
 * @brief Retourne la couleur correspondant au numéro de l'instruction
 * passé en paramètre.
 * La valeur retournée doit-être l'identificateur d'une paire de couleur
 * initialisé via `init_pair`.
 * 
 * @param num_instr 
 * @return int 
 */
int get_instr_color(instr_ram num_instr)
{
    switch (num_instr)
    {
    case READ:
    case WRITE:
        return COLOR_INSTR_IO;
    
    case JUMP:
    case JUMZ:
    case JUML:
    case JUMG:
        return COLOR_INSTR_JMP;
    
    case STOP:
        return COLOR_STOP;
    default:
        return COLOR_INSTR_DFT;
    }
}


int get_adr_type_color(char adr_type)
{
    if (adr_type == '@') return COLOR_ADR_IND;
    else return COLOR_ADR_NUM;
}


/**
 * @brief Met à jour l'affichage de l'état de la machine RAM.
 * On utilise cpy_e car la "vraie" liste est modifiée au fur et à
 * mesure de l'exécution (on dépile lorsque l'on lit une valeur)
 * 
 * @param r Un pointeur sur la structure représentant la machine.
 * @param wi Un pointeur sur la fenêtre d'instruction
 * @param wm Un pointeur sur la fenêtre de mémoire
 * @param cpy_e L'état initial de la bande d'entrée
 */
void update_ui(ram *r, WINDOW *wi, WINDOW *wm, liste cpy_e)
{
    if (!need_refresh) return;

    erase();
    draw_infos(r);
    draw_bands(r, cpy_e, 1);
    draw_bands(r, r -> sortie.start, 0);
    draw_status_bar();
    refresh();
    draw_instr_window(wi, r);
    draw_mem_window(wm, r);

    need_refresh = 0;
}




void draw_infos(ram *r)
{
    char *logo1 = " ____      _    __  __ ";
    char *logo2 = "|  _ \\    / \\  |  \\/  |";
    char *logo3 = "| |_) |  / _ \\ | |\\/| |";
    char *logo4 = "|  _ <  / ___ \\| |  | |";
    char *logo5 = "|_| \\_\\/_/   \\_\\_|  |_|";

    char *e = "";
    char d[64];
    char c[64];
    char i[64];

    sprintf(c, "Compteur ordinal: %d", r -> compteur_ord);
    sprintf(i, "Instructions executees: %d", r -> nb_exec);
    sprintf(d, "Delai: %dms", r -> delay);


    char *to_print[9] = {logo1, logo2, logo3, logo4, logo5, e, d, c, i};

    for (int i = 0; i < 9; i++)
    {
        mvprintw(4 + i, COLS / 2 - strlen(to_print[i]) / 2, "%s", to_print[i]);
    }
}



/**
 * @brief S'occupe de gérer les actions des raccourcis clavier.
 * 
 * Permet également de savoir quand il faut mettre à jour l'affichage:
 * en effet, suite au problème avec le traitement de SIGWINCH mentionné
 * dans rsend_update_ui_sig, on simule le redimensionnement de la 
 * fenêtre via `resizeterm`, et lorsque ncurses traite ce changement, 
 * il envoie un caractère (KEY_RESIZE) qui sera donc détecté par le 
 * is_keypressed() (puis par le getch()).
 * 
 * cf. le man de `resizeterm`:
 * If ncurses is configured to supply its own SIGWINCH handler, the 
 * resizeterm function ungetch's a KEY_RESIZE which will be read on the 
 * next call to getch. This is used to alert an application that the 
 * screen size has changed, and that it should repaint special features 
 * such as pads that cannot be done automatically. 
 * 
 * On met donc à jour l'affichage à chaque touche détectée.
 * 
 * 
 * @param loop Un pointeur sur l'entier controllant la boucle principale
 */
void handle_keypresses(int *loop, ram *r)
{
    if (!is_keypressed()) return;

    int pressed = getch();
    switch (pressed)
    {
    case 'q':
        *loop = 0;
        break;
    
    /* Signale au thread de simulation de mettre en pause */
    case 'p':
        raise(SIGUSR1);     
        break;
    
    /* Signale au thread de simulation d'exécuter l'instruction suivante */
    case 'n':
        raise(SIGUSR2);
        break;

    case '+':
        r -> delay += 100;
        break;
    
    case '-':
        if (r -> delay >= 100) r -> delay -= 100;
        break;

    default:
        break;
    }

    need_refresh = 1;
}



void launch(char *e, char *filename)
{
    ram *r = init_ram(e);
    r -> nb_instr = lire_fichier(filename, r -> instructions);
    liste cpy_e = copy_liste(r -> entree.start);


    init_ui();

    /*
     * Largeur des fenêtres.
     * COLS - SPACE_BTWN_W est la place libre pour les fenêtres.
     * On divise par 2 car il y a 2 fenêtres (instructions et mémoire)"
     */
    int w_width = (COLS - SPACE_BTWN_W) / 2;
    
    WINDOW *instr_w = newwin(LINES - (3 + 2 + 1), w_width, 3, 3);
    WINDOW *mem_w = newwin(LINES - (3 + 2 + 1), w_width, 3, COLS - 3 - w_width);
    update_ui(r, instr_w, mem_w, cpy_e);
    napms(r -> delay);  /* Pour avoir le temps de voir la 1ère */

    pthread_create(&exec_thread_id, NULL, launch_simu_thread, r);

    int loop = 1;
    while (loop)
    {
        update_ui(r, instr_w, mem_w, cpy_e);
        handle_keypresses(&loop, r);
    }

    pthread_cancel(exec_thread_id);
    
    delwin(instr_w);
    delwin(stdscr);
    endwin();

    free_liste(cpy_e);
    free_ram(r);
}
