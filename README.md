# Machine RAM
Ce programme écrit en `C` permet de simuler et visualiser l'exécution
d'un programme écrit en [machine RAM][RAM]. \
Cette nouvelle version sera principalement utilisée dans le cadre du
module d'I53.\
La partie d'entrée sortie est gérée avec la librairie [`ncurses`][ncurses]




Utilisation de la commande: `ram [-e "x1, x2, ..., xn] fichier.ram`\
avec `x1, x2, ..., xn` les entiers à placer sur la bande d'entrée
(peuvent également être séparés simplement par des espaces).\
Le `fichier.ram` doit respecter la syntaxe suivante:
- une ligne ne doit pas faire + de 1024 caractères
- une seule instruction par ligne, qui doit être au début de la ligne
- le type d'adressage doit être séparé de l'instruction par au moins un
  espace (il peut y en avoir plusieurs).
- l'adresse peut-être collée au type d'adressage, ou bien séparée de ce
  dernier par un ou plusieurs espaces.
- tout ce qui est situé à droite d'un `;` sur la même ligne qu'une
  instruction est un commentaire
- il peut y avoir des lignes vides
- toutes les lignes commençant par un caractère non-alphabétique est un
  commentaire

### Raccourcis clavier
Les raccourcis clavier qui sont implémenté pour l'instant sont:
- `q` pour quitter le programme
- `p` pour mettre en pause l'exécution du programme
- `n` pour passer à l'instruction suivante lorsque le programme est en
  pause
- `+` et `-` pour augmenter / diminuer le délai entre l'exécution de 2
  instructions




[RAM]: https://zanotti.univ-tln.fr/ALGO/I31/MachineRAM.html
[ncurses]: https://invisible-island.net/ncurses/
