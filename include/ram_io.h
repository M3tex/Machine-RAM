#ifndef _RAM_IO_HEADER
#define _RAM_IO_HEADER


#include <ncurses.h>
#include "ram.h"


/*
 * On définit nos propres couleurs.
 * Attention ça n'est possible que si le terminal le supporte.
 * 
 * Il y a 8 couleurs par défaut dans ncurses, on commence donc à 8.
 * 
 * COLOR_INSTR_DFT -> la couleur par défaut pour les instructions
 * COLOR_INSTR_JMP -> la couleur pour JUMP et ses variantes
 * COLOR_INSTR_IO  -> la couleur pour READ et WRITE
 * COLOR_ADR_IND   -> la couleur du @
 * COLOR_ADR_NUM   -> la couleur du #
 * COLOR_STOP      -> la couleur pour STOP
 * COLOR_NB        -> la couleur des adresses
 */
#define COLOR_INSTR_DFT 8
#define COLOR_INSTR_JMP 9
#define COLOR_INSTR_IO 10
#define COLOR_STOP 11

#define COLOR_ADR_IND 12
#define COLOR_ADR_NUM 13
#define COLOR_NB 14
#define COLOR_GRAY 15


/* Paires de couleurs */
#define WHITEBG_BLACKFG 1
#define WHITEBG_WHITEFG 2
#define DEFAULTBG_GRAYFG 3


/* Espace entre les 2 fenêtres */
#define SPACE_BTWN_W 40


void init_ui();
void update_ui(ram *r, WINDOW *wi, WINDOW *wm, liste cpy_e);

void launch(ram *r);
void launch_minimal(ram *r);

void draw_status_bar();
void draw_instr_window(WINDOW *w, ram *r);
void draw_mem_window(WINDOW *w, ram *r);
void draw_bands(ram *r, liste e, int is_entree);
void draw_infos(ram *r);

int is_keypressed();
void handle_keypresses(int *loop, ram *r);

int get_instr_color(instr_ram num_instr);
int get_adr_type_color(char adr_type);

#endif