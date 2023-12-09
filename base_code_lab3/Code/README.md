## Choix d'implémentation

### 6.1 :

Toutes les **extensions** (notées _(extention)_ ) ci-dessous font partie de la représentation interne uniquement (aucune déclaration dans le `.h`).

### `s_SubList` et `SubList`
```c
struct s_SubList {
	LinkedElement* first;
	LinkedElement* last;
};

typedef struct s_SubList SubList;
```

La structure `s_SubList` doit être déclarée et implémentée dans le `.c`.
Cette structure n'est utile qu'à l'implémentation du tri fusion.

### `list_split`
```c
SubList list_split(SubList l)
```

La fonction doit être déclarée et implémentée dans le `.c`.
Cette fonction n'est utile qu'à l'implémentation du tri fusion.

### `list_merge`
```c
SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f)
```

La fonction doit être déclarée et implémentée dans le `.c`.
Cette fonction n'est utile qu'à l'implémentation du tri fusion.

### `list_mergesort`
```c
SubList list_mergesort(SubList sl, OrderFunctor f)
```

La fonction doit être déclarée et implémentée dans le `.c`.
Cette fonction n'est utile qu'à l'implémentation du tri fusion.

### `list_sort`
```c
List* list_sort(List* l, OrderFunctor f)
```

La fonction doit être déclarée dans le `.h` et implémentée dans le `.c`.
Cette fonction fait aussi partie de l'interface, c'est celle que l'utilisateur aura à appeler pour faire le tri fusion via la fonction `list_mergesort` après un court pré-traitement.

### `fprint_list` _(extension)_
```c
void fprint_list(FILE* outstream, List* l)
```

Affiche le contenu d'une liste donnée dans le flux de sortie souhaité.
Utile pour les tests et notamment avec Valgrind.

### `print_list` _(extension)_
```c
void print_list(List* l)
```

Affiche le contenu d'une liste donnée dans la sortie standard.
Utile pour les tests.

### `fprint_sublist` _(extension)_
```c
void fprint_sublist(FILE* outstream, SubList* sl)
```

Affiche le contenu d'une sublist donnée dans le flux de sortie souhaité.
Utile pour les tests et notamment avec Valgrind.

### `print_sublist` _(extension)_
```c
void print_sublist(SubList* sl)
```

Affiche le contenu d'une sublist donnée dans la sortie standard.
Utile pour les tests.

### `extract_sublist` _(extension)_
```c
SubList extract_sublist(List* l)
```

Extrait une sublist d'une liste donnée.
Cette opération a pour conséquence de couper le chaînage avec la liste donnée pour le rediriger sur une structure SubList simplifiée.

### `reconstruct_list_linking_from_sublist` _(extension)_
```c
List* reconstruct_list_linking_from_sublist(List* l, SubList* sl)
```

Permet de reconstruire le chaînage d'une liste à partir d'une sublist contenant tous les éléments chaînés à lié à la liste.
Utile pour les tests et l'implémentation finale de la fonction `list_sort`.

### `s_SubListSplit` et `SubListSplit` _(extension)_
```c
typedef struct s_SubListSplit {
	SubList left;
	SubList right;
} SubListSplit;
```

Structure annexe utilisée pour stocker le résultat de la fonction `reconstruct_sublists_from_split` de manière lisible et sans allocation mémoire supplémentaire.

### `reconstruct_sublists_from_split` _(extension)_
```c
SubListSplit reconstruct_sublists_from_split(SubList list_split_result)
```

Permet de reconstruire proprement deux sublists indépendantes à partir du résultat de la fonction `list_split`.
Les deux sublists sont renvoyés dans une structure `s_SubListSplit`.

### `delete_sublist` _(extension)_
```c
void delete_sublist(SubList* sl)
```

Permet de libérer la mémoire occupée par les éléments chaînés que contient une sublist donnée (utile pour les tests complémentaires).

## Comportement sur les jeux de test

Les fonctions implémentées passent les tests prévus sans problème.

Pour la partie 6, j'ai fait le choix de pousser plus loin les tests sur la représentation interne directement, pour les fonctions annexes qui permettent d'implémenter le tri fusion.

Ainsi, ces fonctions sont possiblement assez robuste par rapport à ce qui était attendu.

Ces tests complémentaires sont executables via le main en executant `./list_test 10`.

## Analyse personnelle

L'implémentation des premières fonction jusqu'à la partie 6 sur le tri fusion était assez usuel étant donné le travail fait en cours.

Mais pour implémenter le tri fusion pas à pas, j'ai rencontré plusieurs problème, toujours d'allocations mémoire, mais aussi de robustesse et de clareté. J'ai ainsi essayé de tester pas à pas chacune des fonctions essentielles en créant des fonctions d'affichage, des jeux de tests complémentaires spécifique à ces fonctions.

La partie la plus complexe restait la compréhension de l'implémentation :
- l'implémentation demandée de la fonction `list_split` m'a paru très contre-intuitive pendant un certain temps, ce qui m'a poussé à segmenter le problème et à tester les limites de cette fonction tout en essayant de l'optimiser (parcours de la liste en temps `n/2`, comportement face à des listes vides ou mal initialisées, ...)
- l'implémentation de la fonction `list_merge` était elle aussi peu claire. Plusieurs camarades et moi-même avons donc décidé d'implémenter une version en pseudo-code trouvée sur internet, mais dans un style impératif plutôt que récursif ([cf. source](https://fr.wikipedia.org/wiki/Tri_fusion#Algorithme)).

Une trace de ce travail de test reste présente à la toute fin du fichier `list.c` ou se trouve une fonction `list_tests`.

C'était intéressant d'étre confronté à de tels problèmes qui m'ont poussés à me remettre en question et à prendre du recul.